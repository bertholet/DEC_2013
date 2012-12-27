#pragma once

#include "DECCORE_EXPORT.h"
#include <vector>
#include "wfmesh.h"
#include "wingedEdge.h"
#include "Observer.h"

/************************************************************************/
/* A wingedEdges structure mesh that can be generated out of a simple 2D
/* Wireframe model (See wfmesh). This structure allows efficient iteration
/* over mesh neighborhoods. The prerequisit for this structure to work 
/* properly is that the input mesh
/* is an oriented, discrete 2-manifold mesh.
/*
/* No data kept in the wireframe mesh is duplicated, the wfmesh is wrapped 
/*by this structure. The wingedmesh object will free the wfmesh upon deletion.
/*
/* This structure keeps a reference to the input wfMesh and is invalidated 
/* if the mesh topology of the underlying wfmodel is changed, i.e the mesh
/* sends the CONNECTIVITY_CHANGED message. Validity can be checked with the
/* isValid method, and reinitialize can be called if needed.
/*
/*
/* Upon deletion the referenced wfModel is not freed.
/************************************************************************/

class DECCORE_EXPORT wingedMesh:Observer<meshMsg>
{
public:
	//dummy constructor
	wingedMesh();
	//standard constructor.
	wingedMesh(wfMesh *aMesh);
	~wingedMesh(void);

	//getter Methods
	wfMesh* getWfMesh();
	std::vector<tuple3f> & getVertices();
	std::vector<wingedEdge> & getEdges();
	std::vector<tuple3i> & getFaces();
	std::vector<std::vector<int>> & getv2e();
	std::vector<tuple3i> & getf2e();
	
	//////////////////////////////////////////////////////////////////////////
	// Resolve an edge to its index / list position.
	//////////////////////////////////////////////////////////////////////////
	int edgeIndex(wingedEdge & edge);
	////////////////////////////////////////////////////////////
	// Get one edge incident to vertex
	//////////////////////////////////////////////////////////////////////////
	wingedEdge & getAnEdge( int vertex );

	bool isValid();
	virtual void update( void * src, meshMsg msg );

private:
	//validity
	bool valid;
	//the basic vertex and face lists.
	wfMesh *myMesh;
	//winged edge stuff
	//vertex to incident edge
	std::vector<std::vector<int>> v2e;
	//face to bordering edge
	std::vector<tuple3i> f2e;
	//the winged edges
	std::vector<wingedEdge> edges;


	void initEdges(void);
};

