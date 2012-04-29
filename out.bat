@echo off
if exist out rmdir /S /Q out
mkdir out
for %%a in (COPYING *.txt src\*.ini) do copy %%a out\
for %%b in (32 64) do for %%f in (2 3) do (
  copy src\final.%%b.Far%%f.vc\localcmdhistory.dll out\localcmdhistory.far%%f.x%%b.dll
)
mkdir out\src
mkdir out\src\plugin_123
copy src\* out\src\
copy src\plugin_123\* out\src\plugin_123\
if exist localcmdhistory.7z del localcmdhistory.7z
cd out
7z -mx9 a ..\localcmdhistory.7z *
cd ..
