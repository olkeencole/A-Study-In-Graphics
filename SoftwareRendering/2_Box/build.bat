@echo off

if "%1" == "1" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

mkdir build
pushd build
cl  -FC -O2 -Zi /I include  ..\BoxDemo.cpp -W2 -wd4312 user32.lib  gdi32.lib kernel32.lib opengl32.lib 
popd