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

*/ 
#include <Windows.h>
#include <GL/GL.h>
#include <iostream>
#include <stdint.h>
#include "OBJLoader.h"

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


// Shaders
// Just displays pos in NDC and colors
const char* vertexShader = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 verts;\n" 
	"layout (location = 1) in vec3 vColor;\n"
	"out vec3 color;\n" 

	"void main() { \n"
		"gl_Position = vec4(verts.x, verts.y, verts.z, 1.0);\n"
		"color = vColor;\n"
	"}\n\0" ;

const char* fragmentShader = 
	"in vec3 color;\n"
	"void main(){\n"
		"gl_FragColor = vec4(color, 1.0);\n" 
	"}\n\0";



//WGL CONSTANTS FLAGS
//
#define WGL_CONTEXT_MAJOR_VERSION_ARB           	0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           	0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             	0x2093
#define WGL_CONTEXT_FLAGS_ARB                   	0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            	0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB               	0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB     0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB           0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB  0x00000002
#define ERROR_INVALID_VERSION_ARB                  0x2095
#define ERROR_INVALID_PROFILE_ARB                  0x2096



//OPENGL CONSTANT FLAGS
	//
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82


 
// OPENGL FUNCTION POINTER TYPE DEFINITIONS
//	
typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

typedef HGLRC WINAPI wgl_create_attrib_arb(HDC hdc, HGLRC sharedContext, const int *attribs );

typedef void WINAPI gl_GenVertexArrays (GLsizei n, GLuint *arrays);
typedef void WINAPI gl_GenBuffers (GLsizei n, GLuint *buffers);
typedef void WINAPI gl_BindVertexArray (GLuint array);
typedef void WINAPI gl_BindBuffer (GLenum target, GLuint buffer);
typedef void WINAPI gl_BufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void WINAPI gl_VertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void WINAPI gl_EnableVertexAttribArray (GLuint index);

typedef void   WINAPI gl_CompileShader (GLuint shader);
typedef GLuint WINAPI gl_CreateProgram (void);
typedef GLuint WINAPI gl_CreateShader  (GLenum type);
typedef void   WINAPI gl_DeleteShader  (GLuint shader);
typedef void   WINAPI gl_ShaderSource  (GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void   WINAPI gl_AttachShader  (GLuint program, GLuint shader);
typedef void   WINAPI gl_LinkProgram   (GLuint program);
typedef void   WINAPI gl_UseProgram    (GLuint program);

typedef void  WINAPI gl_GetProgramiv      (GLuint program, GLenum pname, GLint *params);
typedef void  WINAPI gl_GetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void  WINAPI gl_GetShaderiv       (GLuint shader, GLenum pname, GLint *params);
typedef void  WINAPI gl_GetShaderInfoLog  (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);



// FUNCTION POINTER DECLARATIONS
//
gl_GenVertexArrays  		*glGenVertexArrays;
gl_BindVertexArray  		*glBindVertexArray; 
gl_GenBuffers       		*glGenBuffers; 
gl_BindBuffer       		*glBindBuffer; 
gl_BufferData       		*glBufferData; 
gl_VertexAttribPointer		*glVertexAttribPointer; 
gl_EnableVertexAttribArray  *glEnableVertexAttribArray;

gl_AttachShader   *glAttachShader ;  
gl_CompileShader  *glCompileShader;  
gl_CreateProgram  *glCreateProgram;  
gl_CreateShader   *glCreateShader ;  
gl_DeleteShader   *glDeleteShader ;  
gl_ShaderSource   *glShaderSource ;  
gl_LinkProgram    *glLinkProgram  ;  
gl_UseProgram     *glUseProgram   ;  

gl_GetProgramiv       *glGetProgramiv      ;
gl_GetProgramInfoLog  *glGetProgramInfoLog ;
gl_GetShaderiv        *glGetShaderiv       ;
gl_GetShaderInfoLog   *glGetShaderInfoLog  ;



/// GLOBALS
//
static HGLRC GlobalRenderContext; 
static int WindowWidth  = 800; 
static int WindowHeight = 600; 
static GLuint VAO,VBO, Shader; 


// Store Vertex Data In Buffer In GPU
//
void InitOpenGL()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts,  GL_STATIC_DRAW ); //6

	int index = 6; 
	glVertexAttribPointer(0 , 3,  GL_FLOAT, GL_FALSE, sizeof(float) * index ,   (void *) 0); //6 
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1 , 3,  GL_FLOAT, GL_FALSE, sizeof(float) * index ,   (void *) ( 3 * sizeof(float) ) ); //6 
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

 } 



