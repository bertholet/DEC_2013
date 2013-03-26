#pragma once
#include <vector>
#include "tuple3.h"
#include "floatVector.h"
#include "Observer.h"
#include "DECCORE_EXPORT.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// The messages that can be sent to observers: any vertex position 
// change or geometry/connectivity changes
//////////////////////////////////////////////////////////////////////////
enum DECCORE_EXPORT meshMsg{POS_CHANGED, CONNECTIVITY_CHANGED, MESH_DELETED, TEX_CHANGED};

class meshObserver{
public:
	virtual void update(void * src, meshMsg msg)=0;
};
//extern template Observer<meshMsg>;

/************************************************************************/
/* The most basic representation of a 2 simplex: the wireframe model,
/* described by vertices and triangles. Additionally normals and texture
/* coordinates can be stored on the mesh.
/************************************************************************/
class DECCORE_EXPORT wfMesh
{
protected:
	
	//the mesh
	vector<tuple3f> vertices;
	//index of the vertices of a face
	vector<tuple3i> faces; 
	
	
	//the normals by vertices
	vector<tuple3f> normals_per_vertex;
	//the indices of the normals at the vertex points of the faces
	vector<tuple3i> face_tex;
	 /* vector<tuple3i> face_normals_perVertex*/
	vector<tuple3f> tex;

	//Observer List
	//vector<Observer<meshMsg> *> observer; //leads to stupid linker problems...
	vector<meshObserver *> observer;

public:


	//dummy constructos
	wfMesh(void);
	//constructors
	wfMesh(const char* file);
	
	~wfMesh(void);

	void reset(vector<tuple3f> & _vertices, vector<tuple3i> & _faces);
	void initNormalsFromVertices();

	//////////////////////////////////////////////////////////////////////////
	//simple operations on the vertices
	//triggers POS_CHANGED message
	//////////////////////////////////////////////////////////////////////////
	void addOrthogonalNoise( float max );
	void addUniformNoise(float sigma);
	void scaleVertices(float scale);
	void translateVertices( float dx,float dy,float dz );
	void translateVertices( std::vector<tuple3f> & translation, float scale );
	//////////////////////////////////////////////////////////////////////////
	// This will scale and translate the mesh such that 
	// it lies in the [-1,1]^3 cube
	//////////////////////////////////////////////////////////////////////////
	void normalize(void);



	vector<tuple3f>& getVertices(void);
	vector<tuple3i>& getFaces();
//	vector<tuple3i>& getFaces2Normals();
	//vector<tuple3f> & getFaceNormals();
	vector<tuple3f> & getNormals();
	vector<tuple3f> & getTexCoords();
	
	void setVertices( vector<tuple3f> & verts );
	void setVertices(floatVector & x, floatVector & y, floatVector & z )
	{
		unsigned int sz = x.size();
		if(vertices.size() != sz){
			vertices.resize(x.size(), tuple3f());
		}
		for(unsigned int i = 0; i < sz; i++){
			vertices[i].set(x[i],y[i],z[i]);
		}
	}
	void setFaces( vector<tuple3i> & facs );
	void setNormals( vector<tuple3i>&  v_per_f_normals, vector<tuple3f> & normls );

	void setTexture( vector<tuple3i> & tex_per_f_per_v, vector<tuple3f> & tex );
	void setTextures_perVertex( double * x, double * y );
	//assumes one array of size 2*#vertices, where the first half
	//is given by x and the second half by y coordinates.
	void setTextures_perVertex( double * xy);
	void setTextures_perVertex( float * xy);
	//textures stored in tuple3f.x, tuple3f.y. The value in tuple3f.z is ignored.
	void setTextures_perVertex( vector<tuple3f> & textures );

	// returns face number hit. The ray is described  by start and to, which are supposed 
	// to already be in object coordinates.
	tuple3i * intersect( tuple3f & start,tuple3f &to, int * closestVertex,
		int * face, tuple3f & intersectPosition);

	//Observer methods
// 	void attach(Observer<meshMsg> * o);
// 	void detatch( Observer<meshMsg> * obs );
// 	void updateObserver(meshMsg msg);
	void attach(meshObserver * o);
	void detatch(meshObserver * obs );
	void updateObserver(meshMsg msg);
};
