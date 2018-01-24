/* ==========================================================
$File:    BoxDemo.cpp
$Revision v 1.1
$Creator: Keenan Cole
$Notice: 
============================================================*/

/* A STUDY IN GRAPHICS
  
  API :  OpenGL 
  DEMO:  FPSDEMO
  The minimum lines of code needed to
  1: Initialize OpenGL from Windows
  2: Store a Cube as Vertex Information in GL Buffers
  3: Build the Necessary Matrix Information for the Shaders
  4: ....

*/

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <Windowsx.h>
#include "OpenGL.h"
#include "Math.h"

using namespace std;
// UTILITY CONSTANTS
#define ArrayLength(value) ( sizeof(value) / sizeof(value[0] ) ) //ArrayLength of an Array


static int WindowWidth  = 800; 
static int WindowHeight = 600; 



// Simple Camera Structure
// Needs to be a class now
struct Camera {
	Vector3 position = Vector3(0, 0,   -5.0f); 
	Vector3 forward  = Vector3(0,  0, -1.0f); // what we are looking at// direction facing // or could think of it as rotation I suppose. 

	float fieldOfView =  60.0f; 
	float nearClip = .1f; 
	float farClip  = 100.0f; 
};

Camera camera; 





 Vector3 modelPosition = Vector3(.1f, 0, 0);

 bool Running = false;

static bool moveForwardState = false;
static bool moveBackState = false; 
static bool moveRightState = false;
static bool moveLeftState = false;

 void ProcessMouseInput(HWND &WindowHandle, WPARAM wParam, LPARAM lParam ) {
	float mouseSensitivity = 10.0f;
	//Center of Window
	POINT cPoint; 
    cPoint.x =  WindowWidth/2;
    cPoint.y =  WindowHeight/2;
    ClientToScreen(WindowHandle, &cPoint);

    //Mouse Pos from Window	
	long xPos = GET_X_LPARAM(lParam); 
	long yPos = GET_Y_LPARAM(lParam); 
	POINT mousePoint;
	mousePoint.x = xPos;
	mousePoint.y = yPos;
    ClientToScreen(WindowHandle, &mousePoint);

    //Get Movement from Center
	float movementX = float( cPoint.x - mousePoint.x) ;//(lastMouseX ));//*10.0f; 
	float movementY = float( cPoint.y - mousePoint.y) ;//(lastMouseY ));//*10.0f; 

	//Massage it and put it in delta
	if(movementX != 0)
	mouseXDelta = -movementX / mouseSensitivity;

	if(movementY != 0)
	mouseYDelta = -movementY / mouseSensitivity;

	//Set cursor back to center
   SetCursorPos(cPoint.x, cPoint.y);//WindowWidth/2, WindowHeight/2);
}

 //RENDER AND DRAW EVERYTHING
 void RenderOpenGLContext(GLuint shader)
 {
 	//PREPARE CAMERA AND MATRICES
 	Matrix4 model = Matrix4(); //Constructor sets to Identity
 	Matrix4 view = Matrix4();
 	Matrix4 proj = Matrix4();

 	view = LookAt(camera.position,  camera.position + camera.forward, Vector3(0, 1.0f, 0.0f) );
 	proj = Perspective( (camera.fieldOfView * DEG2RAD),  ( (float) WindowWidth / (float)WindowHeight ), camera.nearClip, camera.farClip); 

 	glUseProgram(shader); 

 	GLuint shader_model_id = glGetUniformLocation(shader, "model"); 
 	glUniformMatrix4fv(shader_model_id, 1, GL_FALSE,  &model.m[0]);

 	GLuint shader_view_id = glGetUniformLocation(shader, "view"); 
 	glUniformMatrix4fv(shader_view_id, 1, GL_FALSE, &view.m[0] );

 	GLuint shader_proj_id = glGetUniformLocation(shader, "proj"); 
    glUniformMatrix4fv(shader_proj_id, 1, GL_FALSE,   &proj.m[0]);   //cd);//&proj.m[0]);    value_ptr(projMatrix)

 	glBindVertexArray(VAO);

 	for (int i = 0; i < ArrayLength(mPositions); ++i)
 	{
 		 model.identity();
 		 model = model.RotateZAxis( modelPosition.x * 30.0f);
 		 model = model.RotateYAxis( modelPosition.y * 30.0f); // = model.RotateYAxis(modelPosition.x*50.0f);
 		// model = model.RotateXAxis( modelPosition.x * 30.0f);
 		 model.Translate( mPositions[i] );
 		 
 		 glUniformMatrix4fv(shader_model_id, 1, GL_FALSE,  &model.m[0]);
 		 glDrawArrays( GL_TRIANGLES, 0 , 36 );
 	}

 	glBindVertexArray(0);
 }


