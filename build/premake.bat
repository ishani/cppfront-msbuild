@echo off
cd /D "%~dp0"

"../ext/premake-bin/win64/premake5.exe" --file=premake.lua vs2022

pause
