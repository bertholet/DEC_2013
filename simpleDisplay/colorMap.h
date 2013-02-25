#pragma once
#include <vector>
#include "wingedMesh.h"
//#include "mouseStrokeListener.h"
#include "Resetable.h"
#include "MODEL.h"

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


class markupMap:public colorMap, public Resetable
{
private:
	tuple3f basecolor;
	std::vector<tuple3f> cols;
public:
	markupMap();
	~markupMap();
	void associateTo(wfMesh & m_);
	void reset();
	void mark(int vertex, tuple3f &col);
	virtual std::vector<tuple3f> & getColors();
	//virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	//virtual void processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

};

class vorticityMap:public colorMap
{
private:
	std::vector<tuple3f> cols;
	float scale;
public:
	vorticityMap();
	~vorticityMap();

	void setScale(float scl);
	void update(std::vector<float> & values, MODEL & model);
	virtual std::vector<tuple3f> & getColors();
	//virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	//virtual void processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

};

/*class curvatureMap:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	curvatureMap(wingedMesh & m);
	~curvatureMap();

	virtual std::vector<tuple3f> & getColors();
};*/