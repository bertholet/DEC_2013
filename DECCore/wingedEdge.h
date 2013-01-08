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

		
	//start and end vertex
	int start();
	int end();

	//getters amd setters
	void set( int start, int stop );
	void setNext( wingedEdge * e );
	void setPrev( wingedEdge * e );
	void setIndex(int ind);
	int getIndex();

	// for winged meshes with borders this border
	//compatible version will return the previous and next edge also
	// for a boundary edge, returning the other boundary edge
	wingedEdge & getNext_bc( int vertex );
	wingedEdge & getPrev_bc( int vertex );

	//returns NULL if there is no next / previous edge
	//
	wingedEdge * getNext( int vertex );
	wingedEdge * getPrev( int vertex );

	//methods specialized for boundary edges
	//returns NULL if there is no next / previous edge
	wingedEdge * nextBorderEdge();
	int getFirstBoundaryVertex();


	void setLeftFace(int fc);
	void setRightFace(int fc);

	// border Test method: true if the edge is
	// a border edge
	bool isOnBorder();

	//true if the edge is a border edge and the first edge in the
	//1-neighborhood of vertex
	bool isFirstEdge(int vertex);

	//true if the edge is a border edge and the last edge in the
	//1-neighborhood of vertex
	bool isLastEdge(int vertex);

	//orientation of the vertex j in the edge: -1,1, or 0
	int orientation( int j );

	//internal simple edge presentation as tuple
	tuple2i & getTuple();

	//returns the incident vertex that is not 'vertex'
	// if vertex is not contained -1 is returned.
	int otherVertex( int vertex );

	bool operator < (const wingedEdge & other) const;
	bool operator == (const wingedEdge & other) const;
	bool operator != (const wingedEdge & other) const;

private:

};

