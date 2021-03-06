#pragma once
#include "tuple3.h"
#include "DECCORE_EXPORT.h"


class DECCORE_EXPORT wingedEdge
{
public:
	//start and end vertex
	tuple2i v_a_b;
	//right (previous) and left (next) face
	tuple2i fc_p_n;

	wingedEdge * preva, * nexta;
	wingedEdge * prevb, * nextb;

	int myIndex;

	wingedEdge(void);
	wingedEdge (int start, int end);
	~wingedEdge(void);

	//////////////////////////////////////////////////////////////////////////
	//start and end vertex
	//////////////////////////////////////////////////////////////////////////
	int start();
	int end();

	//////////////////////////////////////////////////////////////////////////
	//getters amd setters
	//////////////////////////////////////////////////////////////////////////
	void set( int start, int stop );
	void setNext( wingedEdge * e );
	void setPrev( wingedEdge * e );
	void setLeftFace(int fc);
	void setRightFace(int fc);
	void setIndex(int ind);
	int getIndex();

	//////////////////////////////////////////////////////////////////////////
	// for winged meshes with borders this border compatible 
	// version of getNext/Prev will return the previous and next edge also
	// for a boundary edge, returning the other boundary edge
	//////////////////////////////////////////////////////////////////////////
	wingedEdge & getNext_bc( int vertex );
	wingedEdge & getPrev_bc( int vertex );

	//////////////////////////////////////////////////////////////////////////
	//method to iterate around the edges incident to the vertex
	//returns NULL if there is no next / previous edge
	//////////////////////////////////////////////////////////////////////////
	wingedEdge * getNext( int vertex );
	wingedEdge * getPrev( int vertex );

	//////////////////////////////////////////////////////////////////////////
	//orientation of the vertex j in the edge. Returns -1,1, or 0
	//////////////////////////////////////////////////////////////////////////
	int orientation( int j );

	//////////////////////////////////////////////////////////////////////////
	//get the internal simple edge tuple presentation 
	//////////////////////////////////////////////////////////////////////////
	tuple2i & getTuple();
	//////////////////////////////////////////////////////////////////////////
	// get the adjacent faces
	//////////////////////////////////////////////////////////////////////////
	tuple2i & getAdjFaces();
	int getLeftFace();
	int getRightFace();
	//returns what would be the left face if vertex was the start vertex of
	// the edge
	int getLeftFace( int vertex );

	//////////////////////////////////////////////////////////////////////////
	//returns the incident vertex that is not 'vertex'
	// if vertex is not contained -1 is returned.
	//////////////////////////////////////////////////////////////////////////
	int otherVertex( int vertex );
	//////////////////////////////////////////////////////////////////////////
	// returns -1 or the vertex shared by the two edges
	//////////////////////////////////////////////////////////////////////////
	int and( wingedEdge & other );

	//////////////////////////////////////////////////////////////////////////
	//methods specialized for boundary edges
	//returns the next edge on the boundary this edge belongs to (where next is
	//consistent with the manifold induced boundary orientation)
	//returns NULL if there is no next / previous edge or the actual edge is
	//not on a boundary
	//////////////////////////////////////////////////////////////////////////
	wingedEdge * nextBorderEdge();
	wingedEdge * previousBorderEdge();
	
	//////////////////////////////////////////////////////////////////////////
	//If this edge is a boundary edge, this method returns the edge vertex
	//which occurs earlier on the edge, according to the manifold induced
	//boundary orientation
	//////////////////////////////////////////////////////////////////////////
	int getFirstBoundaryVertex();

	//////////////////////////////////////////////////////////////////////////
	// border Test method: true if the edge is
	// a border edge
	//////////////////////////////////////////////////////////////////////////
	bool isOnBorder();

	//true if the edge is a border edge and the first edge in the
	//1-neighborhood of vertex
	bool isFirstEdge(int vertex);

	//true if the edge is a border edge and the last edge in the
	//1-neighborhood of vertex
	bool isLastEdge(int vertex);

	int commonFace( wingedEdge & other );
	int otherFace( int face );

	bool operator < (const wingedEdge & other) const;
	bool operator == (const wingedEdge & other) const;
	bool operator != (const wingedEdge & other) const;

	bool hasPrevEdge(int vertex);
	bool hasNextEdge(int vertex);
	bool contains( int start, int end );
	int getBoundaryFace();
	
private:

};

