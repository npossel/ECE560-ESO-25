@echo off
REM Creates list of function names and sizes, and a sorted version.
REM This script's working directory may change depending on how uVision is launched
REM %~dp0 is the directory this script is stored in (Lab2\Scripts\)
REM thus, all paths are relative to this location
set PATH="%~dp0..\..\Tools\GetRegions";%PATH%
@echo on
GetRegions.exe "%1" -z -o"%~dp0..\Objects\function_sizes.txt"
sort /R "%~dp0..\Objects\function_sizes.txt" /O "%~dp0..\Objects\sorted_function_sizes.txt"
