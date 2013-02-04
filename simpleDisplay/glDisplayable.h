#pragma once
#include "wfmesh.h"
#include <QGLWidget>

#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include "Observer.h"

#include "colorMap.h"


class glDisplayable: 
	public meshObserver
{
public:

	enum glDispMessage {REFRESH_DISPLAY};
	//constructor
	glDisplayable();
	virtual ~glDisplayable(void);

	bool linkedAndReady();
	virtual void sendToGPU() = 0;
	virtual void draw(QMatrix4x4 & cam2View,QVector3D & eye)=0;

	void set(QMatrix4x4 & m2w, QMatrix4x4 &normalMat);
	QMatrix4x4 & getModel2world();
	QMatrix4x4 & getNormalMatrix();
	void rot( float angle, float axisX, float axisY,float axisZ );

	//void sendColorMap(colorMap &map);


	virtual void update(void * src, meshMsg msg) = 0;

	void attach(Observer<glDispMessage> * o);
	void detatch( Observer<glDispMessage> * obs );
	void updateObserver(glDispMessage msg);

private:
	QMatrix4x4 model2world, normalMatrix;
	std::vector<Observer<glDispMessage> * > observer;

	
protected:
	QGLShaderProgram m_shader; //actually part of a vao, but thats hidden.

	bool prepareShaderProgram( const QString & vspath, const QString & fspath , const QString & gspath = "");
	
	void setUniformValue(const char *name, QMatrix4x4 & mat);
	void setUniformValue(const char *name, QVector3D & vec);
	void setUniformValue(const char *name, float val);
	void enableBuffer(const char *name, QGLBuffer & buffer);
	void disableBuffer(const char * name);
	void setUpBuffer(const char *name, QGLBuffer & buffer, std::vector<tuple3f> & values, QGLBuffer::UsagePattern type);
	void setUpIndexBuffer(QGLBuffer & buffer, std::vector<tuple3i> & values, QGLBuffer::UsagePattern type);

};

class glDisplayableIntersectable:public glDisplayable
{
public:
	glDisplayableIntersectable(wfMesh * bla){}
	virtual ~glDisplayableIntersectable(){}
	virtual void sendColorMap( colorMap &map )=0;
	virtual tuple3i * intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position )=0;
	virtual wfMesh * getWfMesh() = 0;
};