// Compile Shader and Store in GPU Buffer
//
 void InitShader()
 {
 	//Debuging
 	char log[512]; 
 	int success;
 	GLuint vertex, frag, shader; 

 	vertex = glCreateShader(GL_VERTEX_SHADER); 
 	glShaderSource(vertex, 1, &vertexShader, NULL); //4 id, count, shader text, null < length
 	glCompileShader( vertex);// just takes the idea
	glGetShaderiv( vertex, GL_COMPILE_STATUS, &success);
	 
 	//if(!success)
 	{
 		glGetShaderInfoLog(vertex, 512, NULL, log); // //shader, log limit, dnull, charlog)
 		std::cout << "SHADER ERROR: " << log << std::endl;
 	}

 	frag = glCreateShader(GL_FRAGMENT_SHADER);
 	glShaderSource( frag, 1, &fragmentShader, NULL);
 	glCompileShader(frag); 

 	shader = glCreateProgram();
 	glAttachShader(shader, vertex);
 	glAttachShader(shader, frag);
 	glLinkProgram(shader); 
	glGetProgramiv(shader, GL_LINK_STATUS, &success );
 	
 	//if( !success)
 	{
 		glGetProgramInfoLog( shader, 512, NULL, log );
 		std::cout << "LINKING ERROR: " << log << std::endl;
 	}

 	glDeleteShader(vertex); 
 	glDeleteShader(frag);  

 	Shader = shader;
 }


// Draw with OpenGLCore
//
 void RenderOpenGLContext(GLuint shader)
 {
 	glUseProgram(shader); 

 	glBindVertexArray(VAO);
 	glDrawArrays( GL_TRIANGLES, 0 , 3 );

 	glBindVertexArray(0);
 }



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

void DrawLineColor(uint32 colorA, uint32 colorB,  int x1, int y1, int x2, int y2 )
{
	
	for (float i = 0; i <= 1; i+=.001f)
	{
		int x = x1* (1.0f - i) + x2 * i;
		int y = y1* (1.0f - i) + y2 * i;

		uint32 color = (uint32) Lerp( (uint32) colorA, (uint32) colorB, i );
		DrawPixel( color, x, y);
	}
}

