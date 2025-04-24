@echo off
REM This script's working directory may change depending on how uVision is launched
REM %~dp0 is the directory this script is stored in (Lab2\Scripts\)
REM thus, all paths are relative to this location
set PATH="%~dp0..\..\Tools\GetRegions";%PATH%
@echo on
GetRegions.exe "%1" -c -s -o"%~dp0..\Source\Profiler\region.c"
