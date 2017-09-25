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
  4: Render a Series of Boxes in Perspectival Rendering

*/

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <Windowsx.h>
#include "OpenGL.h"
#include "Math.h"

#define GLM_FORCE_RADIANS
#include "glm\glm.hpp"
#include "glm\gtx\random.hpp"
#include "glm\gtc\random.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
using namespace std;
// UTILITY CONSTANTS
#define Length(value) ( sizeof(value) / sizeof(value[0] ) ) //Length of an Array

static int WindowWidth  = 800; 
static int WindowHeight = 600; 

struct Camera {
	Vector3 position = Vector3(0, 0,   -5.0f); 
	Vector3 forward  = Vector3(0,  0, -1.0f); // what we are looking at// direction facing // or could think of it as rotation I suppose. 

	float fieldOfView =  60.0f; 
	float nearClip = .1f; 
	float farClip  = 100.0f; 
};

Camera camera; 
Vector3 GetRight(){
	return Cross(camera.forward, Vector3(0, 1,  0) );
}

float pitch = 0; 
float yaw = 0; 

float zRotation = 0;

float mouseXDelta = 0; 
float mouseYDelta = 0;

 Vector3 mPositions[] = {
  Vector3( 0.0f,  0.0f,  0.0f), 
  Vector3( 2.0f,  5.0f, -15.0f), 
  Vector3(-1.5f, -2.2f, -2.5f),  
  Vector3(-3.8f, -2.0f, -12.3f),  
  Vector3( 2.4f, -0.4f, -3.5f),  
  Vector3(-1.7f,  3.0f, -7.5f),  
  Vector3( 1.3f, -2.0f, -2.5f),  
  Vector3( 1.5f,  2.0f, -2.5f), 
  Vector3( 1.5f,  0.2f, -1.5f), 
  Vector3(-1.3f,  1.0f, -1.5f),

  Vector3(-3.7f,  3.0f, -7.5f),  
  Vector3( 3.3f, -2.0f, -2.5f),  
  Vector3( 4.5f,  4.0f, -8.5f), 
  Vector3( -4.5f,  5.2f, -9.5f), 
  Vector3(-3.3f,  -5.0f, -15.5f),

  Vector3(-1.7f, -3.0f, -7.5f),  
  Vector3( 1.3f, -2.0f, -2.5f),  
  Vector3( 4.5f, -4.0f, -4.5f), 
  Vector3( 2.5f,  4.2f, -7.5f), 
  Vector3(-2.3f,  -5.0f, -7.5f),

  Vector3(-4.7f, -3.3f,  -7.5f),  
  Vector3( 2.332f, -2.6f, -3.5f),  
  Vector3( 3.2f, -2.7f, -4.5f), 
  Vector3(-3.5f,  3.2f, -7.5f), 
  Vector3(-2.3f,  -2.43f, -7.5f) ,

  Vector3( 6.0f,  0.7f, -4.5f), 
  Vector3(-5.5f,  -0.2f, -7.5f), 
  Vector3( 6.1f,  -1.43f, -7.5f) ,
  Vector3(-7.2f,  -1.2f, -7.5f)
 };
 Vector3 modelPosition = Vector3(.1f, 0, 0);


void ProcessPitch() {
        camera.forward.x = cos(DEG2RAD * pitch);
        camera.forward.y = sin(DEG2RAD * pitch);
		camera.forward.z = cos(DEG2RAD * pitch);
		Normalize(camera.forward);
		
}
void ProcessYaw() {
		camera.forward.x *= cos(DEG2RAD * yaw);
		camera.forward.z *= sin(DEG2RAD * yaw);
		//Normalize(camera.forward);
	}

