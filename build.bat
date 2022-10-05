@echo off

SETLOCAL 
SET BuildDebug=0

:parseparams
IF "%~1"=="" GOTO endparse
IF /I "%~1"=="-debug" SET BuildDebug=1
SHIFT /1
GOTO parseparams
:endparse


IF %BuildDebug%==1 (
    SET build_type=DEBUG
    SET common_options=/Zi /Od /EHsc /fp:fast /GR- /arch:AVX /nologo /W4 /WX /wd4054 /wd4055 /wd4100 /wd4127 /wd4201 /wd4996 /DVK_USE_PLATFORM_WIN32_KHR /DAQUA_DEBUG=1
) ELSE (
    REM TODO: configure release options
    SET build_type=RELEASE
    SET common_options=/Zi /O2 /EHsc /fp:fast /GR- /arch:AVX /nologo /W4 /WX /wd4054 /wd4055 /wd4100 /wd4127 /wd4201 /wd4996 /DVK_USE_PLATFORM_WIN32_KHR /DAQUA_DEBUG=0
)


pushd %~dp0

CALL setup_path.bat

IF not exist deploy mkdir deploy
IF not exist build mkdir build
pushd build


echo.
echo ---------------------------------------
echo -[winmain-%build_type%]--------------------%derpderp%
echo.

SET SOURCES=..\code\main.cpp ..\libs\dearimgui\backends\imgui_impl_sdl.cpp ..\libs\dearimgui\backends\imgui_impl_opengl3.cpp ..\libs\dearimgui\imgui*.cpp
SET INCLUDES=/I..\libs\dearimgui /I..\libs\SDL2-devel-2.24.0-VC\SDL2-2.24.0\include
SET LIBS=Advapi32.lib user32.lib Shell32.lib opengl32.lib ..\libs\SDL2-devel-2.24.0-VC\SDL2-2.24.0\lib\x64\SDL2main.lib ..\libs\SDL2-devel-2.24.0-VC\SDL2-2.24.0\lib\x64\SDL2.lib

cl %common_options% /Fe..\deploy\aqua %SOURCES% %INCLUDES% /link /subsystem:windows /INCREMENTAL:NO %LIBS%

echo.
echo.

popd
popd