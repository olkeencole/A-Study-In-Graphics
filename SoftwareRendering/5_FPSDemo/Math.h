
// SIMPLE MATH DATA STRUCTURES
// Vector3
// Matrix4
#include <iostream>
using namespace std;

#define PI 3.14
#define DEG2RAD ((PI ) / 180.0f) 


struct Vector2
{
public:
	float x, y;

	Vector2 (){
		x = 0; 
		y = 0;
	}
	Vector2(float xVal, float yVal){
		this->x = xVal;
		this->y = yVal;
	}

};

//Vector3

struct Vector4{
public:
	float x,y,z, w;
	Vector4(){
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vector4(float nx, float ny, float nz, float nw) {
		x = nx;
		y = ny; 
		z = nz; 
		w = nw;
	} 


};

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

	

	Vector3 operator+=(Vector3 &rhs) {
		
		this->x += rhs.x ; 
		this->y += rhs.y ; 
		this->z += rhs.z ; 

		return *this;
	}


	Vector3 operator-=(Vector3 &rhs) {
		
		this->x -= rhs.x ; 
		this->y -= rhs.y ; 
		this->z -= rhs.z ; 

		return *this;
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

	Vector3 operator-(const Vector3 &v1, const Vector3 &v2) {
		Vector3 result; 
		result.x = v1.x - v2.x; 
		result.y = v1.y - v2.y; 
		result.z = v1.z - v2.z; 

		return result;
	}

	
	Vector3 operator*(Vector3 &v1, float v) {
			Vector3 result;
			result.x = v1.x * v ; 
			result.y = v1.y * v ; 
			result.z = v1.z * v ; 

			return result;
	}

	float Magnitude( Vector3 v) {
		float r = ( v.x * v.x + v.y * v.y + v.z * v.z );
		if(r < 0)
			r *= -1;
		return  (float)sqrt( (double) r );
	}


	Vector3 Normalize(Vector3 v) {

		Vector3 result = Vector3(); 
		float length = Magnitude(v);

		if(length != 0) {
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
		Vector3 v2 = rhs; //Normalizrhs);
		Vector3 result = Vector3();

		result.x = v1.y * v2.z - v1.z * v2.y;
		//result.y = v1.x * v2.z - v1.z * v2.x;///backwardsa?
		result.y = v1.z * v2.x - v2.z * v1.x; 
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
	static Matrix4 Multiply(Matrix4 &m2, Matrix4 &m1)
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

		//TODO(keenan): Run tests and makes ure there's no glitch here
	static Matrix4 MultiplyTest(Matrix4 &m1, Matrix4 &m2)
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

	void Scale( Vector3 s){
		Scale( s.x, s.y, s.z);
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


Matrix4 operator*(Matrix4 &m, Matrix4 &m1){
	return Matrix4::Multiply(m, m1);
}

// Vector3 operator*(Matrix4 &m, Vector3 &v) {
// 		Vector3 result = Vector3();

// 		//First row of matrix down through the vector
// 		result.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2]  * v.z  + m.m[3]  * 0.0f;
// 		result.y = m.m[4] * v.x + m.m[5] * v.y + m.m[6]  * v.z  + m.m[7]  * 0.0f;
// 		result.z = m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z  + m.m[11] * 0.0f;
// 		return result;	
// }


inline Vector4 operator*(Matrix4 &m, Vector4 &v) {
		Vector4 result = Vector4();

		//First row of matrix down through the vector
		result.x = m.m[0]  * v.x +   m.m[4]  * v.y    + m.m[8]     * v.z   + m.m[12]   * v.w;
		result.y = m.m[1]  * v.x +   m.m[5]  * v.y    + m.m[9]     * v.z   + m.m[13]   * v.w;
		result.z = m.m[2]  * v.x +   m.m[6]  * v.y    + m.m[10]    * v.z   + m.m[14]   * v.w;
		result.w = m.m[3]  * v.x +   m.m[7]  * v.y    + m.m[11]    * v.z   + m.m[15]   * v.w;
		return result;	
}

// Vector4 operator*(Matrix4 &m, Vector4 &v) {
// 		Vector4 result = Vector4();

// 		//First row of matrix down through the vector
// 		result.x = m.m[0]  * v.x +   m.m[1]  * v.y    + m.m[2]     * v.z   + m.m[3]   * v.w;
// 		result.y = m.m[4]  * v.x +   m.m[5]  * v.y    + m.m[6]     * v.z   + m.m[7]   * v.w;
// 		result.z = m.m[8]  * v.x +   m.m[9]  * v.y    + m.m[10]    * v.z   + m.m[11]   * v.w;
// 		result.w = m.m[12]  * v.x +  m.m[13]  * v.y    + m.m[14]   * v.z   + m.m[15]   * v.w;
// 		return result;	
// }
// UTILITY FUNCTIONS

inline Matrix4 Perspective( float fov, float aspectRatio,  float n, float f) {
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

void PrintVector( Vector3 v) {
	char buffer[256];
	wsprintf(buffer, "Vec3 : %d %d %d \n", v.x, v.y, v.z );
}

Matrix4 LookAt(Vector3 pos, Vector3 target, Vector3 up) {

	Vector3 facing  = Normalize(  pos - target);  // why not center eye?	
	Vector3 right   = Normalize( Cross(up, facing));    // does order matter?
	Vector3 localUp = Normalize( Cross( facing, right)); 

	Matrix4 result = Matrix4();

	result.Set(    right.x,    localUp.x,  facing.x, 0, 
			       right.y,    localUp.y,  facing.y, 0,
			       right.z,    localUp.z,  facing.z, 0,
	      -Dot(right, pos),    -Dot(localUp, pos),    -Dot(facing, pos),  1.0f );

	char buffer[256];
	wsprintf(buffer, "LocalUp: %d %d %d - UP %d %d %d", localUp.x, localUp.y, localUp.z, up.x, up.y, up.z );
	return result;
}

float Lerp( float a, float b, float index) {

	return ( a + (b-a) * index ); 
}


float Clamp(float value, float min, float max){

	float result = value;
	if(value < min)
		result = min;

	if(value > max)
		result = max;

	return result;
}




inline int ABS(int val){

	if(val < 0) return val * -1;

	 return val;
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
// Matrix4 LookAt(Vector3 pos, Vector3 target, Vector3 up) {

// 	Vector3 facing  = Normalize( target - pos);  // why not center eye?	
// 	Vector3 right   = Normalize( Cross( facing,   up));    // does order matter?
// 	Vector3 localUp = Normalize( Cross( right, facing)); 

// 	Matrix4 result = Matrix4();

// 	result.Set(    right.x,    localUp.x,  -facing.x, 0, 
// 			       right.y,    localUp.y,  -facing.y, 0,
// 			       right.z,    localUp.z,  -facing.z, 0,
// 	      -Dot(right, pos),    -Dot(localUp, pos),    Dot(facing, pos),  1.0f );

// 	char buffer[256];
// 	wsprintf(buffer, "LocalUp: %d %d %d - UP %d %d %d", localUp.x, localUp.y, localUp.z, up.x, up.y, up.z );
// 	return result;
// }