@echo off
:: Helpful resources: 
:: 		https://www.tutorialspoint.com/batch_script/batch_script_arrays.htm
::		https://ss64.com/nt/syntax-arrays.html

cls
Setlocal EnableDelayedExpansion

set SrcPath=..\Include
set DstPath=..\..\Project_Base\Include
robocopy !SrcPath! !DstPath! *.c *.h /S 


set SrcPath=..\Source
set DstPath=..\..\Project_Base\Source
robocopy !SrcPath! !DstPath! *.c *.h /S 

set FileName[0]=control.c
set FileName[1]=LCD\LCD_text.c
set FileName[2]=LCD\ST7789.c
set FileName[3]=threads.c
set FileName[4]=UI.c
set FileName[5]=control.h

for /l %%n in (0,1,5) do (
		:: echo !SrcPath!\!FileName[%%n]! to !DstPath!\!FileName[%%n]!
	call :Filtered_Copy !SrcPath!\!FileName[%%n]!, !DstPath!\!FileName[%%n]!
)

EXIT /B %ERRORLEVEL%

:Filtered_Copy 
echo From %~1 to %~2
findstr /L /V /C:" SOLUTION" %~1 > %~2
EXIT /B 0 