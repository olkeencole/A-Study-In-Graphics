/* ==========================================================
$File: Keenan Cole
$Revision
$Creator
$Notice: 
============================================================*/

/* A Study In Graphics
   API: Software Rendering
   Demo: Triangle Render using the standard barycentric rasterizing method

   - As simple as a script can be initializing the Windows API and using
   it to render a simple triangle from some vertices

      Line algorithm very buggy. Need to switch over to Bresenham's algorithm


*/ 
#include <Windows.h>
#include <iostream>
#include <stdint.h>
#include "Math.h"
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


Vector3 positionOffset; 

static HGLRC GlobalRenderContext; 
static int WindowWidth  = 800; 
static int WindowHeight = 600; 


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

int abs(int val){

	if(val < 0) return val * -1;

	 return val;
}
 
bool CheckRangeNDC(float x1)
{
	return x1 >= -1.0f && x1 <= 1.0; 
}

int NDCXToScreenPixel( float xNDC) {
    int width = bitmapWidth-1; 
	int xPixel = ( (( (xNDC + 1.0f)  )) ) *  ( width  / 2.0f); 

	return xPixel;
}

int NDCYToScreenPixel( float yNDC) {
	int height = bitmapHeight-1;
	int yPixel = ( (( 2.0f - (yNDC + 1.0f)    )) ) * (height / 2.0f); 

	return yPixel;
}


void DrawSquare(uint32 color, int xPos, int yPos, int width, int height)
{
  for (int y = yPos; y < height; ++y)
  {
  	for(int x = xPos; x < width; ++x)
  		DrawPixel(color,  x, y);
  }
}

Vector3 ConvertNDCToScreen( Vector3 ndcPoint ) {

	return Vector3( NDCXToScreenPixel( ndcPoint.x), NDCYToScreenPixel( ndcPoint.y), ndcPoint.z );
}

//v2 and v3 have the same Y

/* Slope of the line: y / x
   Inverse: x / y
   Changing slope of a line = v2.y - v1.y / v2.x - v1.x */ 

