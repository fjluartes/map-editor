@echo off
cmake --preset windows-default
cmake --build --preset windows-default
for /f "delims=" %%A in ('some_command') do (
    build\map-editor.exe %%A
)