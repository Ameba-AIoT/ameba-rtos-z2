::echo TARGET_DIR %1
::echo PROJ_DIR %2
::echo PROJ_NAME %3
::echo IAR_TOOL_DIR %4

::pause
cd /D %2

set tooldir=%2\..\..\..\component\soc\realtek\8710c\misc\iar_utility
set libdir=%2\..\..\..\component\soc\realtek\8710c\misc\bsp\lib\common\IAR
set projname=%~3
set iartooldir=%4

%iartooldir%\bin\iccarm.exe --cpu Cortex-M33 %projname%_version.c -o Debug\Obj\%projname%\%projname%_version.o
%iartooldir%\bin\iarchive.exe -r %libdir%\%projname%.a Debug\Obj\%projname%\%projname%_version.o

::pause
exit /b