void Swap( Vector3 *v1, Vector3 *v2){

	Vector3 temp; 
	temp = *v1; 

	v1->x = v2->x; 
	v1->y = v2->y; 

	v2->x = temp.x; 
	v2->y = temp.y; 
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

/*
 I think this  is ultimately a cross product, since
 the magnitude of the cross product is the area of the parallogram
 . dividing by half gets a triangle area. 
	
	cross(b-a , c-a); 

*/

float perpDot(const Vector3 &a, const Vector3 &b){
	return (a.x * b.y) - (b.x * a.y);
}

int barycentric(const Vector3 &a, const Vector3 &b, const Vector3 &c){
	return (b.x - a.x) * (c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}

void DrawBaryTriangle(   const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, uint32 color){

	// Compute Bounding Box
	float minX = min3( v1.x, v2.x, v3.x);
	float maxX = max3( v1.x, v2.x, v3.x);
	float minY = min3( v1.y, v2.y, v3.y);
	float maxY = max3( v1.y, v2.y, v3.y);

	minX = max(minX, 0);
	maxX = min(maxX, WindowWidth-1);

	minY = max(minY, 0);
	maxY = min(maxY, WindowHeight-1);

	int x = 0, y = 0; 

	for (y = minY; y <= maxY; ++y)
	{
		for (x = minX; x <= maxX; ++x)
		{
			Vector3 currentPoint = Vector3(x, y, 1.0f);

			float c1 = perpDot(v2 - v3, currentPoint);
			float c2 = perpDot(v3 - v2, currentPoint);
			float c3 = perpDot(v3, currentPoint);

			float area = perpDot(v1, v2);

			float s = perpDot(currentPoint - v1, v2);
			float t = perpDot(v1, currentPoint - v2); 
			float sN = s / area;
			float sT = t / area;



			int w0 = barycentric(v2, v3, currentPoint);
			int w1 = barycentric(v3, v1, currentPoint);
			int w2 = barycentric(v1, v2, currentPoint);

			if(w0 >= 0 && w1 >= 0 && w2 >= 0){
				DrawPixel(color, x, y);
			}
		}
	}

}


void DrawBaryTriangle(   const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, Vector3 &color1, Vector3 &color2, Vector3 &color3){

	// Compute Bounding Box
	float minX = min3( v1.x, v2.x, v3.x);
	float maxX = max3( v1.x, v2.x, v3.x);
	float minY = min3( v1.y, v2.y, v3.y);
	float maxY = max3( v1.y, v2.y, v3.y);

	minX = max(minX, 0);
	maxX = min(maxX, WindowWidth-1);

	minY = max(minY, 0);
	maxY = min(maxY, WindowHeight-1);

	int x = 0, y = 0; 

	for (y = minY; y <= maxY; ++y)
	{
		for (x = minX; x <= maxX; ++x)
		{
			Vector3 currentPoint = Vector3(x, y, 1.0f);

			float c1 = perpDot(v1, currentPoint);
			float c2 = perpDot(v2, currentPoint);
			float c3 = perpDot(v3, currentPoint);

			float area = perpDot(v1, v2);

			float s = perpDot(currentPoint - v1, v2);
			float t = perpDot(v1, currentPoint - v2); 
			float sN = s / area;
			float sT = t / area;


			int w0 = barycentric(v2, v3, currentPoint);
			int w1 = barycentric(v3, v1, currentPoint);
			int w2 = barycentric(v1, v2, currentPoint);

			float Nw0 = ((float) w0 / area) / 2.0f;
			float Nw1 = ((float) w1 / area) / 2.0f; 
			float Nw2 = ((float) w2 / area) / 2.0f;

			if(w0 >= 0 && w1 >= 0 && w2 >= 0){
				uint32 finalColor = GetColor( Nw0*color1.x + Nw1*color2.x + Nw2*color3.x, 
											  Nw0*color1.y + Nw1*color2.y + Nw2*color3.y,
											  Nw0*color1.z + Nw1*color2.z + Nw2*color3.z );


				DrawPixel(finalColor, x, y);
			}
		}
	}

}
 
 void RenderUpdate(){

    int stride = 6; 
    int count = ArrayCount( vertices) / stride; 
    int realCount = ArrayCount(vertices);

  Vector3 triangle[3] ; 


  uint32 colors[3];
  Vector3 colors24[3];

    for (int i = 0; i < ArrayCount(vertices)-1; i+=stride)
    {

    	uint32 color = GetColor(vertices[i+3], vertices[i+4], vertices[i+5] ); 
    	triangle[i / stride] =  Vector3( vertices[i], vertices[i+1], vertices[i+2] );  
    	colors[ i / stride] = color;
    	// triangle[i] = v1;[]

    	colors24[i / stride].x = vertices[i+3]; 
    	colors24[i / stride].y = vertices[i+4]; 
    	colors24[i / stride].z = vertices[i+5]; 

    	//DrawLineNDC(color,  vertices[i], vertices[i+1],  vertices[i+stride], vertices[i+1+stride]   );
    	//DrawTriangle(color, Vector3(vertices[i], vertices[i+1] ), Vector3( vertices[i+stride], vertices[i+1+stride]) ); 
    }

    uint32 color = GetColor(vertices[realCount-3], vertices[realCount-2], vertices[realCount-1] ); 
 
    DrawBaryTriangle(ConvertNDCToScreen(triangle[2]),
                 ConvertNDCToScreen(triangle[1]), 
                 ConvertNDCToScreen(triangle[0]), 
    	         colors24[2], colors24[1], colors24[0] ) ;
 }



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

    RenderUpdate();

     
}


/* TODO
 -  Set up the double buffer
 -  Can we set individual pixels? 
 -  Convert pixels to NDC coordinates

*/
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
			if(wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}

			if(wParam == VK_RIGHT || wParam =='D')
			{
				positionOffset.x += .01f;

			}else
			if(wParam == VK_LEFT || wParam =='A')
			{
				positionOffset.x -= .01f;
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

	MSG msg = {}; 
	//PostMessage
	while( GetMessage( &msg, NULL, 0, 0))  
	{
		TranslateMessage(&msg); 
		DispatchMessage( &msg); 
	}


	return 0;
}