void DrawLine(uint32 color, int x1, int y1, int x2, int y2 )
{
	
	for (float i = 0; i <= 1; i+=.001f)
	{
		int x = x1* (1.0f - i) + x2 * i;
		int y = y1* (1.0f - i) + y2 * i;
		DrawPixel( color, x, y);
	}
	return; 

	int dX, dY, absDx, absDy, x = 0, y = 0, adyMinusBdx;

	dX = x2 - x1; 
	dY = y2 - y1; 

	absDx = dX < 0 ? dX*-1 : dX; 
	absDy = dY < 0 ? dY*-1 : dY;

	DrawPixel(color, x1, y1);


	while( abs(x) < absDx && abs(y) < absDy){

		//dont' really  understand the adyMinusBdx part. I know its likes an error tracker
		//I guess it allows us to zig zag back and forth
		if(adyMinusBdx < 0){

			if(dX > 0){
				x++;
			}
			else
			{
				x--;
			}

			adyMinusBdx+=absDx;
		}		
		else
		{
			if(dY > 0){
				y++;
			}
			else
				y--;

			adyMinusBdx -= absDx;
		}

		DrawPixel(color, x1+x, y1+y);
	}
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


void DrawLineNDC(uint32 color, float x1, float y1, float x2, float y2){

	bool b = CheckRangeNDC(x1) && CheckRangeNDC(y1) && CheckRangeNDC(x2) && CheckRangeNDC(y2); 
	if(!b)  return;

	int width = bitmapWidth-1; 
	int height = bitmapHeight-1;

	 int xPos1 = ( (( (x1 + 1.0f)  )) ) *  ( width  / 2.0f); 
	 int yPos1 = ( (( 2.0f - (y1 + 1.0f)    )) ) * (height / 2.0f); 
	 
	 int xPos2 = ( (( (x2 + 1.0f)  )) ) * (width  / 2.0f );  
  	 int yPos2 = ( (( 2.0f - (y2 + 1.0f)  )) ) * (height / 2.0f); 

	 DrawLine(color, xPos1, yPos1, xPos2, yPos2);  
}

void DrawLineNDC( uint32 color, Vector3 v1, Vector3 v2) {

	DrawLineNDC( color, v1.x, v1.y, v2.x, v2.y);
}

void DrawSquare(uint32 color, int xPos, int yPos, int width, int height)
{
  for (int y = yPos; y < height; ++y)
  {
  	for(int x = xPos; x < width; ++x)
  		DrawPixel(color,  x, y);
  }
}

void DrawCircle(int xCenter, int yCenter, int radius) {

}


Vector3 ConvertNDCToScreen( Vector3 ndcPoint ) {

	return Vector3( NDCXToScreenPixel( ndcPoint.x), NDCYToScreenPixel( ndcPoint.y), ndcPoint.z );
}

//v2 and v3 have the same Y

/* Slope of the line: y / x
   Inverse: x / y
   Changing slope of a line = v2.y - v1.y / v2.x - v1.x */ 

void SortVerticesByAscendingOrder( ) {


}

void FillFlatBottomTriangle( Vector3 v1, Vector3 v2, Vector3 v3) {

	// float step =  v3.y - v1.y; 
	//if(v1.y > v2.y) return; 

	float invSlope1 = (v2.x - v1.x) / (v2.y - v1.y) ;
	float invSlope2 = (v3.x - v1.x) / (v3.y - v1.y) ;

	float curX1 = v1.x; 
	float curX2 = v1.x; 

	for (int scanLine = v1.y; scanLine <= v2.y; scanLine++) //scanline is walking down the y from top to bottom
	{
		DrawLine( GetColor(.3f,.5f, .7f) , curX1, scanLine,   curX2, scanLine );
		curX1 += invSlope1; 
		curX2 += invSlope2;   
	}
}

//Assume that v1 and v2 have the same y value
void FillFlatTopTriangle( Vector3 v1, Vector3 v2, Vector3 v3) {

	float invSlope1 = (v3.x - v1.x) / (v3.y - v1.y); 
	float invSlope2 = (v3.x - v2.x) / (v3.y - v2.y); 

	float curX1 = v3.x; 
	float curX2 = v3.x; 

	for (int scanLine = v3.y; scanLine >= v2.y; scanLine--) //Going up the screen
	{
		DrawLine( GetColor(.3f, .4f, .2f ), curX1, scanLine, curX2, scanLine);
		curX1 -= invSlope1; 
		curX2 -= invSlope2; 
	}
}


//v1 and v2 have the same Y
void TopTriangle( Vector3 v1, Vector3 v2, Vector3 v3) {
	
}

/* filling the triangle would be a lot easier if the points were screen pixels 
   instead of the NDC coordinate system
   */

void DrawWireTriangle( Vector3 v1, Vector3 v2, Vector3 v3) {

	/* 1 - Sort the vertices so that the are in descending order  (bare in mind that how Windows and OpenGL treat the y is reversed)
		2- Find if we have a top or bottom triangle 
		3. If not, theen find a split in the triangle by adding a fourth vert that bisects the triangle with v2.y 
		4. Then rasterize the top and bottom triangles
	
	*/

	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), v1, v2 );
	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), v2, v3 );
	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), v3, v1 );

}

