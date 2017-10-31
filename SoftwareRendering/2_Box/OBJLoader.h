#ifndef OBJLOADER_H
#define OBJLOADER_H


#include <vector>
#include "math.h"
#include <iostream>
#include <fstream>
#include <string> 
#include <cctype>
#include <sstream>
#include <stdio.h>

const int OBJ_ERROR    = 0; 
const int OBJ_VERTEX   = 1;
const int OBJ_NORMAL   = 2;
const int OBJ_FACE     = 3; 
const int OBJ_TEXCOORD = 4;
const int OBJ_OBJECT   = 5;
const int OBJ_MATERIAL = 6;
const int OBJ_USEMTL   = 7;

  


//C++ String Method
/*
	string text; Stores the read file in a string 
	fstream file; stores the file data
	stringstream ; process the string from the file | intermediary between file and string 
*/

struct Vertex
{
	Vector3 position; 
	Vector3 normal; 
	Vector3 color;
	Vector2 uv; 
};

struct Mesh 
{
	public:
	vector<Vertex> vertices;
    vector<unsigned int> indices; 

    Mesh() {

    }
	Mesh(vector<Vertex> verts, vector<unsigned int> faces ){
		vertices = verts;
		indices = faces;
	}
};

struct FaceIndexData
{
	vector<unsigned int> faces; 
	vector<unsigned int> texcoords; 
	vector<unsigned int> normals; 
}; 

struct mtl
{
	string name; 
	Vector3 mainColor; 


	//Texture texture1;

}; 	


Mesh InitializeMesh(vector<Vector3> posVerts,  vector<Vector2> uvVerts, vector<Vector3> normVerts, FaceIndexData faceData );
// void GetMaterials(  string path, string fileName, vector<mtl> *materials);

//enum

int currentState = 0;
int GetState(string line)
{
	char firstChar = line[0];

	if(firstChar  == '#' || firstChar == 'g')
	return 0;

	//check next char to see if it is vn
	if(firstChar == 'v')
		if(line[1]=='n')
			return 2;
		else if( line[1] == 't')
			return 4;
		else
			return 1;  

	if(firstChar == 'f')
	{
		return 3;
	}

	if(firstChar == 'o')
		return OBJ_OBJECT;  

	if(firstChar == 'm')
	{
		if(line[1] == 't')
			return OBJ_MATERIAL;
	}

	if(firstChar == 'u')
	{
		return OBJ_USEMTL;
	}


	return 0;
}

int CheckSlash(stringstream *parser){

	//check if a slash exists
	int val = -1;
		if(parser->peek() == '/')
		{
			parser->ignore(1); // ignore the slash
			//check if next one is a digit TODO
			// if(parser->isdigit( parser->peek() ) )
			*parser >> val; 
		}
		return val;
}


// Texture GetTextureFromLabel( string label, vector<mtl> *materials )
// {
// 	for (int i = 0; i < materials->size(); ++i)
// 	{
// 		mtl mat = materials->at(i); 

// 		//cout << "labeled desired: " << label << " " << "labeled checked " << mat.name << endl;
// 		if( mat.name == label )
// 			return mat.texture1;
// 	}

// }


