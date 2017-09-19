A Graphic Study


After many years of an irrational (or perhaps rational) hesitation towards learning modern graphics API, 
I took the plunge to understand OpenGL. Parts were better than expect and others worse. Nevertheless, success in the endeavour showed me that with enough patience, grit and williness to spending a few hours every evening learning arcane symbols and math, I could understand whatever I wanted. 

Perhaps the most painful part of the experience was simply to piece together the information I desired from a variety of sources. Graphics API tutorials are plentiful, but most sacrifice thoroughness with the need for ease and accessibiity (which I fault no one for!). Indeed you can never go too far without adding another library to "ease the pain", but I was determined to string together everything with absolute minimum dependency. 

But now here you will be able find simple examples of the popular graphics APIs in order to gain a decent understanding of how to get started with graphics programming. 

This will likely be an ongoing project for sometime until I've made a journey through the selected APIs (or until new ones arise).  

The goal currently as it stands is to create 5 samples for each of the popular APIs listed below:

1 - OpenGL Core
2 - Software Rendering - where all the mysteries of graphics will be made manifest
3 - Vulcan
4 - DirectX 11
5 - DirectX 12 
6 - Metal

The five samples are intended to be as basic as you can get, but they contain little to no dependencies. No dependencies to any Windows or Math libraries here. See all the necessary calls to Windows in order to get the hardware accelerated pipeline setup. 


Demo Samples
Each API folder contains 5 demos for the purposes of comparing the APIs against each other

1. Color Triangle
 - The Hello World of graphics programming. This demo has the benefits of showing all the necessary boilerplate in order to coax the OS into using the hardware accelerated API as well as how to send triangle data to the GPU. 

2. Box Demo
- The basic functionality in which to render a 3D perspective scene with simple boxes. 

3. OBJ Model Loading
- Using a custom but portable OBJ loader, pipe the data to the graphics card

4. Simple Interactive Game - Breakout
 - No texture loading. Just simple cubes, and simple physics, but all used to make a simple 3D game. 

5. Simple FPS walkthrough with no interaction? 
    Skybox? OBJ loading? Definitely no collisions 