void ProcessKeyEventPress( bool isDown, bool wasDown, bool *controlState)
{
	if(isDown && !wasDown){
		*controlState = true;
	}
	else if(!isDown && wasDown) {
		*controlState = false;
	} 
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch( uMsg)
	{
		case WM_CREATE: 
		{
			//Set Cursor to the middle of the screen
			POINT cPoint; 
			//RECT rcClient; 
			//GetClientRect(hwnd, &rcClient);
			cPoint.x = WindowWidth/2;
			cPoint.y = WindowHeight/2;
			ClientToScreen(hwnd, &cPoint);
			

			SetCursorPos(cPoint.x, cPoint.y);//WindowWidth/2, WindowHeight/2);
			ShowCursor(false);		
		}
		break;
		case WM_DESTROY:{
			ShowCursor(true);

			PostQuitMessage(0);
			return 0;
		}

		default: 
		{
			return DefWindowProc(hwnd, uMsg, wParam ,lParam);
		}break;

	}

	return result;
 
}

float DeltaTime;

struct Time {

	float DeltaTime; 
	LARGE_INTEGER lastCount; 
	LARGE_INTEGER PerformanceFrequencyResult; 
	int PerformanceFrequency;
	
	void Initialize(){
		QueryPerformanceCounter(&lastCount); 
		QueryPerformanceFrequency(&PerformanceFrequencyResult);
	}

	void Update(){

		LARGE_INTEGER currentCount; 
		QueryPerformanceCounter( &currentCount);
		long timeElapsed = (currentCount.QuadPart - lastCount.QuadPart );
		lastCount = currentCount;
		DeltaTime = (float) timeElapsed /(float) PerformanceFrequency;
	}
};


void Initialize(){

	LARGE_INTEGER lastCount; 
	LARGE_INTEGER PerformanceFrequencyResult; 
	int PerformanceFrequency;
	QueryPerformanceCounter(&lastCount); 
	QueryPerformanceFrequency(&PerformanceFrequencyResult);
	PerformanceFrequency = PerformanceFrequencyResult.QuadPart;
	
	//MSG msg = {}; 
	float lastMouseX   = 0, lastMouseY = 0;
	float ignoreDeltaX = 0, ignoreDeltaY = 0;
}

void FPSLoop(){

		yaw   += mouseXDelta * DeltaTime * 40;
		pitch += mouseYDelta * DeltaTime * 40;

  		float moveSpeed = 5.0f;
	    if(moveForwardState == true) {
			camera.position += camera.forward * DeltaTime * moveSpeed;
		}

		if(moveBackState == true) {
			camera.position -= camera.forward * DeltaTime * moveSpeed;
		}

		if(moveRightState)
			camera.position += GetRight() * DeltaTime * moveSpeed;

		if(moveLeftState)
			camera.position -= GetRight() * DeltaTime * moveSpeed;

        ProcessPitch();
	    ProcessYaw();
}

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR     CommandLine,
    int       WindowShow
)
{
	//Create and Register WindowClass	
	WNDCLASS         wc = {}; 
	wc.lpfnWndProc      = WindowProc; 
	wc.hInstance        = Instance; 
	wc.lpszClassName    = "Windows Test";

	RegisterClass(&wc);
	//Create Window	
	HWND WindowHandle = CreateWindowEx( 0, 
					    "Windows Test", 
					    "Learn to Window Program",
					     WS_OVERLAPPEDWINDOW,
				     	 CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight, 
				     	 NULL, NULL, Instance, NULL ) ; //Get Handle to Window

	if( WindowHandle == NULL) { return 0; }

	ShowWindow(WindowHandle, WindowShow);

	Initialize();

	SetCapture(WindowHandle);

	Running = true;
	while(Running ) 
	{
		MSG msg; 

		mouseYDelta = 0;
		mouseXDelta = 0;

		HandleWindowsMessages(WindowHandle);	
		HDC DeviceContext = GetDC(WindowHandle);

		//Clear Screen to a Dark Grey
		glViewport(0,0, WindowWidth, WindowHeight);
		glClearColor(0.3f, .3f, .3f, 0.3f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

		FPSLoop();
		
		RenderOpenGLContext(Shader);
		SwapBuffers(DeviceContext);


		LARGE_INTEGER currentCount; 
		QueryPerformanceCounter( &currentCount);
		long timeElapsed = (currentCount.QuadPart - lastCount.QuadPart );
		lastCount = currentCount;

		DeltaTime = (float) timeElapsed /(float) PerformanceFrequency;
		//SetCursorPos( (float)WindowWidth/2.0f, (float)WindowHeight/2.0f);

	}

	ReleaseCapture();
	return 0;
}