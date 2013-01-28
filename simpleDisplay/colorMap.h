#pragma once
#include <vector>
#include "wingedMesh.h"
#include "mouseStrokeListener.h"

class colorMap
{
public:
	colorMap(void);
	virtual ~colorMap(void);

	virtual std::vector<tuple3f> & getColors() = 0;
};

class constColor:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	constColor(wfMesh & m, tuple3f color);

	virtual std::vector<tuple3f> & getColors();
};

class borderMap:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	borderMap(wingedMesh & m);
	~borderMap();

	virtual std::vector<tuple3f> & getColors();
};

class angleMap:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	angleMap(wingedMesh & m);
	~angleMap();

	virtual std::vector<tuple3f> & getColors();
};


class strokeMap:public colorMap,public mouseStrokeProcessor
{
private:
	tuple3f basecolor;
	std::vector<tuple3f> cols;
public:
	strokeMap();
	~strokeMap();
	void associateTo(wfMesh & m_);
	void reset();
	virtual std::vector<tuple3f> & getColors();
	virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

};
/*class curvatureMap:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	curvatureMap(wingedMesh & m);
	~curvatureMap();

	virtual std::vector<tuple3f> & getColors();
};*/