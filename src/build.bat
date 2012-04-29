@echo off
set MAKER=nmake -f makefile_vc
set DIRSUFF=DIRSUFF

if "%FAR3HOME%"=="" set FAR3HOME=../../../FAR

%MAKER% FARDEV=%FAR3HOME%/fardev DEF_NAME=localcmdhistory_3 %DIRSUFF%=.Far3 FAR3=1

goto :eof
