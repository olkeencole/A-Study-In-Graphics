/* ==========================================================
$File: Keenan Cole
$Revision
$Creator
$Notice: 
============================================================*/

/* A Study In Graphics
   API: OpenGL
   Demo: Simple Triangle With Color Interpolation

   - As simple as a script can be initializing OpenGLCore from the 
     Windows API and displaying a polygon

*/ 


#include <Windows.h>
#include <GL/GL.h>
#include <iostream>

using namespace std;


// Triangle Data with Vertex Color
//
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


// Draw With Old OpenGL 1.1
// 
 void RenderOldOpenGL()
 {
		//RenderOpenGLContext(Shader);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION); 
		glLoadIdentity();

		glBegin(GL_TRIANGLES); 
		float p = .7f; 
		glColor3f( 1.0f, 0.0f, 0.0f);
		glVertex2f(-p, -p);

		glColor3f( 0.0f, 1.0f, 0.0f);
		glVertex2f( p, -p);

		glColor3f( 0.0f, 0.0f, 1.0f);
		glVertex2f( 0,  p); 

		glEnd();
 }


// Windows Message Callback
// 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg)
	{
		case WM_CREATE: 
		{
			//Describe the type of Pixel to Display
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
			
			// Did we get the initial graphics context? 
			// Then Now we can create the newer context
			// And Load our function pointers with addresses
			if( wglMakeCurrent( DeviceContext, renderContext) )
			{
				cout << "Making WGL Current and attempting to fetch functions" << endl;
				MessageBoxA(0 , (char*) glGetString(GL_VERSION), "OPENGL_VERSION", 0);
				
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

				glGetProgramiv       = (gl_GetProgramiv      *) wglGetProcAddress("glGetProgramiv"); 
				glGetProgramInfoLog  = (gl_GetProgramInfoLog *) wglGetProcAddress("glGetProgramInfoLog"); 
				glGetShaderiv        = (gl_GetShaderiv       *) wglGetProcAddress("glGetShaderiv"); 
				glGetShaderInfoLog   = (gl_GetShaderInfoLog  *) wglGetProcAddress("glGetShaderInfoLog"); 
				
				// wglGetProcAddrss("wglCreteContextAttribARB");
				if(wglCreateContextAttribARB ) {
						OutputDebugString( "Found Contrext Attrib Function\n" );
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
				        // MessageBoxA(0 , (char*) ("Created Modern Context"), "OPENGL_VERSION", 0);

						InitOpenGL();
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
			glClear(GL_COLOR_BUFFER_BIT);

			RenderOpenGLContext(Shader);
			//RenderOldOpenGL();
			SwapBuffers(DeviceContext); 
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