/* ==========================================================
$File: Keenan Cole
$Revision
$Creator
$Notice: 
============================================================*/

/* A Study In Graphics
   API: Software Rendering
   Demo: Simple Triangle With Color Interpolation

   - As simple as a script can be initializing the Windows API and using
   it to render a simple triangle from some vertices

   Interactive Computer Graphics - A Top-Down Approach, 2012, E. Angel & D.Shreiner
Introduction to Computer Graphics course, M. Pauly, EPFL 2015
How OpenGL works, D. V. Sokolov, available at github.com/ssloy/tinyrenderer
Triangle rasterization in practice and the following posts, F. Giesen, available here
Rasterization: a Practical Implementation, , available at scratchapixel.com
How to write a (software) 3d polygon pipeline, C. Bloom, available here

*/ 
#include <Windows.h>
#include <iostream>
#include <stdint.h>
#include "math.h"
using namespace std;

#define Assert(Expression) if(! ( Expression)) { *(int *)0 = 0; }

// UTILITY CONSTANTS
#define PI 3.14
#define DEG2RAD ((PI ) / 180.0f) 
#define ArrayCount(value) ( sizeof(value) / sizeof(value[0] ) ) //Length of an Array

#define internal        static // can only be used in its translation unit (file)
#define local_persist   static 
#define global_variable static 

typedef uint8_t uint8; 
typedef uint32_t uint32;

// Triangle Data with Vertex Color
//
float vertices[] = { 
	-1.0f,  -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.0f,   1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
};

float topFlatVertices[] = { 
	-1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.0f,   -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
};

float polyVertices[] = { 
	-.8f,  .6f,  0.0f,  1.0f, 0.0f, 0.0f,
	 .8f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 -0.6f,   1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
};

float verts[] = { 
	-1.0f,  -1.0f,  0,  1, 0, 0,
	 .95f,     -1,  0,  0, 1, 0,
	    0,    .9f,  0,  0, 0, 1,
}; 


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



// Vertice Data for for a Cube
// Vertex Infomation with Position and Color Stored in the Vertices

// Vertice Data for for a Cube
// Vertex Infomation with Position and Color Stored in the Vertices
float Cube_Color[] = {
    // Vert Position          // Color          
 -0.5, 0.5, 0.5 ,1,1,0,
0.5, 0.5, 0.5 ,0,1,0,
0.5, -0.5, 0.5 ,0,0,1,
-0.5, -0.5, 0.5 ,1,0,0,
-0.5, 0.5, 0.5 ,1,1,0,
0.5, -0.5, 0.5 ,0,0,1,
-0.5, 0.5, -0.5 ,1,1,0,
0.5, 0.5, -0.5 ,0,1,0,
0.5, 0.5, 0.5 ,0,0,1,
-0.5, 0.5, 0.5 ,1,0,0,
-0.5, 0.5, -0.5 ,1,1,0,
0.5, 0.5, 0.5 ,0,0,1,
-0.5, -0.5, -0.5 ,1,1,0,
0.5, -0.5, -0.5 ,0,1,0,
0.5, 0.5, -0.5 ,0,0,1,
-0.5, 0.5, -0.5 ,1,0,0,
-0.5, -0.5, -0.5 ,1,1,0,
0.5, 0.5, -0.5 ,0,0,1,
-0.5, -0.5, 0.5 ,1,1,0,
0.5, -0.5, 0.5 ,0,1,0,
0.5, -0.5, -0.5 ,0,0,1,
-0.5, -0.5, -0.5 ,1,0,0,
-0.5, -0.5, 0.5 ,1,1,0,
0.5, -0.5, -0.5 ,0,0,1,
-0.5, 0.5, -0.5 ,1,1,0,
-0.5, 0.5, 0.5 ,0,1,0,
-0.5, -0.5, 0.5 ,0,0,1,
-0.5, -0.5, -0.5 ,1,0,0,
-0.5, 0.5, -0.5 ,1,1,0,
-0.5, -0.5, 0.5 ,0,0,1,
0.5, 0.5, 0.5 ,1,1,0,
0.5, 0.5, -0.5 ,0,1,0,
0.5, -0.5, -0.5 ,0,0,1,
0.5, -0.5, 0.5 ,1,0,0,
0.5, 0.5, 0.5 ,1,1,0,
0.5, -0.5, -0.5 ,0,0,1,
};


