#pragma once
#include <QMouseEvent>
#include <QGLWidget>
#include "tuple3.h"
#include "Resetable.h"
#include "colorMap.h"

class mouseStrokeable
{
public:
	virtual void getProjectionMatrix( GLdouble proj[16] ) = 0;
	virtual void getModelViewMatrix( GLdouble model[16] ) = 0;
	virtual void getViewport( GLint viewPort[4] ) =0;
	virtual tuple3i * intersect(tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position)=0;
};

class mouseStrokeProcessor
{
public:
	virtual void process(int intersec_vertex, int intersec_face, tuple3f & intersec_pos)=0;
	virtual void processOnPress(int intersec_vertex, int intersec_face, tuple3f & intersec_pos)=0;
};


class mouseStrokeListener
{
private:
	mouseStrokeable * strokedObject;
	std::vector<mouseStrokeProcessor *> strokeProcessor;

	GLdouble proj[16], model[16];
	GLint viewPort[4];
	GLdouble p1x,p1y,p1z;
	tuple3f start, stop, intersec_pos;
	int intersec_face, intersec_vertex;


	bool resolve(QMouseEvent* event);
public:
	mouseStrokeListener(void);
	~mouseStrokeListener(void);

	void setStrokedObject(mouseStrokeable & mouseStroked);
	void addStrokeProcessor(mouseStrokeProcessor & processor);
	void removeStrokeProcessor(mouseStrokeProcessor & processor);

	void onMouseMove(QMouseEvent* event );
	void onMousePress(QMouseEvent* event );
};


class directionCollector:public mouseStrokeProcessor, public Resetable
{
private:
	tuple3f last_pos;
	std::vector<int> faces;
	std::vector<tuple3f> directions;
	std::vector<tuple3f> positions;
	bool isActive;
public:
	directionCollector();
	
	virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	virtual void processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	void reset();

	void setActive(bool what);
	std::vector<tuple3f>* getPositions();
	std::vector<tuple3f>* getDirections();
};

class vertexCollector: public mouseStrokeProcessor, public Resetable
{

private:
	std::vector<int> vertices;
	bool isActive;
	markupMap * myMap;
	tuple3f col;
public:
	vertexCollector();

	virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	virtual void processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );

	void reset();
	void mapTo( markupMap * map, tuple3f color);

	void setActive(bool what);
};
