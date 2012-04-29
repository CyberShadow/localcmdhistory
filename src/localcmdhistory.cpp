#include <CRT/crt.hpp>
#include "plugin_123/plugin_123.hpp"
#include "version.hpp"

// {fa23d0ef-cfe1-4b70-97b1-87e07edc4a6a}
static const GUID MainGuid =
{ 0xfa23d0ef, 0xcfe1, 0x4b70, { 0x97, 0xb1, 0x87, 0xe0, 0x7e, 0xdc, 0x4a, 0x6a} };

ADD_GETGLOBALINFO;

static struct PluginStartupInfo StartupInfo;

void WINAPI WIDE_SUFFIX(SetStartupInfo)(const struct PluginStartupInfo *Info)
{
	StartupInfo = *Info;
}

void WINAPI WIDE_SUFFIX(GetPluginInfo)(struct PluginInfo *Info)
{
	static const wchar_t *MenuStrings[1];
	MenuStrings[0] = TEXT("Local Command History");

	Info->StructSize = sizeof(*Info);
	Info->Flags = PF_ALLPANELEVENTS;
  	Info->PluginMenu.Guids = &MainGuid;
  	Info->PluginMenu.Strings = MenuStrings;
	Info->PluginMenu.Count = _countof(MenuStrings);
}

// ***************************************************************************

struct HistoryNode
{
	HistoryNode *pNext;
	LPCTSTR szCmdLine;
};

// First node is last new command
HistoryNode* history;

HistoryNode** findInHistory(LPCTSTR szCmdLine, HistoryNode** ppPrev = NULL)
{
	HistoryNode** ppNode = &history;
	if (ppPrev)
		*ppPrev = NULL;

	while (*ppNode && wcscmp(szCmdLine, (*ppNode)->szCmdLine))
	{
		if (ppPrev)
			*ppPrev = *ppNode;
		ppNode = &(*ppNode)->pNext;
	}

	return ppNode;
}

void addToHistory(LPCTSTR szCmdLine)
{
	HistoryNode** ppExisting = findInHistory(szCmdLine);
	HistoryNode* pNode;
	if (*ppExisting)
	{
		pNode = *ppExisting;
		*ppExisting = pNode->pNext;
	}
	else
	{
		pNode = new HistoryNode;
		pNode->szCmdLine = wcsdup(szCmdLine);
	}
	pNode->pNext = history;
	history = pNode;
}

// ***************************************************************************

int WINAPI WIDE_SUFFIX(ProcessPanelEvent)(const struct ProcessPanelEventInfo *Info)
{
	if (Info->Event == FE_COMMAND)
	{
		LPCTSTR szCmdLine = (LPCTSTR)Info->Param;
		addToHistory(szCmdLine);
	}
	return FALSE;
}

// {026cfc47-f1e1-436f-616d-97aaf029635d}
static const GUID MenuGuid =
{ 0x026cfc47, 0xf1e1, 0x436f, { 0x61, 0x6d, 0x97, 0xaa, 0xf0, 0x29, 0x63, 0x5d} };

OPENPLUGIN
{
	static const FarMenuItem MenuItems[] = {
		{0, TEXT("Previous command"), {}, 0, 0},
		{0, TEXT("Next command"    ), {}, 0, 0},
	};
	int iChoice = StartupInfo.Menu(&MainGuid, &MenuGuid, -1, -1, 0, FMENU_AUTOHIGHLIGHT | FMENU_WRAPMODE, TEXT("Local Command History"), NULL, NULL, NULL, NULL, MenuItems, _countof(MenuItems));
	if (iChoice >= 0)
	{
		INT_PTR szBufSize = StartupInfo.PanelControl(PANEL_ACTIVE, FCTL_GETCMDLINE, 0, NULL);
		LPTSTR szCmdLine = new TCHAR[szBufSize];
		StartupInfo.PanelControl(PANEL_ACTIVE, FCTL_GETCMDLINE, szBufSize, szCmdLine);
		HistoryNode* pPrev = NULL;
		HistoryNode* pNode = *findInHistory(szCmdLine, &pPrev);
		if (iChoice==0) // previous
		{
			if (wcslen(szCmdLine)==0)
			{
				if (history)
					StartupInfo.PanelControl(0, FCTL_SETCMDLINE, 0, (void*)history->szCmdLine);
			}
			else
			{
				if (pNode && pNode->pNext)
					StartupInfo.PanelControl(0, FCTL_SETCMDLINE, 0, (void*)pNode->pNext->szCmdLine);
			}
		}
		else
		{
			if (pPrev)
				StartupInfo.PanelControl(0, FCTL_SETCMDLINE, 0, (void*)pPrev->szCmdLine);
			else
			{
				addToHistory(szCmdLine);
				StartupInfo.PanelControl(0, FCTL_SETCMDLINE, 0, (void*)TEXT(""));
			}
		}
		delete[] szCmdLine;
	}

	return NULL;
}