glm::vec3 Convert(Vector3 v){

	return glm::vec3(v.x, v.y, v.z);
}

 //Draw Function
 void RenderOpenGLContext(GLuint shader)
 {
 	//PREPARE CAMERA AND MATRICES
 	Matrix4 model = Matrix4(); //Constructor sets to Identity

 	Matrix4 view = Matrix4();
 	view.identity();
 	view = LookAt(camera.position,  camera.position  + camera.forward, Vector3(0, 1.0f, 0.0f) );

 	glm::mat4 viewMatrix; 
 	viewMatrix = glm::lookAt( Convert(camera.position),  Convert(camera.position  + camera.forward), Convert( Vector3(0, 1.0f, 0.0f)) );

 	Matrix4 proj = Matrix4();
 	proj = Perspective( (camera.fieldOfView * DEG2RAD),  ( (float) WindowWidth / (float)WindowHeight ), camera.nearClip, camera.farClip); 

 	glUseProgram(shader); 

 	GLuint shader_model_id = glGetUniformLocation(shader, "model"); 
 	glUniformMatrix4fv(shader_model_id, 1, GL_FALSE,  &model.m[0]);


 	GLuint shader_view_id = glGetUniformLocation(shader, "view"); 
 	glUniformMatrix4fv(shader_view_id, 1, GL_FALSE, &view.m[0] );

 	GLuint shader_proj_id = glGetUniformLocation(shader, "proj"); 
    glUniformMatrix4fv(shader_proj_id, 1, GL_FALSE,   &proj.m[0]);   //cd);//&proj.m[0]);    value_ptr(projMatrix)

 	glBindVertexArray(VAO);

 	for (int i = 0; i < Length(mPositions); ++i)
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


//Mouse and keyboard new input system
struct game_button{
	bool up; 
	bool down; 
}; 

bool GetKeyDown(int keycode){
		return false;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg)
	{
		case WM_CREATE: 
		{
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
				32,                        //Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                        //Number of bits for the depthbuffer
				8,                        //Number of bits for the stencilbuffer
				0,                        //Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC DeviceContext = GetDC(hwnd);
			int letWindowChoosePixelFormat; 
			letWindowChoosePixelFormat = ChoosePixelFormat(DeviceContext, &pfd ); // DC and PixelFormatDescriptor 
			SetPixelFormat(DeviceContext,  letWindowChoosePixelFormat, &pfd); //is pfd correct? 

			HGLRC renderContext = wglCreateContext(DeviceContext ); // dummy context
			
			if( wglMakeCurrent( DeviceContext, renderContext) )
			{
				cout << "Making WGL Current and attempting to fetch functions" << endl;
				
				//Misspelling: wglCreateContextAttribsARB
				wgl_create_attrib_arb* wglCreateContextAttribARB = (wgl_create_attrib_arb*)
				 				        wglGetProcAddress("wglCreateContextAttribsARB"); 
				   
				glGenVertexArrays 		  = (gl_GenVertexArrays*) 	       wglGetProcAddress("glGenVertexArrays");
				glBindVertexArray 		  = (gl_BindVertexArray*) 	       wglGetProcAddress("glBindVertexArray"); 
    			glGenBuffers      		  = (gl_GenBuffers* ) 		       wglGetProcAddress("glGenBuffers");
    			glBindBuffer      		  = (gl_BindBuffer*)  		       wglGetProcAddress("glBindBuffer"); 
    			glBufferData      		  = (gl_BufferData* )              wglGetProcAddress("glBufferData"); 
    			glVertexAttribPointer 	  = (gl_VertexAttribPointer*)      wglGetProcAddress("glVertexAttribPointer"); 
    			glEnableVertexAttribArray = (gl_EnableVertexAttribArray* ) wglGetProcAddress("glEnableVertexAttribArray");

				glAttachShader  = (gl_AttachShader *) wglGetProcAddress("glAttachShader"); 
				glCompileShader = (gl_CompileShader*) wglGetProcAddress("glCompileShader"); 
				glCreateProgram = (gl_CreateProgram*) wglGetProcAddress("glCreateProgram"); 
				glCreateShader  = (gl_CreateShader *) wglGetProcAddress("glCreateShader"); 
				glDeleteShader  = (gl_DeleteShader *) wglGetProcAddress("glDeleteShader"); 
				glShaderSource  = (gl_ShaderSource *) wglGetProcAddress("glShaderSource"); 
				glLinkProgram   = (gl_LinkProgram  *) wglGetProcAddress("glLinkProgram"); 
				glUseProgram    = (gl_UseProgram   *) wglGetProcAddress("glUseProgram"); 

				glGetUniformLocation = (gl_GetUniformLocation *)wglGetProcAddress("glGetUniformLocation");

				glGetProgramiv       = (gl_GetProgramiv      *) wglGetProcAddress("glGetProgramiv"); 
				glGetProgramInfoLog  = (gl_GetProgramInfoLog *) wglGetProcAddress("glGetProgramInfoLog"); 
				glGetShaderiv        = (gl_GetShaderiv       *) wglGetProcAddress("glGetShaderiv"); 
				glGetShaderInfoLog   = (gl_GetShaderInfoLog  *) wglGetProcAddress("glGetShaderInfoLog"); 
				glUniformMatrix4fv   = (gl_UniformMatrix4fv *)  wglGetProcAddress("glUniformMatrix4fv");

				if(wglCreateContextAttribARB ) {
						OutputDebugStringA( "Found Contrext Attrib Function\n" );
				        // MessageBoxA(0 , (char*) ("Found Context Attrib Function"), "OPENGL_VERSION", 0);

					const int attribs[] = {
						WGL_CONTEXT_MAJOR_VERSION_ARB, 3, 
						WGL_CONTEXT_MINOR_VERSION_ARB, 3,
						WGL_CONTEXT_FLAGS_ARB,         0, 
						WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
						0
					};

					HGLRC ShareContext = 0; 
					HGLRC modernRC = wglCreateContextAttribARB(DeviceContext, ShareContext, attribs);
					if(modernRC)
					{
						wglMakeCurrent( DeviceContext, modernRC);
						wglDeleteContext(renderContext);
						GlobalRenderContext  = modernRC; 

						OutputDebugString( "Created a Modern OpenGL Context\n" );
						InitializeOpenGLBuffers();
						InitShader();
					}
					else
					{
						OutputDebugString( "Failed Creating Modern Context\n" );
						//MessageBoxA(0 , (char*) ("Failed Creating Modern Conttext"), "OPENGL_VERSION", 0);

					}
				}
				else 
				{
					cout << "Create Context Attrib Failed" << endl;
					// MessageBoxA(0 , (char*) ("Failed Context Attrib"), "OPENGL_VERSION", 0);

				}
			}
			else
			{
				// Note(keenan): Antiquated version of opengl
				std::cout << "WGL Render Context Failed";

			}
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
		{
			HDC DeviceContext = GetDC(hwnd);

			glViewport(0,0, WindowWidth, WindowHeight);
			glClearColor(0.0f, .0f, .0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderOpenGLContext(Shader);
			//RenderOldOpenGL();
			SwapBuffers(DeviceContext); 
		}
		case WM_KEYDOWN:
		{
			
		}break;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam ,lParam);
}

bool Running = false;

float DeltaTime;


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

	LARGE_INTEGER lastCount; 
	LARGE_INTEGER PerformanceFrequencyResult; 
	int PerformanceFrequency;
	QueryPerformanceCounter(&lastCount); 
	QueryPerformanceFrequency(&PerformanceFrequencyResult);
	PerformanceFrequency = PerformanceFrequencyResult.QuadPart;
	
	//MSG msg = {}; 
	float lastMouseX   = 0, lastMouseY = 0;
	float ignoreDeltaX = 0, ignoreDeltaY = 0;
	SetCapture(WindowHandle);
	Running = true;
	while(Running ) 
	{
		MSG msg; 

			//reset input
		mouseYDelta = 0;
		mouseXDelta = 0;
		//GetMessage( &msg, NULL, 0, 0))  
		if(PeekMessage(&msg, 0,0, 0, PM_REMOVE )) { 

			if( msg.message == WM_QUIT)
			{
				Running = false;
			}

			if(msg.message == WM_MOUSEMOVE) {

				float xPos = (float)GET_X_LPARAM(msg.lParam); 
				float yPos = (float)GET_Y_LPARAM(msg.lParam); 

				float centerY = ((float)WindowHeight/2.0f); 
				float centerX = ((float)WindowWidth/2.0f); 

				//xPos -= ignoreDeltaX; 
				//yPos -= ignoreDeltaY; 
				mouseXDelta = (xPos -lastMouseX);//*10.0f; 
				mouseYDelta = (yPos -lastMouseY);//*10.0f; 

				lastMouseX = xPos;
				lastMouseY = yPos;


				ignoreDeltaX = ((float)WindowWidth/2.0f) - xPos; 
				ignoreDeltaY = ((float)WindowHeight/2.0f) - yPos;

				//SetCursorPos(centerX, centerY);
			}

			long wParam = (long) msg.wParam;

			if(wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}

			if(wParam == VK_RIGHT)
			{
				//modelPosition.x += .2f;
				//camera.position += GetRight() * .2f;
				yaw += .6f;

			}
			if(wParam == VK_LEFT)
			{
				// modelPosition.x -= .2f;
			    //camera.position -= GetRight() * .2f;
			    yaw -= .6f;

			}
			if(wParam == VK_DOWN) {
				// modelPosition.y  -= .2f;
				 pitch += .5f;
				//camera.position -= camera.forward * .2f;
			}
			if(wParam == VK_UP) {
				// modelPosition.y += .2f;
				 pitch -= .5f;
				// camera.position +=  camera.forward;//* .2f;
			}//
			TranslateMessage(&msg); 
			DispatchMessage( &msg); 
		}

		HDC DeviceContext = GetDC(WindowHandle);
		glViewport(0,0, WindowWidth, WindowHeight);
		glClearColor(0.0f, .0f, .0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		yaw   += mouseXDelta * DeltaTime * 10;
		pitch += mouseYDelta * DeltaTime * 10;

    ProcessPitch();
	ProcessYaw();

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