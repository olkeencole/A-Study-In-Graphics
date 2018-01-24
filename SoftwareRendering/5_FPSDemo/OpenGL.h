#ifndef OPENGL_H
#define OPENGL_H

#include <GL/GL.h>

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


//Send the vertices to the GPU and say how the data is laid out
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




#endif