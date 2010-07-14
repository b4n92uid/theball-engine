@echo off
set OUTPUT=tbe-1.0.%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%.7z
title Making %OUTPUT%
if exist %OUTPUT% del %OUTPUT%
7z a %OUTPUT% * -mx=9 -mtc=on -xr!build -x!doc/html -xr!sourcesold -xr!versions -xr!*.lnk -xr!*.bat -xr!*.tdl
echo End of process
pause > nul
