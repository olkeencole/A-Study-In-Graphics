/* ==========================================================
$File:    BoxDemo.cpp
$Revision v 1.1
$Creator: Keenan Cole
$Notice: 
============================================================*/

/* A STUDY IN GRAPHICS
  
  API :  OpenGL 
  DEMO:  BoxDemo


  The minimum lines of code needed to
  1: Initialize OpenGL from Windows
  2: Store a Cube as Vertex Information in GL Buffers
  3: Build the Necessary Matrix Information for the Shaders
  4: Render a Series of Boxes in Perspectival Rendering

*/


#include <iostream>
#include <stdio.h>
#include <Windows.h>

#include <GL/GL.h>
using namespace std;

// UTILITY CONSTANTS
#define PI 3.14
#define DEG2RAD ((PI ) / 180.0f) 
#define Length(value) ( sizeof(value) / sizeof(value[0] ) ) //Length of an Array



/***********************************
  - OpenGL TYPEDEFS
  Since we are doing the "wrangling" or initialization of OpenGL
  ourselves, we need pull out what we need in order to create
  the functions necessary to make OpenGL work.

***********************************/
// WGL CONSTANTS FLAGS
// From "glcorearb.h"
// https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB              0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB              0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB                0x2093
#define WGL_CONTEXT_FLAGS_ARB                      0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB               0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB                  0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB     0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB           0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB  0x00000002
#define ERROR_INVALID_VERSION_ARB                  0x2095
#define ERROR_INVALID_PROFILE_ARB                  0x2096


//OPENGL CONSTANT FLAGS
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82

// OPENGL TYPE DEFS
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
typedef GLint WINAPI gl_GetUniformLocation(GLuint program, const GLchar *name);

typedef void WINAPI gl_UniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

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
gl_GetUniformLocation *glGetUniformLocation;
gl_UniformMatrix4fv   *glUniformMatrix4fv;


/// GLOBALS
static HGLRC GlobalRenderContext; 
static int WindowWidth  = 800; 
static int WindowHeight = 600; 




// SIMPLE MATH DATA STRUCTURES
// Vector3
// Matrix4


//Vector3
struct Vector3
{
    public: 
	float x, y, z; 

	Vector3(float x, float y, float z) {
		Set(x,y,z);
	} 

	Vector3() {
		Set(0,0,0);
	}

	void Set(float xv, float yv, float zv) {
		this->x = xv; 
		this->y = yv;
		this->z = zv;
	}
	//Length, Dot, Cross, 
};

	Vector3 operator+(Vector3 &v1, Vector3 &v2) {
		Vector3 result; 
		result.x = v1.x + v2.x; 
		result.y = v1.y + v2.y; 
		result.z = v1.z + v2.z; 

		return result;
	}

	Vector3 operator-(Vector3 &v1, Vector3 &v2) {
		Vector3 result; 
		result.x = v1.x - v2.x; 
		result.y = v1.y - v2.y; 
		result.z = v1.z - v2.z; 

		return result;
	}


	float Magnitude( Vector3 v) {
		float r = ( v.x * v.x + v.y * v.y + v.z * v.z );
		return  (float)sqrt( (double) r );
	}


	Vector3 Normalize(Vector3 v) {

		Vector3 result = Vector3(); 
		float length = Magnitude(v);

		if(length > 0) {
			if(v.x != 0)
			result.x = v.x / length;

			if(v.y != 0) 
			result.y = v.y / length;
			
			if(v.z != 0)
			result.z = v.z / length;
		}

		return result;
}

	void Normalize(Vector3 *v) {

		float length = Magnitude(*v);

		v->x = v->x / length; 
		v->y = v->y / length;
		v->z = v->z / length;
	}

	float Dot(Vector3 v, Vector3 rhs) {
		
		Vector3 v1 = v;// Normalize(v);
		Vector3 v2 = rhs; //Normalize(rhs);

		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); // == to Cos theta
	}

	Vector3 Cross(Vector3 v, Vector3 rhs) {

		Vector3 v1 = v; //Normalize(v);
		Vector3 v2 = rhs; //Normalize(rhs);
		Vector3 result = Vector3();

		result.x = v1.y * v2.z - v1.z * v2.y;
		result.y = v1.x * v2.z - v1.z * v2.x;
		result.z = v1.x * v2.y - v1.y * v2.x;   
		return result;
	}


	void PrintMathTest() {

	Vector3 result = Cross(Vector3(1,0,0), Vector3(0,1,0) ); //Result = Vec3(0,0,1);
	cout << "Expected: " << "0 0 1 : " ; 
	cout << "Received: " << result.x << " " << result.y << " " << result.z << endl; 

	result = Normalize(Vector3( 2, 2, 2));
    cout << "Expected: " << "0 0 1 : " ; 
	cout << "Received: " << result.x << " " << result.y << " " << result.z << endl; 
	
	float mag = Magnitude(Vector3( 2, 2, 2));
    cout << "Expected: " << "2: " ; 
	cout << "Received: " << mag << endl; // result.x << " " << result.y << " " result.z << endl; 
	
	float dotProd = Dot(Vector3( 1, 1, 1)  , Vector3( 1, 1, 1)  );
    cout << "Expected: " << "1: " ; 
	cout << "Received: " << dotProd << endl; // result.x << " " << result.y << " " result.z << endl; 
	
	dotProd = Dot(Vector3( 0, 0, 1)  , Vector3( 0, 1, 0)  );
    cout << "Expected: " << "1: " ; 
	cout << "Received: " << dotProd << endl; // result.x << " " << result.y << " " result.z << endl; 

}


