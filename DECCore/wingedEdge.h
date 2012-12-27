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

	wingedEdge(void);
	wingedEdge (int start, int end);
	~wingedEdge(void);

	bool operator < (const wingedEdge & other) const;
	bool operator == (const wingedEdge & other) const;
	bool operator != (const wingedEdge & other) const;
	void set( int start, int stop );
	int start();
	int end();
	void setNext( wingedEdge * e );
	void setPrev( wingedEdge * e );
	wingedEdge & getNext( int vertex );
	wingedEdge & getPrev( int vertex );
	int orientation( int j );
	tuple2i & getTuple();

	//returns the incident vertex that is not 'vertex'
	// if vertex is not contained -1 is returned.
	int otherVertex( int vertex );
};

