@echo off
setlocal

cmake --preset windows-default
cmake --build --preset windows-default

if "%~2"=="" (
    echo Usage: %~nx0 {-edit^-|-map} ^<map_name^>
    exit /b 1
)

if not "%~3"=="" (
    echo Usage: %~nx0 {-edit^-|-map} ^<map_name^>
    exit /b 1
)

set "MODE=%~1"
set "MAP_NAME=%~2"

if /I "%MODE%"=="-edit" goto run
if /I "%MODE%"=="-map" goto run

echo Error: first argument must be -edit or -map
echo Usage: %~nx0 {-edit^-|-map} ^<map_name^>
exit /b 1

:run
build\map-editor.exe "%MODE%" "%MAP_NAME%"