Vector3 positionOffset; 
float *zBuffer;
int processCount = 1;

int pitch = 6;

 //int vCount = (   ArrayCount(Cube_Color) ) /  pitch ;
//Vertex *vertices; //[  vCount];

/// GLOBALS
//
static HGLRC GlobalRenderContext; 
static int WindowWidth  = 800; 
static int WindowHeight = 600; 

struct Camera {
	Vector3 position = Vector3(0,  0,   5.0f); 
	Vector3 forward  = Vector3(0,  0, -1.0f); // what we are looking at// direction facing // or could think of it as rotation I suppose. 

	float fieldOfView =  60.0f; 
	float nearClip = .1f; 
	float farClip  = 4.0f; 

	Matrix4 view; 
	Matrix4 proj; 
};

struct Vertex
{
	Vector3 position; 
	Vector3 normal; 
	Vector3 color;
	Vector2 uv; 
};
 	
Camera camera; 
float Ds = 0;
float tanHalfFovy; 

global_variable BITMAPINFO bitmapInfo; 
global_variable void *BitmapMemory; 
global_variable HBITMAP bitmapHandle; 
global_variable HDC BitmapDeviceContext;
global_variable int bitmapWidth; 
global_variable int bitmapHeight; 
global_variable int BytesPersPixel = 4; 


void DrawPixel(uint32 color, int x, int y)
{
	//int pitch = width * BytesPersPixel; 
	uint32 *base = (uint32*) BitmapMemory; 
	uint32 *pixel = (uint32 *)  base + (uint32)(x + y * (bitmapWidth)  ) ;
	*pixel = color;
}

uint32 GetColor( int r, int g, int b)
{
	uint32 color; 
	color =  ((uint32) b); 
	color += ((uint32) g) << 8; 
	color += ((uint32) r) << 8 * 2;
	//color = a << 8 *3;
	return color;
}

uint32 GetColor( float r, float g, float b) // Normalized
{
	return GetColor( (int)( r * 255),(int)( g * 255), (int)(b*255)) ;
}


void DrawSquare(uint32 color, int xPos, int yPos, int width, int height)
{
  for (int y = yPos; y < height; ++y)
  {
  	for(int x = xPos; x < width; ++x)
  		DrawPixel(color,  x, y);
  }
}

bool CheckRangeNDC(float x1)
{
	return x1 >= -1.0f && x1 <= 1.0; 
}

int NDCXToScreenPixel( float xNDC) {
    int width = bitmapWidth-1; 
    xNDC = Clamp(xNDC, -1, 1);
	int xPixel = ( (( (xNDC + 1.0f)  )) ) *  ( width  / 2.0f); 

	return xPixel;
}

int NDCYToScreenPixel( float yNDC) {
	int height = bitmapHeight-1;
	yNDC = Clamp(yNDC, -1, 1);
	int yPixel = ( (( 2.0f - (yNDC + 1.0f)    )) ) * (height / 2.0f); 

	return yPixel;
}




inline Vector3 ConvertNDCToScreen( Vector3 ndcPoint ) {

	float zScreen = ( (Ds / 2) * ndcPoint.z) + (Ds / 2);
	return Vector3( NDCXToScreenPixel( ndcPoint.x), NDCYToScreenPixel( ndcPoint.y), zScreen );
}


float Max( float a, float b){
	if(a > b)
		return a; 
	else
		return b;
}

float Min(float a, float b){
	if( a > b)
		return b; 
	else 
		return a;
}
float min3( float f1, float f2, float f3){
	float result = Min(f1, f2);
	result = Min(result, f3);
	return result;
}

float max3(float f1, float f2, float f3){
	float result = Max(f1, f2); 
	result = Max(result, f3);
	return result;
}



float ParallelArea( Vector3 v1, Vector3 v2, Vector3 v3)
{
		Vector3 BA = v2 - v1; 
		Vector3 CA = v3 - v1; 


		return (BA.x * CA.y) - (BA.y * CA.x);
}