class Matrix4 {

	public:
    float m[16];
	
	void Set( float ma[16] ) {
		for (int i = 0; i < 16; ++i)
		{
			this->m[i] = ma[i];
		}
	}

	void Set( float m00,float m01,float m02,float m03,
			  float m04,float m05,float m06,float m07,
			  float m08,float m09,float m10,float m11,
			  float m12,float m13,float m14,float m15 ) { 

		m[0]   = m00;
        m[1]   = m01;
        m[2]   = m02;
        m[3]   = m03;
        m[4]   = m04;
        m[5]   = m05;
        m[6]   = m06;
        m[7]   = m07;
        m[8]   = m08;
        m[9]   = m09;
        m[10]  = m10;
        m[11]  = m11;
        m[12]  = m12;
        m[13]  = m13;
        m[14]  = m14;
        m[15]  = m15;
	}


	Matrix4() {
		identity();
	}

	void identity () {
		m[0] = m[5] = m[10] = m[15] = 1.0f;
	    m[1] = m[2] = m[3]  = m[4]  = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	} 

	//TODO(keenan): Run tests and makes ure there's no glitch here
	Matrix4 Multiply(Matrix4 &m1, Matrix4 &m2)
	{	
		Matrix4 result = Matrix4(); 
		
		result.m[0]  = m1.m[0] * m2.m[0]    + m1.m[1] * m2.m[4]    + m1.m[2] * m2.m[8]     + m1.m[3] * m2.m[12]; 
		result.m[1]  = m1.m[0] * m2.m[1]    + m1.m[1] * m2.m[5]    + m1.m[2] * m2.m[9]     + m1.m[3] * m2.m[13]; 
		result.m[2]  = m1.m[0] * m2.m[2]    + m1.m[1] * m2.m[6]    + m1.m[2] * m2.m[10]    + m1.m[3] * m2.m[14]; 
		result.m[3]  = m1.m[0] * m2.m[3]    + m1.m[1] * m2.m[7]    + m1.m[2] * m2.m[11]    + m1.m[3] * m2.m[15]; 

		result.m[4]  = m1.m[4] * m2.m[0]  +   m1.m[5] * m2.m[4]    + m1.m[6] * m2.m[8]     + m1.m[7] * m2.m[12];
		result.m[5]  = m1.m[4] * m2.m[1]  +   m1.m[5] * m2.m[5]    + m1.m[6] * m2.m[9]     + m1.m[7] * m2.m[13];
		result.m[6]  = m1.m[4] * m2.m[2]  +   m1.m[5] * m2.m[6]    + m1.m[6] * m2.m[10]    + m1.m[7] * m2.m[14];
		result.m[7]  = m1.m[4] * m2.m[3]  +   m1.m[5] * m2.m[7]    + m1.m[6] * m2.m[11]    + m1.m[7] * m2.m[15];

		result.m[8]  = m1.m[8] *  m2.m[0]  +   m1.m[9] * m2.m[4]  +  m1.m[10] * m2.m[8]  +  m1.m[11] * m2.m[12];
		result.m[9]  = m1.m[8] *  m2.m[1]  +   m1.m[9] * m2.m[5]  +  m1.m[10] * m2.m[9]  +  m1.m[11] * m2.m[13];
		result.m[10] = m1.m[8] *  m2.m[2]  +   m1.m[9] * m2.m[6]  +  m1.m[10] * m2.m[10]  + m1.m[11] * m2.m[14];
		result.m[11] = m1.m[8] *  m2.m[3]  +   m1.m[9] * m2.m[7]  +  m1.m[10] * m2.m[11]  + m1.m[11] * m2.m[15];

		result.m[12] = m1.m[12] * m2.m[0]  +  m1.m[13] * m2.m[4]  +  m1.m[14] * m2.m[8]  +   m1.m[15] * m2.m[12];
		result.m[13] = m1.m[12] * m2.m[1]  +  m1.m[13] * m2.m[5]  +  m1.m[14] * m2.m[9]  +   m1.m[15] * m2.m[13];
		result.m[14] = m1.m[12] * m2.m[2]  +  m1.m[13] * m2.m[6]  +  m1.m[14] * m2.m[10]  +  m1.m[15] * m2.m[14];
		result.m[15] = m1.m[12] * m2.m[3]  +  m1.m[13] * m2.m[7]  +  m1.m[14] * m2.m[11]  +  m1.m[15] * m2.m[15];

	    return result;
	}

