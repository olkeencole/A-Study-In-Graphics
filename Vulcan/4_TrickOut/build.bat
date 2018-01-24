@echo off
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

if "%1" == "1" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

mkdir build
pushd build
cl  -FC -Zi /I C:\VulkanSDK\1.0.65.0\Include\ /I ..\include  ..\vkBox.cpp -W2 -wd4312 user32.lib  ..\library\glfw3dll.lib  gdi32.lib kernel32.lib  C:\VulkanSDK\1.0.65.0\Lib\vulkan-1.lib
popd

REM is it an iteration of a previous form or an improvement over the core functionality
