A Graphic Study


After many years of an irrational (or perhaps rational) hesitation towards learning modern graphics API, I've decided to learn OpenGL, and it was pretty fun (mostly). In the effort to continue on the momentum that I've built up, I've decided to do a quick study of the major APIs and create the same basic demos across them all. My goal is that this should cement for me the foundational principles of 3D graphics and GPU processing while also giving me a basic familiarity with the common graphics APIs in use. 

Graphics tutorials are plentiful, but they typically rely on the use of external libraries adding dependencies (and steps!) to a project. I wanted to see how much I could do from the ground up, and so whenever possible I have tried to avoid adding libraries. We will see how long this will continue.  

This will be an ongoing project. The current goal is to create 5 demos for each of the popular APIs listed below:

1. - OpenGL Core
2. - Software Rendering - doing everything from scratch
3. - Vulcan
4. - DirectX 11
5. - DirectX 12 
6. - Metal

The 5 demos will be there same for each in order to have something of a control variable between the studies. I hope this will be conducive for comparing differences between the APIs.

The demos 

1. Color Triangle
 - The Hello World of graphics programming. This demo has the benefits of showing all the necessary boilerplate in order to coax the OS into using the hardware accelerated API as well as how to send vertex data to the GPU. 

2. Box Demo
- Adding to the last demo the ability to render perspective. 

3. OBJ Model Loading
- Loading a custom model with lighting and textures. 

4. Simple Interactive Game - Breakout
 - No texture loading. Just simple cubes, and simple physics, but all used to make a simple 3D game. 

5. Simple FPS walkthrough? 
    - Mouse and Keyboard interaction with lighting and skybox. 


BUILDING

Currently, I only have Windows builds at the moment. You'll need Visual Studio to compile them. The build.bat files will build the files, but it'll need to be able to find vsvarsall.bat, which is typically located in Program Files/Visual Studio. If you have a different version of Visual Studio, you'll need to modify the second line. 