void DrawBaryTriangle(   Vector3 &v1,  Vector3 &v2,  Vector3 &v3, Vector3 &color1, Vector3 &color2, Vector3 &color3){


	// //Swap order if necesssary 
 //    if(v1.x > v2.x) Swap(&v1, &v2);
	// if(v1.x > v3.x) Swap(&v1, &v3);

	// if(v2.y > v3.y ) Swap(&v2, &v3);

	// Compute Bounding Box
	float minX = min3( v1.x, v2.x, v3.x);
	float maxX = max3( v1.x, v2.x, v3.x);
	float minY = min3( v1.y, v2.y, v3.y);
	float maxY = max3( v1.y, v2.y, v3.y);

	minX = max(minX, 0);
	maxX = min(maxX, bitmapWidth-1);

	minY = max(minY, 0);
	maxY = min(maxY, bitmapHeight-1);

	int x = 0, y = 0; 

	for (y = minY; y <= maxY; ++y)
	{
		for (x = minX; x <= maxX; ++x)
		{
			Vector3 currentPoint = Vector3(x, y, 1.0f);

			float area =  ParallelArea(v1, v2, v3); ////perpDot(v1, v2);

			float w0 = ParallelArea( v2, v3, currentPoint);;// barycentric(v2, v3, currentPoint);
			float w1 = ParallelArea( v3, v1, currentPoint);;// barycentric(v3, v1, currentPoint);
			float w2 = ParallelArea( v1, v2, currentPoint);;// barycentric(v1, v2, currentPoint);

			float Nw0 = ((float) w0 / area);
			float Nw1 = ((float) w1 / area); 
			float Nw2 = ((float) w2 / area);

			float interpolatedZ = Nw0 * v1.z + Nw1 * v2.z + Nw2 * v3.z; 

			bool depthTest = zBuffer[ x+ y * (bitmapWidth) ] <= interpolatedZ;

			if(w0 >= 0 && w1 >= 0 && w2 >= 0  ){

					if(depthTest) {
					uint32 finalColor = GetColor( Nw0*color1.x + Nw1*color2.x + Nw2*color3.x, 
												  Nw0*color1.y + Nw1*color2.y + Nw2*color3.y,
												  Nw0*color1.z + Nw1*color2.z + Nw2*color3.z );


					DrawPixel(finalColor, x, y);
					zBuffer[ x+ y*bitmapWidth ] = interpolatedZ;
				}
			}
		}
	}

}
 
//Now need to transform model - view - perspective
// we are recalculating for every vertice BAD!!!
inline Vector3 MVP_Transform( Vector3 v, Matrix4 &mvp){

	Vector3 result = Vector3(0,0,0); 
	Vector4 interim = Vector4(v.x, v.y, v.z, 1.0f);
	Vector4 interimResult = Vector4();
	//float tanHalfFovy = tan( camera.fieldOfView / 2.0f); 
	//Ds = 1.0f / tanHalfFovy;
	
	interimResult  = mvp * interim;	
	float divide   = interimResult.w; 

	result = Vector3( interimResult.x / divide,  interimResult.y / divide,  interimResult.z / divide );

	//Now also need NDC matrix for formulas or clipping space
	//Also, do I want to do clipping? 
	return result;

}

internal void
WindowUpdate(HDC hdc, RECT *windowRect, int x, int y, int width, int height) {

	int windowWidth  = windowRect->right  - windowRect->left;
	int windowHeight = windowRect->bottom - windowRect->top; 
	StretchDIBits( hdc, /*x, y, width, height,
					    x, y, width, height, */
						0, 0, bitmapWidth, bitmapHeight, 
						0, 0, windowWidth, windowHeight,
					    BitmapMemory, 
					    &bitmapInfo, 
						DIB_RGB_COLORS, SRCCOPY);  
}

void UpdateWindow(HDC hdc){
	StretchDIBits( hdc, /*x, y, width, height,
					    x, y, width, height, */
						0, 0, bitmapWidth, bitmapHeight, 
						0, 0, bitmapWidth, bitmapHeight,
					    BitmapMemory, 
					    &bitmapInfo, 
						DIB_RGB_COLORS, SRCCOPY); 
}

