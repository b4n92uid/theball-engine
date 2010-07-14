@echo off
title Make all demos

del bin\*.exe

for /D %%i in (*.*) do (
	if exist %%i/Makefile (
		cls
		echo **** Make %%i ****
		echo.
		
		cd %%i
		make all
		
		cd ..
		if not exist bin\%%i.exe goto error
	)
)

echo Process complete !
goto eof

:error
echo Error occurd
goto eof

:eof
pause > nul