Mesh LoadMeshOBJ(char* path, char* fileName)
{	
	vector<Mesh> meshes;

	ifstream file;
	string text, line, name; 
	stringstream intermediary; 

	string pathString = path; 
	string fName      = fileName;

	file.open(pathString + fName);

	if( !file.good( ))
	{
		cout << fName << " " << " could not be opened" << endl;
	}
	else
	{
		cout << fName << " opened successfully" << endl ;
	}

	text = intermediary.str();

	vector<Vector3> posVerts; 
	vector<Vector3> normVerts; 
	vector<Vector2> uvVerts;
	vector<mtl> materials; 
	
	//vector<Texture> textures; 
	//Texture texID;

	FaceIndexData faceIndexData;

	vector<unsigned int> faces;
	vector<unsigned int> faceUVindices; 
	vector<unsigned int> faceNormIndices;

	while(!file.eof() )
	{
		//First part: Get State
		getline(file, line);

		currentState = GetState(line);
		if( currentState != OBJ_ERROR)
		{
			Vertex vert;
			stringstream parser; 
			parser << line;

			//Verts
			if(currentState == OBJ_VERTEX)
			{
				Vector3 p; 
				char state; 

				parser >> state ;
				parser >> p.x >> p.y >> p.z ;

				posVerts.push_back(p); 
			}

			if( currentState == OBJ_NORMAL)
			{
				Vector3 p; 
				char state; 

				parser >> state >> state ; 
				parser >> p.x >> p.y >> p.z ;

			
				normVerts.push_back(p);
			}

			// Faces
			if(currentState == OBJ_FACE)
			{
				Vector3 p;
				Vector3 uv; 
				Vector3 n; 

				char state; 

				parser >> state ; // command, could just ignore
				parser >> p.x ;
				uv.x = CheckSlash(&parser);
				n.x  = CheckSlash(&parser);
				
				parser	>> p.y ; //>> p.z ;
				uv.y = CheckSlash(&parser);
				n.y  = CheckSlash(&parser);

				parser	>> p.z ; //>> p.z ;
				uv.z = CheckSlash(&parser);
				n.z = CheckSlash(&parser);	

				//cout << p.x  << endl;

				faces.push_back(p.x); 
				faces.push_back(p.y); 
				faces.push_back(p.z); 

				if(uv.x > 0)
				{
					faceUVindices.push_back(uv.x);
					faceUVindices.push_back(uv.y);
					faceUVindices.push_back(uv.z);

				}

				if(n.x > 0)
				{
					faceNormIndices.push_back(n.x);
					faceNormIndices.push_back(n.y);
					faceNormIndices.push_back(n.z);
				}

			}

				//vt
			if(currentState == OBJ_TEXCOORD)
			{
				Vector2 p; 
				char state; 

				parser >> state >> state ; //character by character
				parser >> p.x >> p.y ;

				uvVerts.push_back(p); 

			}

			if(currentState == OBJ_OBJECT)
			{
				string eatMe;
				char state;
				parser >> state;
				parser >> eatMe;
				cout << state << " " << eatMe << " " << endl;

				if(posVerts.size() > 0) { //OF course, if we encounter an o before we have anything, we can't accept it yet
					//finalize the current mesh info into a new mesh

					faceIndexData = {}; 

					faceIndexData.faces     	  = faces;
					faceIndexData.texcoords 	  = faceUVindices; 
					faceIndexData.normals         = faceNormIndices; 

					Mesh mesh = InitializeMesh(posVerts, uvVerts, normVerts, faceIndexData);

					//cout << "Tex Name: " << texID.name << " : ID: " << texID.id <<endl;
				  // if(texID.id > -1 && texID.id < 100)
				//	mesh.SetTexture( texID);
				
					meshes.push_back( mesh);

					// Reinitialize
					//posVerts.clear();
					//uvVerts.clear();
					//normVerts.clear();
					faces.clear();
					faceUVindices.clear();
					faceNormIndices.clear();
					//texID = {};
				}
			}

			if(currentState == OBJ_MATERIAL)
			{
				string state; 
				string matName; 

				parser >> state; 
				parser >> matName; 

				cout << matName << endl;
				string matPath =  path; //"Models/yoshi/"; //STILL HARDCODING THIS
				string fullPath = matPath + matName;

				cout << fullPath << endl;
				//cout << fullPath; 
				//GetMaterials(matPath ,matName, &materials );
				printf("Material Size: %zd \n", materials.size() );
				//NOw we need the path and process the path 

			}

			if(currentState == OBJ_USEMTL)
			{
				string state; 
				parser >> state;
				string textureName; 
				parser >> textureName; 
				
				//texID = GetTextureFromLabel( textureName , &materials);

				//cout << "Texture found: " << texID.name << " : "<< texID.id <<  endl;
			}

			// todo : be sure to compare between your importer and others and note the differences and why
		}
	}

	//Wrap up with the final mesh
	faceIndexData = {}; 

	faceIndexData.faces     	  = faces;
	faceIndexData.texcoords 	  = faceUVindices; 
	faceIndexData.normals         = faceNormIndices; 


	Mesh newMesh = InitializeMesh(posVerts, uvVerts, normVerts, faceIndexData);

	//cout << "Attempting to Set Texture: " <<endl; 
	//cout << path << " : " << texID.name << " : "<< texID.id <<  endl;
	
	//if(texID.id > -1 && texID.id < 100)
	//newMesh.SetTexture(texID);

	meshes.push_back(newMesh);
	
	return meshes[0]; //Model(meshes);
}