void DrawDepthBuffer(){
		for (int y = 0; y < bitmapHeight; y++)
    {
    	for(int x = 0; x < bitmapWidth; x++){
    	//uint32 *base = (uint32*) BitmapMemory; 
    	
    	float p = zBuffer[ x + y*bitmapWidth]; 

    	float n = camera.nearClip;
    	float f = camera.farClip;
    	p = (2 * n) / (f + n - p * (f - n));
    	p = 1 - p;
    	p *= 2.0f;

    	if(p < 1.9){

    		//something
    		cout <<" Hello";
    	}

    	//Clamp(p, 0, 1.0f);
		DrawPixel(GetColor( p, p, p) ,x, y);   
		}	
    }

}

 void RenderUpdate(Vertex vertices[] , int vCount){

    int stride = 6; 


   // int vCount = ArrayCount( vertices) ; //

	  Vector3 triangle[3] ; 
	  Vector3 flatTop[3]; 
	  Vector3 skewed[3]; 

	  uint32 colors[3];
	  Vector3 colors24[3];
  
      Matrix4 mod  = Matrix4();
	
	//camera.position.z = positionOffset.x;
	//
		for(int x=0; x < ArrayCount(mPositions); x++ ) {

	     mod.Translate(mPositions[x]);
	     mod  = mod.RotateZAxis( positionOffset.x  );
		 mod  = mod.RotateYAxis(positionOffset.y   );
		 mod  = mod.RotateXAxis(positionOffset.x   );

		 Matrix4 mvp =  camera.proj * camera.view * mod; //Double check matrix mult

				//Process Vertex List
				for (int i = 0; i < vCount	; i+=3)
				{

					Vector3 c1 = MVP_Transform(  vertices[i+0].position, mvp);
					Vector3 c2 = MVP_Transform(  vertices[i+1].position, mvp);
					Vector3 c3 = MVP_Transform(  vertices[i+2].position, mvp);

					Vector3 ndc1 = ConvertNDCToScreen( c1);  
					Vector3 ndc2 = ConvertNDCToScreen( c2); 
					Vector3 ndc3 = ConvertNDCToScreen( c3); 

					DrawBaryTriangle((   ndc1 ),
				                     (   ndc2 ), 
				                     (   ndc3 ), 
				    	                 vertices[i+0].color, vertices[i+1].color,  vertices[i+2].color) ; 


					// DrawBaryTriangle((   ndc3 ),
				 //                     (   ndc2 ), 
				 //                     (   ndc1 ), 
				 //    	                 vertices[i+3].color, vertices[i+2].color,  vertices[i+1].color) ; 
				}
	}


   
 }



 /*Graphics Pipeline
	- Verts are in local space (currently we are lucky in that the verts corespond to NDCs)
	- Transform to World
	- Transform to Camera
	- Apply Perspective 
	-

 */

//Device Inde Bitmap
internal void  ResizeDIBSection(int width, int height){

	if(BitmapMemory){
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	bitmapWidth  = width; 
	bitmapHeight = height;

	bitmapInfo.bmiHeader.biSize  		 = sizeof(bitmapInfo.bmiHeader) ; 
    bitmapInfo.bmiHeader.biWidth 		 = width;
    bitmapInfo.bmiHeader.biHeight        = -height;
    bitmapInfo.bmiHeader.biPlanes        = 1; 
    bitmapInfo.bmiHeader.biBitCount      = 32;
    bitmapInfo.bmiHeader.biCompression   = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage     = 0; 
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0; 
    bitmapInfo.bmiHeader.biClrUsed 	     = 0 ;
    bitmapInfo.bmiHeader.biClrImportant  = 0;

  //  int BytesPersPixel = 4; 
    int BitmapMemorySize = (width * height) * BytesPersPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE ) ;

    uint8 *row = (uint8 *) BitmapMemory; 
    int pitch  = width * BytesPersPixel; 

    zBuffer = new float[width * height];
    

}

