/* ==========================================================
$File: Keenan Cole
$Revision
$Creator
$Notice: 
============================================================*/

/* A Study In Graphics
   API: Software Rendering
   Demo: Perspective Correct Rendering of a bunch of Boxes

   - As simple as a script can be initializing the Windows API and using
   it to render a simple triangle from some vertices

*/ 
#include <Windows.h>
#include <GL/GL.h>
#include <iostream>
#include <stdint.h>
#include "OBJLoader.h"
#include "model.cpp"
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

void FillFlatBottomTriangle( Vector3 v1, Vector3 v2, Vector3 v3, uint32 color) {

	// float step =  v3.y - v1.y; 
	//if(v1.y > v2.y) return; 

	float invSlope1 = (v2.x - v1.x) / (v2.y - v1.y) ;
	float invSlope2 = (v3.x - v1.x) / (v3.y - v1.y) ;

	float curX1 = v1.x; 
	float curX2 = v1.x; 

	for (int scanLine = v1.y; scanLine <= v2.y; scanLine++) //scanline is walking down the y from top to bottom
	{
		DrawLine( color, curX1, scanLine,   curX2, scanLine );
		curX1 += invSlope1; 
		curX2 += invSlope2;   
	}
}

//Assume that v1 and v2 have the same y value
void FillFlatTopTriangle( Vector3 v1, Vector3 v2, Vector3 v3, uint32 color) {

	float invSlope1 = (v3.x - v1.x) / (v3.y - v1.y); 
	float invSlope2 = (v3.x - v2.x) / (v3.y - v2.y); 

	float curX1 = v3.x; 
	float curX2 = v3.x; 

	for (int scanLine = v3.y; scanLine >= v2.y; scanLine--) //Going up the screen
	{
		DrawLine( color, curX1, scanLine, curX2, scanLine);
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

void DrawTriangle(Vector3 v1, Vector3 v2, Vector3 v3, uint32 color){

	//Assume its a general triangle

	//bubble sort
	if(v1.y > v2.y) Swap(&v1, &v2);
	if(v1.y > v3.y) Swap(&v1, &v3);
	if(v2.y > v3.y) Swap(&v2, &v3);

	Vector3 v4 = Vector3();
	v4.x = v1.x  + (v2.y - v1.y) / ( (v3.y - v1.y) ) * (v3.x - v1.x ); 
	v4.y = v2.y; 
	//v4.x = v3.x; 

	FillFlatBottomTriangle(v1, v2, v4, color); 

	FillFlatTopTriangle(v2, v4, v3, color); 
}

void DrawTriangle(Vector3 v1, Vector3 v2, Vector3 v3){

	DrawTriangle(v1, v2, v3, GetColor(1.0f, 1.0f, 1.0f) ); 
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

// Vector3 barycentric(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p){

// 	Vector3 u = Cross( Vector3(  Vector3( p3 - p1).x, 
// 		                         Vector3( p2 - p1).y, 
// 		                         Vector3( p1 - p).z )
// 					    , 
// 					    Vector3(  Vector3( p3 - p1).x, 
// 		                         Vector3(  p2 - p1).y, 
// 		                         Vector3(  p1 - p).z ) ) ;
// }

// Vec3f barycentric(Vec2i *pts, Vec2i P)
//  { 
//     Vec3f u = cross(Vec3f( pts[2][0] - pts[0][0],
//                            pts[1][0] - pts[0][0],
//                            pts[0][0] - P[0]),

//                      Vec3f(pts[2][1] - pts[0][1], 
//                    	       pts[1][1] - pts[0][1], 
//                    	       pts[0][1] - P[1]));


//     if (std::abs(u[2])<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates 
   
//     return Vec3f(1.f-(u.x + u.y) / u.z, u.y / u.z, u.x / u.z); 
// } 

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
 
 Mesh model; 

 Vector3 Convert2(Vec3f v){
 	Vector3 result = Vector3(v.x, v.y, v.z);
 	return result;
 }

 void RenderUpdate(){

    int stride = 6; 
    int count = ArrayCount( vertices) / stride; 
    int realCount = ArrayCount(vertices);

  Vector3 triangle[3] ; 
  Vector3 flatTop[3]; 
  Vector3 skewed[3]; 

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
 

    //Draw last Vert to First Vert

    Vector3 lightPos = Vector3(0, 0, 5.0f);

    int faceCount = model.indices.size() / 3;

    for (int i = 0; i < model.vertices.size(); ++i)
    {
    	
    	//DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[i].position,   model.vertices[i+1].position );
    }

    for (int i = 0; i <  model.vertices.size();  i+= 3)
   {
   	int index0 = i;//  model.indices[i]; 
   	int index1 = i+1; //model.indices[i+1];
   	int index2 = i+2; //model.indices[i + 2];

   	Vector3 lightDir = lightPos - model.vertices[0].position; 
    lightDir = 	(lightPos);

   	Vector3 normalAverage = model.vertices[index0].normal + model.vertices[index1].normal + model.vertices[index2].normal;
   	normalAverage = Normalize(normalAverage);

   	float intensity = Dot(  normalAverage, Normalize(lightDir) ); 

  	// if(intensity > 0 )
   // 	DrawBaryTriangle( ConvertNDCToScreen( model.vertices[index2].position), 
			// 	      ConvertNDCToScreen( model.vertices[index1].position), 
			// 	      ConvertNDCToScreen( model.vertices[index0].position),
			// 	      GetColor( intensity , intensity,  intensity)  );
    	
	//DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index0].position,   model.vertices[index1].position );
	//DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index1].position,   model.vertices[index2].position );
	//DrawLineNDC(GetColor(1.0f,1.0f, 1.0f), model.vertices[index2].position,   model.vertices[index0].position );	

    }

          // DrawWireTriangle((skewed[2]), (, [0]), (skewed[1])) ; 

    DrawBaryTriangle(ConvertNDCToScreen(triangle[2]),
                 ConvertNDCToScreen(triangle[1]), 
                 ConvertNDCToScreen(triangle[0]), 
    	         colors24[2], colors24[1], colors24[0] ) ; 



   //  Model headModel = Model("models/african_head.obj");
   //  Vector3 lightPos2 = Vector3(0,0, 5.0f);

   // for (int i = 0; i <  headModel.nfaces();  i++)
   // {
   // 	vector<int> face = headModel.face(i); 

   // 	int index0 = face[0]; 
   // 	int index1 = face[ 1];
   // 	int index2 = face[2];

   // //	Vector3 lightDir = lightPos - model.vertices[0].position; 
   // 	//Normalize(lightDir);
   // 	Vector3 lightDir = lightPos2 - Convert2( headModel.verts_[index0] ); 
   // 	//Normalize(lightDir);

   // 	Vector3 n = Cross( Convert2( headModel.verts_[index1] - headModel.verts_[index0] ), 
   // 		               Convert2( headModel.verts_[index2] - headModel.verts_[index0] ) ) ;
   // 	n = Normalize(n);

   // 	//Vector3 normalAverage = model.vertices[index0].normal + model.vertices[index1].normal + model.vertices[index2].normal;
   // 	//Normalize(normalAverage);

   // 	float intensity =  Dot(  Normalize(lightPos2), n); 

  	// if(intensity >= 0 )
   // 	DrawBaryTriangle( ConvertNDCToScreen(  Convert2( headModel.verts_[index2]) ) , 
			// 	      ConvertNDCToScreen( Convert2( headModel.verts_[index1]) ) , 
			// 	      ConvertNDCToScreen( Convert2( headModel.verts_[index0]) ) ,
			// 	      GetColor(  intensity ,  intensity,    intensity )  );
    	
	

   // }
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

    //}
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


    RenderUpdate();

          // DrawWireTriangle((skewed[2]), (, [0]), (skewed[1])) ; 

    // DrawBaryTriangle(ConvertNDCToScreen(triangle[2]),
    //              ConvertNDCToScreen(triangle[1]), 
    //              ConvertNDCToScreen(triangle[0]), 
    // 	         colors24[2], colors24[1], colors24[0] ) ; 



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

	model = LoadMeshOBJ("models/", "african_head.obj");

	
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