echo off

set COMPILE_TIME=%date:~10,4%/%date:~4,2%/%date:~7,2%-%time:~0,8%
set GITVER=""
for /f "tokens=*" %%a in ('git rev-parse HEAD') do set GITVER=%%a
::del version.c

if exist %1_version.c (
del %1_version.c
)

::pause
::echo #include ^<section_config.h^> > %1_version.c
::echo SECTION(".rodata.version") >> %1_version.c
::echo #if defined(__GNUC__) >> %1_version.c
::echo __attribute((used))  >> %1_version.c
::echo #elif defined(__ICCARM__) >> %1_version.c
::echo __root >> %1_version.c
::echo #endif >> %1_version.c
echo const char %1_rev[] = "%1_ver_%GITVER%_%COMPILE_TIME%"; >> %1_version.c

::pause