// Windows Message Callback
// 
//
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg)
	{
		case WM_CREATE: 
		{
			
		}
		break;
		case WM_SIZE: 
		{
			RECT rect; 
			GetClientRect(hwnd, &rect); 
			int width  = rect.right  - rect.left; 
			int height = rect.bottom - rect.top;

			ResizeDIBSection(width, height); 
		}break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT paint;

			HDC DeviceContext = BeginPaint(hwnd, &paint); 
			int x = paint.rcPaint.left; 
			int y = paint.rcPaint.top; 
			int width = paint.rcPaint.right - x; 
			int height = paint.rcPaint.bottom - y; 

			RECT clientRect ;
			GetClientRect(hwnd, &clientRect);
			//PatBlt(DeviceContext, x, y, width, height, WHITENESS); 
			//RenderUpdate();
			WindowUpdate(DeviceContext, &clientRect,  x, y , width, height);


			EndPaint(hwnd, &paint); 
			//RenderOldOpenGL();
			// SwapBuffers(DeviceContext); 
		}
		case WM_KEYDOWN:
		{
			float step = 4.0f;
			if(wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}

			if(wParam == VK_RIGHT || wParam =='D')
			{
				positionOffset.x += step;

			}else
			if(wParam == VK_LEFT || wParam =='A')
			{
				positionOffset.x -= step;
			}

			if(wParam == VK_UP   || wParam =='W')
			{
				positionOffset.y += step;
					

			}else
			if(wParam == VK_DOWN || wParam =='S')
			{
				processCount -=1;
			     positionOffset.y -= step;


			}
		}break;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam ,lParam);
}



int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR     CommandLine,
    int       WindowShow
)
{

	//Create and Register WindowClass	
	WNDCLASS wc = {}; 
	wc.lpfnWndProc = WindowProc; 
	wc.hInstance   = Instance; 
	wc.lpszClassName   = "Windows Test";

	bool isRunning = true;
	
	
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


	// process Matrices and Vertex Buffer
    tanHalfFovy = tan( camera.fieldOfView / 2.0f); 
	Ds = 1.0f / tanHalfFovy;

	camera.view = LookAt( camera.position, camera.position + camera.forward, Vector3(0,1,0)  );
	camera.proj = Perspective(  camera.fieldOfView * DEG2RAD, (float)WindowWidth / (float) WindowHeight, camera.nearClip, camera.farClip); 

   const int pitch = 6;

   const int vCount =   ArrayCount(Cube_Color)  /  pitch ;
   Vertex vertices[vCount]; 

	for (int i = 0; i < ArrayCount(Cube_Color) ; i+=6)
	{
		int index  = i;

		if( index > 0)
			index = index / 6;

		int index1 = i+1; 
		int index2 = i+2; 

		int color1 = i+3;
		int color2 = i+4;
		int color3 = i+5;

		vertices[index].position.x = Cube_Color[ i ];
		vertices[index].position.y = Cube_Color[ index1 ];
		vertices[index].position.z = Cube_Color[ index2 ];

		vertices[index].color = Vector3 (Cube_Color[ color1 ],  Cube_Color[ color2 ], Cube_Color[ color3 ]);
	}

	// //Correct the winding order
	// for (int i = 0; i < vCount; i+=3)
	// {
	// 	Vertex v1 = vertices[i]; 
	// 	Vertex v2 = vertices[i+1]; 
	// 	Vertex v3 = vertices[i+2];

	//     if(v1.position.x > v2.position.x)  SwapVertices(&v1, &v2);
	// 	if(v1.position.x > v3.position.x)  SwapVertices(&v1, &v3);
	// 	if(v2.position.x > v3.position.x ) SwapVertices(&v2, &v3);

	// 	if(v2.position.y > v3.position.y ) SwapVertices(&v2, &v3);

	// 	vertices[i]   = v1; 
	// 	vertices[i+1] = v2; 
	// 	vertices[i+2] = v3;

	// }

	while(isRunning) {
	
		MSG msg ; 

		if(PeekMessage(&msg, 0, 0 , 0, PM_REMOVE)) {

			if(msg.message == WM_QUIT){
				isRunning = false;
			}

			TranslateMessage(&msg); 
			DispatchMessage( &msg);
		}

    for (int i = 0; i < (bitmapWidth) * (bitmapHeight ) ; i++)
    {
    	zBuffer[i] = -111111.0f;
    }
		DrawSquare(GetColor(0,0,0), 0, 0, bitmapWidth-1, bitmapHeight-1);
		RenderUpdate( vertices, vCount);
		UpdateWindow( GetDC(WindowHandle));
	}

	return 0;
}