	Matrix4 RotateZAxis(float degrees) {
		float rad = degrees * DEG2RAD;
		Matrix4 start = *this;
		Matrix4 r  = Matrix4();

		r.m[0] = cos( rad); 
		r.m[1] = -sin(rad);
		r.m[4] = sin(rad);
		r.m[5] = cos(rad);

		return Multiply(start, r);
	}

	Matrix4 RotateYAxis(float degrees) {
		float rad = degrees * DEG2RAD;
		Matrix4 r = Matrix4();
		Matrix4 start = *this;

		r.m[0]  = cos( rad); 
		r.m[2]  = sin(rad);
		r.m[8]  = -sin(rad);
		r.m[10] = cos(rad);

		return Multiply( start, r) ;
	}

	Matrix4 RotateXAxis(float degrees) {
		float rad = degrees * DEG2RAD;
		Matrix4 start = *this;
		Matrix4 r = Matrix4();

		r.m[5]  = cos( rad); 
		r.m[6]  = -sin(rad);
		r.m[9]  = sin(rad);
		r.m[10] = cos(rad);

		return Multiply( start , r);
	}


	void Scale(float x, float y, float z){
		m[0]  = x; 
		m[5]  = y;
		m[10] = z;
	}


	void Translate(float x, float y, float z) {
		m[12] = x;
		m[13] = y;
		m[14] = z;
	}

	void Translate(Vector3 v) {
		m[12] = v.x;
		m[13] = v.y;
		m[14] = v.z;
	}
}; 


struct Camera {
	Vector3 position = Vector3(0, 0,   5.0f); 
	Vector3 forward  = Vector3(0,  0, -1.0f); // what we are looking at// direction facing // or could think of it as rotation I suppose. 

	float fieldOfView =  60.0f; 
	float nearClip = .1f; 
	float farClip  = 100.0f; 
};
 	
Camera camera; 


Matrix4 Perspective( float fov, float aspectRatio,  float n, float f) {
	Matrix4 mat;

	float tanHalfFovy = tan( fov / 2.0f); 
	float d = 1.0f / tanHalfFovy;

	float a = (n+f )/(n-f);//(-(f + n) ) / (f-n); 
	float B = (2 * n * f)/ (n-f); //(-2 * f * n)    / (f-n);

	mat.Set( 	   d/aspectRatio,          0,      0,    0 , 
				               0,          d,      0,    0 ,
     			               0,          0,     a,    -1 ,
				               0,      	0.0f,     B,    0 );
	return mat;
}


Matrix4 LookAt(Vector3 pos, Vector3 target, Vector3 up) {

	Vector3 facing  = Normalize( pos - target); 
	Vector3 right   = Normalize( Cross( up, facing));    
	Vector3 localUp = Normalize( Cross( right, facing)); 

	Matrix4 result = Matrix4();

	result.Set(    right.x,    localUp.x,  facing.x, 0, 
			       right.y,    localUp.y,  facing.y, 0,
			       right.z,    localUp.z,  facing.z, 0,
	      -Dot(right, pos),     -Dot(localUp, pos),    -Dot(facing, pos),   1.0f );
	return result;
}


