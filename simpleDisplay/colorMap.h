#pragma once
#include <vector>
#include "wingedMesh.h"

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

/*class curvatureMap:public colorMap{
private:
	std::vector<tuple3f> cols;
public:
	curvatureMap(wingedMesh & m);
	~curvatureMap();

	virtual std::vector<tuple3f> & getColors();
};*/