void Swap( Vector3 *v1, Vector3 *v2){

	Vector3 temp; 
	temp = *v1; 

	v1->x = v2->x; 
	v1->y = v2->y; 

	v2->x = temp.x; 
	v2->y = temp.y; 
}

void DrawTriangle(Vector3 v1, Vector3 v2, Vector3 v3){

	//Assume its a general triangle

	//bubble sort
	if(v1.y > v2.y) Swap(&v1, &v2);
	if(v1.y > v3.y) Swap(&v1, &v3);
	if(v2.y > v3.y) Swap(&v2, &v3);

	Vector3 v4 = Vector3();
	v4.x = v1.x  + (v2.y - v1.y) / ( (v3.y - v1.y) ) * (v3.x - v1.x ); 
	v4.y = v2.y; 
	//v4.x = v3.x; 

	FillFlatBottomTriangle(v1, v2, v4); 

	FillFlatTopTriangle(v2, v4, v3); 
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

   // DrawPixel(11111111, width-1, height-1);
    // DrawSquare( GetColor(1.0f, 0.0f, .0f), 0, 0, bitmapWidth, bitmapHeight);
    // DrawSquare( GetColor(.0f, .0f, 1.0f), 100, 100, 300, 300); 

     // DrawLine( GetColor(1.0f, 1.0f, 1.0f), 0, 0, bitmapWidth, bitmapHeight);
    // DrawLine( GetColor(0.0f, 1.0f, 0.0f),  10, 200, 100, 400);
    //DrawLine( GetColor(0.0f, 0.0f, 1.0f), 13, 20, 80, 40);
   //  DrawLine( GetColor(1.0f, 1.0f, 0.0f),  90, 40, 23, 20);


   // DrawLine( GetColor(1.0f, 1.0f, 1.0f),  bitmapWidth-1, bitmapHeight-1, 0, 0);
   // DrawLineNDC( GetColor(.0f, 0.0f, 1.0f), -1.0f, -1.0f, 1.0f, 1.0f);

   //    DrawLineNDC( GetColor(1.0f, .0f, 1.0f), -1.0f, 1.0f, 1.0, -.6);

   //   DrawLineNDC( GetColor(1.0f, .0f, 1.0f), 1.0f, 1.0f, -1.0f, -1.0f);

   //   DrawLineNDC( GetColor(1.0f, 1.0f, 0.0f), 1.0f, 1.0f, 0.0f, .0f);

   //   DrawLineNDC( GetColor(.0f, 1.0f, 0.0f),  .0f,   .0f, 1.0f, 1.0f); //errors out    
   //  DrawLineNDC( GetColor(.0f, 1.0f, 0.0f), 1.0f, -1.0f,  .0f, .0f); //errors out

    Mesh model; 
    model = LoadMeshOBJ("models/", "african_head.obj");


    int stride = 6; 
    int count = ArrayCount( vertices) / stride; 
    int realCount = ArrayCount(vertices);

  Vector3 triangle[3] ; 
  Vector3 flatTop[3]; 
  Vector3 skewed[3]; 

  uint32 colors[3];
 

    for (int i = 0; i < ArrayCount(vertices)-1; i+=stride)
    {

    	uint32 color = GetColor(vertices[i+3], vertices[i+4], vertices[i+5] ); 
    	triangle[i / stride] =  Vector3( vertices[i], vertices[i+1], vertices[i+2] );  
    	colors[ i / stride] = color;
    	// triangle[i] = v1;
    	//DrawLineNDC(color,  vertices[i], vertices[i+1],  vertices[i+stride], vertices[i+1+stride]   );
    	//DrawTriangle(color, Vector3(vertices[i], vertices[i+1] ), Vector3( vertices[i+stride], vertices[i+1+stride]) ); 
    }

    // Flat top triangle 
    for (int i = 0; i < ArrayCount(topFlatVertices)-1; i+=stride)
    {

    	uint32 color = GetColor(topFlatVertices[i+3], topFlatVertices[i+4], topFlatVertices[i+5] ); 
    	flatTop[i / stride] =  Vector3( topFlatVertices[i], topFlatVertices[i+1], topFlatVertices[i+2] );  
    	colors[ i / stride] = color;
    	// triangle[i] = v1;
    	//DrawLineNDC(color,  vertices[i], vertices[i+1],  vertices[i+stride], vertices[i+1+stride]   );
    	//DrawTriangle(color, Vector3(vertices[i], vertices[i+1] ), Vector3( vertices[i+stride], vertices[i+1+stride]) ); 
    }

    // Flat top triangle 
    for (int i = 0; i < ArrayCount(polyVertices)-1; i+=stride)
    {

    	uint32 color = GetColor(polyVertices[i+3], polyVertices[i+4], polyVertices[i+5] ); 
    	skewed[i / stride] =  Vector3( polyVertices[i], polyVertices[i+1], polyVertices[i+2] );  
    	colors[ i / stride] = color;
    	// triangle[i] = v1;
    	//DrawLineNDC(color,  vertices[i], vertices[i+1],  vertices[i+stride], vertices[i+1+stride]   );
    	//DrawTriangle(color, Vector3(vertices[i], vertices[i+1] ), Vector3( vertices[i+stride], vertices[i+1+stride]) ); 
    }

    uint32 color = GetColor(vertices[realCount-3], vertices[realCount-2], vertices[realCount-1] ); 
   // DrawLineNDC(color,  vertices[realCount-6], vertices[realCount-5],  vertices[0], vertices[1]   );

	//DrawTriangle( triangle[2], triangle[0],  triangle[1]);
	// FillFlatBottomTriangle( ConvertNDCToScreen( triangle[2] ), ConvertNDCToScreen( triangle[0]),  ConvertNDCToScreen( triangle[1] )) ;
	// FillFlatTopTriangle( ConvertNDCToScreen( flatTop[0] ), ConvertNDCToScreen( flatTop[1]),  ConvertNDCToScreen( flatTop[2] )) ;
   

    //Draw last Vert to First Vert


    int faceCount = model.indices.size() / 3;

    for (int i = 0; i < model.vertices.size(); ++i)
    {
    	
    	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[i].position,   model.vertices[i+1].position );
    }

	for (int i = 0; i < faceCount;  i+= 3)
   {
   	int index0 = i;//  model.indices[i]; 
   	int index1 = i+1; //model.indices[i+1];
   	//int index2 = model.indices[i+2];  

   	int index2 =  i+2; //model.indices[i + 2];
   	int index3 = model.indices[i  + 3];
   	//int index4 = model.indices[i  + 5];

   	DrawTriangle( ConvertNDCToScreen( model.vertices[index0].position), 
				  ConvertNDCToScreen( model.vertices[index1].position), 
				  ConvertNDCToScreen( model.vertices[index2].position) );
    	
	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index0].position,   model.vertices[index1].position );
	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index1].position,   model.vertices[index2].position );
	DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index2].position,   model.vertices[index0].position );	

    }

          // DrawWireTriangle((skewed[2]), (skewed[0]), (skewed[1])) ; 


      DrawTriangle(ConvertNDCToScreen(skewed[2]), ConvertNDCToScreen(skewed[0]), ConvertNDCToScreen(skewed[1])) ; 



    // for (int y = 0; y < bitmapHeight; ++y)
    // {
    // 	uint8 *pixel = (uint8 *) row;
    // 	for (int x = 0; x < bitmapWidth; ++x)
    // 	{
    // 		// Pixel in meory: 00 00 00 00 little endian
    // 		// RR GG BB xx
    // 		// 0x 00 00 00 00 
    // 		//0xxBBGGRR
    // 		// swapped RRGGBB
    // 		*pixel = (uint8) x; 
    // 		++pixel; 

    // 		*pixel = (uint8) y; 
    // 		++pixel; 

    // 		*pixel = 0 ;
    // 		++pixel; 

    // 		*pixel = 0; 
    // 		++pixel;
    // 	}
    //     row += pitch;//Increment 32 to the next pixel

    // }
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