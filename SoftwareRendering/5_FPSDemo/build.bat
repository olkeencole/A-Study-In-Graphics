@echo off
if not defined DevEnvDir (
rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
)
cl  -O2 -FC -Zi /I include SoftGL.cpp -W2 -wd4312 user32.lib  gdi32.lib kernel32.lib
popd

