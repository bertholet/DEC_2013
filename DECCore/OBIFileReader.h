#pragma once
#include <vector>
#include "tuple3.h"
#include "wfmesh.h"
#include "DECCORE_EXPORT.h"

using namespace std;

class DECCORE_EXPORT OBIFileReader
{
	vector<tuple3f> vertices;
	vector<tuple3f> normals;
	vector<tuple3i> faces;
	vector<tuple3i> faces_normals;
	vector<tuple3i> faces_textures;
	vector<tuple3f> tex;

public:
	OBIFileReader();
	~OBIFileReader(void);

	void parse(const char * file);
	void initializeMesh(wfMesh & mesh);

private:
	vector<tuple3f> getVertices(){return vertices;}
	vector<tuple3f> getNormals(){return normals;}
	vector<tuple3f> getTex(){return tex;}
	vector<tuple3i> getFaces(){return faces;}
	vector<tuple3i> getFaceNormals(){
		return faces_normals;}
	vector<tuple3i> getFaceTextures(){
		return faces_textures;}

	void checkAllVerticesUsed(void);
	//int hasVertex(void);
	//int hasFace(void);
	
	//void operator >>(Vertex * other);
	//void operator >>(Face * other);

};
