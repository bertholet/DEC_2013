#pragma once
#include <math.h>
#include <iostream>
#include <assert.h>
#include "DECCORE_EXPORT.h"

//////////////////////////////////////////////////////////////////////////
//designed for float or double 3-tuples.
//////////////////////////////////////////////////////////////////////////
class DECCORE_EXPORT tuple3f
{
public:
	float x, y, z;
	tuple3f(void);
	tuple3f(float x_, float y_, float z_);
	~tuple3f(void);

	void set(float x_, float y_, float z_);
	void set(tuple3f & other);


//////////////////////////////////////////////////////////////////////////
//standard 3d vector ops.
//////////////////////////////////////////////////////////////////////////
	void normalize(void);
	tuple3f cross(tuple3f &b);
	float norm(void);
	float normSqr(void);
	float dot(tuple3f &other);

	tuple3f operator + (tuple3f &other);
	void operator += (tuple3f &other);
	void operator -= (tuple3f &other);
	void operator *= (float other);
	tuple3f operator * (float other);
	tuple3f operator - (tuple3f &other);
	tuple3f operator - (void);
	tuple3f operator - (float &other);
	void operator -= (float &other);
	float operator[](unsigned int i);

//////////////////////////////////////////////////////////////////////////
//Trigonometry
//////////////////////////////////////////////////////////////////////////

	//cot between the two vectors a, b
	static float cot(tuple3f &a, tuple3f &b);
	//(a-b) dot(c-b)
	static float a_bDotc_b( tuple3f &a, tuple3f &b , tuple3f & c );
	//sinus of the angle at b in the triangle a,b,c
	static float sinPoints(tuple3f &a, tuple3f &b, tuple3f &c);
	//cosinus of the angle at b in the triangle a,b,c
	static float cosPoints( tuple3f& a, tuple3f& b, tuple3f& c );
	//returns the acos of the cos at b, that is an angles [0,pi]
	static float angle(tuple3f& a, tuple3f& b,tuple3f& c);
	//cot at b of the triangle a, b,c
	static float cotPoints(tuple3f &a, tuple3f &b, tuple3f &c);	
	//cross product
	static tuple3f cross(tuple3f &a, tuple3f &b);
	//the norm of the cross product
	static float crossNorm(tuple3f &a, tuple3f &b);


};



//////////////////////////////////////////////////////////////////////////
// Tuplei is different from tuple, it has an orientation and contain method.
class DECCORE_EXPORT tuple2i
{
public:
	int a, b;
	tuple2i(void);
	tuple2i(int x_, int y_);
	~tuple2i(void){}

	void set(int x_, int y_);

	//////////////////////////////////////////////////////////////////////////
	//Returns +-1 or 0 if vertex is the start vertex -1, end +1 neither: 0
	//////////////////////////////////////////////////////////////////////////
	int orientation(int vertex);

	bool operator < (const tuple2i & other) const;
	bool operator == (const tuple2i & other) const;
};

class  DECCORE_EXPORT tuple3i
{
public:
	unsigned int a, b, c;
	tuple3i(void);
	tuple3i(int x_, int y_, int z_);
	~tuple3i(void);

	void set(int x_, int y_, int z_);

	bool contains(tuple2i & tuple);

	bool contains(int what);

	int orientation(tuple2i & edge);
//	int orientation(wingedEdge & edge);
};