void GetMaterials(string path, string fileName, vector<mtl> *materials)
{
	ifstream file;
	string text, line, command; 
	string fullPath = path + fileName;

	stringstream intermediary; 
	//cout << "Material Path: " << path << endl; 

	file.open(fullPath);

	if(!file.good( ))
	{
		cout << "Material File : " << fileName << " could not be opened" << endl ;
		return;
	}
	else
		cout << fileName << " opened sucessfully" <<endl;

	text = intermediary.str();

	mtl material;

	while( ! file.eof() )
	{
		getline( file, line);
		stringstream parser; 
		parser << line;
		parser >> command;

		if( command == "newmtl" )  {
			//reset 
			if(material.name != "")
			{
				materials->push_back( material);
			}

			string name; 
			parser >>  name; 
			//cout << "Material Name: "<< name << endl;

			material = mtl();
			material.name = name;

		}



		if( command == "Kd" )  { 
			
			Vector3 color; 
			parser >> color.x >> color.y >> color.z; 
			material.mainColor = color;

		}

		if( command == "map_Kd" )  { 
						
			string textureName;
			string textureFilePath;

			parser >> textureName;

			textureFilePath = path + textureName;

			cout << "Texture Path: " << textureFilePath << endl;
			//material.texture1.name = textureName;

			//How do we know if this broke? 
			//material.texture1.id = GetTextureID( textureFilePath.c_str() );
		}		
	}
	if(material.name != "")
	{
		materials->push_back( material);
	}

}


Mesh InitializeMesh(vector<Vector3> posVerts, vector<Vector2> uvVerts, vector<Vector3> normVerts,  FaceIndexData faceIndexData )
{
	vector<Vertex> verts;
	vector<unsigned int> faces = faceIndexData.faces;

	// if(uvVerts.size() != posVerts.size() )
	// {
	// 	cout << "Uvs and Verts do not match" ;
	// }
	// //
	// for (int i = 0; i < posVerts.size(); ++i)
	// {
	// 	Vertex newVert; 
	// 	newVert.position = posVerts[i]; 
	// 	//newVert.uv 	     = uvVerts[i];
	// 	//newVert.normals  = normVerts[i]; 
	// 	vector<Vector3> norms; 

	// 	int offset  = 0; 
	// 	if( i > 0) offset =  i / 3;
	// 	offset *= 3;

	// 	//obj specs uses a 1 based offset rather than a 0;
	// 	Vector3 p1 = posVerts[ offset      ]; 
	// 	Vector3 p2 = posVerts[ offset + 1  ]; 
	// 	Vector3 p3 = posVerts[ offset + 2  ];

	// 	Vector3 u = p2 - p1; 
	// 	Vector3 v = p3 - p1;

	// 	Vector3 norm = normalize( cross( u, v) ) ;
	// 	newVert.normal = norm; //norms.push_back(norm);

	// 	verts.push_back(newVert);
	// }

	// return Mesh( verts, faces);
	bool flag1 = false;
	bool flag2 = false;

	for (int i = 0; i < faceIndexData.faces.size(); i++)
	{
		//cout << "Face Count: " + faceIndexData.faces.size();
		Vertex vert;
		///cout << "Index: " << i << " : " << (faces[i] -1 ) << endl; 

		//Get Face Offset Into the Verts
		int f        = faceIndexData.faces[i] - 1; 
		int uvOffset = -1; 

		if(flag1 == false && f > posVerts.size() ) {
			cout << "Index: " << i << " - Face Index Out of Vertex Bounds: " << "vSize: " << posVerts.size() << " - f Index: " << f << endl;
		}
		else
			vert.position = posVerts[ f ];

		if(faceIndexData.texcoords.size() > 0)
		{
			//cout << "UV Count  : " + faceIndexData.texcoords.size();
		 	uvOffset = faceIndexData.texcoords[i] - 1;

		 	if(uvOffset > uvVerts.size()) {
		 		cout << "UV Face Offset Out of UV Array Bounds" << endl;
		 	}
		 	else
		 		vert.uv = uvVerts[uvOffset];
		}
		
		if( normVerts.size() == 0 )
		{
			int offset  = 0; 
			if( i > 0) offset =  i / 3; //Rounding to the nearest 3rd
			offset *= 3;

			//obj specs uses a 1 based offset rather than a 0;
			Vector3 p1 = posVerts[ faces[offset  ]   -1 ]; 
			Vector3 p2 = posVerts[ faces[offset + 1] -1 ]; 
			Vector3 p3 = posVerts[ faces[offset + 2] -1 ];

			Vector3 u = p2 - p1; 
			Vector3 v = p3 - p1;

			Vector3 norm = Normalize( Cross( u, v) ) ;
			vert.normal = norm;
			//cout << "Calc Norm " << norm.x << endl;
		}
		else
		 vert.normal   = normVerts[ faces[i ] - 1];

		// // Do UV verts and verts match up? Do we even have any uvVerts? 
		// if(uvVerts.size() > 0 && faceIndexData.texcoords.size() == 0 && uvVerts.size() == verts.size() )
		// for (int i = 0; i < verts.size(); ++i)
		// {
		// 	verts[i].uv = uvVerts[i];
		// }

		verts.push_back(vert);
	}

	Mesh m(verts, faces);
	return m;
}





#endif