/*
  VERTEX AND FRAG SHADER
  Simple unlit shaders that just use the MVP matrix and spit out 
  some vertex colors
*/

const char* vertexShader = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 verts;\n" 
	"layout (location = 1) in vec3 vColor;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 proj;\n"
	"out vec3 color;\n" 

	"void main() { \n"
		"gl_Position = proj * view * model * vec4(verts.x, verts.y, verts.z, 1.0);\n"
		"color = vColor;\n"
	"}\n\0" ;

const char* fragmentShader = 
	"in vec3 color;\n"
	"void main(){\n"
		"gl_FragColor = vec4(color, 1.0);\n" 
	"}\n\0";




static GLuint VAO,VBO, Shader; 
float zRotation = 0;

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


// Vertice Data for for a Cube
// Vertex Infomation with Position and Color Stored in the Vertices
float Cube_Color[] = {
    // Vert Position          // Color          
    -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 0.0f,  
     0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  
    -0.5f,  0.5f, -0.5f,  1.0f,  0.0f, 0.0f,  
    -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 0.0f,  

    -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 0.0f,   
     0.5f, -0.5f,  0.5f,  0.0f,  1.0f, 0.0f,   
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   
    -0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   
    -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 0.0f,   

    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  
    -0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 1.0f,  
    -0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 1.0f,  
    -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 0.0f,  
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  

     0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 0.0f,  
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f, 0.0f,  
     0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 0.0f,  

    -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 1.0f,  
     0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f, 1.0f,  
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f, 1.0f,  
    -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 0.0f,  
    -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 1.0f,  

    -0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 0.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 
    -0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f, 
    -0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 0.0f
};


void InitializeOpenGLBuffers()
{
	glEnable(GL_DEPTH_TEST);  

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube_Color), Cube_Color, GL_STATIC_DRAW );

	int index = 6; 
	glVertexAttribPointer(0 , 3,  GL_FLOAT, GL_FALSE, sizeof(float) * index ,   (void *) 0); //6 
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1 , 3,  GL_FLOAT, GL_FALSE, sizeof(float) * index ,   (void *) ( 3 * sizeof(float) ) ); //6 
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

 } 

 void InitShader()
 {
 	//Debugging
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


 //Draw Function
 void RenderOpenGLContext(GLuint shader)
 {
 	//PREPARE CAMERA AND MATRICES
 	Matrix4 model = Matrix4(); //Constructor sets to Identity

 	Matrix4 view = Matrix4();
 	view.identity();
 	view = LookAt(camera.position,  camera.position+camera.forward, Vector3(0, 1.0f, 0.0f) );
	
 	Matrix4 proj = Matrix4();
 	proj = Perspective( (camera.fieldOfView * DEG2RAD),  ( (float) WindowWidth / (float)WindowHeight ), camera.nearClip, camera.farClip); 

 	glUseProgram(shader); 

 	GLuint shader_model_id = glGetUniformLocation(shader, "model"); 
 	glUniformMatrix4fv(shader_model_id, 1, GL_FALSE,  &model.m[0]);

 	GLuint shader_view_id = glGetUniformLocation(shader, "view"); 
 	glUniformMatrix4fv(shader_view_id, 1, GL_FALSE,   &view.m[0] );

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
			if(wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}

			if(wParam == VK_RIGHT)
			{
				modelPosition.x += .2f;
			}
			if(wParam == VK_LEFT)
			{
				 modelPosition.x -= .2f;
			}
			if(wParam == VK_DOWN) {
				 modelPosition.y  -= .2f;
				//camera.position.z += .2f;
			}
			if(wParam == VK_UP) {
				 modelPosition.y += .2f;
				 //camera.position.z -= .2f;
			}
		}break;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam ,lParam);
}

bool Running = false;

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



	//MSG msg = {}; 
	Running = true;
	while(Running ) 
	{
		MSG msg; 
		//GetMessage( &msg, NULL, 0, 0))  
		if(PeekMessage(&msg, 0,0, 0, PM_REMOVE )) { 

			if( msg.message == WM_QUIT)
			{
				Running = false;
			}
			TranslateMessage(&msg); 
			DispatchMessage( &msg); 
		}

		HDC DeviceContext = GetDC(WindowHandle);
		glViewport(0,0, WindowWidth, WindowHeight);
		glClearColor(0.0f, .0f, .0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderOpenGLContext(Shader);
		SwapBuffers(DeviceContext);
	}


	return 0;
}