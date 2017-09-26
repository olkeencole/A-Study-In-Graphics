@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

cl  -FC -Zi /I include FPSDemo.cpp -W2 -wd4312 user32.lib  gdi32.lib kernel32.lib opengl32.lib 
popd

