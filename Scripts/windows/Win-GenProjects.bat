@echo off
pushd %~dp0\..\..\
call vendor\premake\bin\premake5.exe vs2022 --file=Build.lua
popd

if "%1"=="nopause" goto end
PAUSE
:end