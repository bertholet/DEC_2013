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
	glDisplayable(wfMesh * mesh);
	virtual ~glDisplayable(void);

	virtual void sendToGPU() = 0;
	virtual void draw(QMatrix4x4 & cam2View,QVector3D & eye)=0;

	QMatrix4x4 & getModel2world();
	QMatrix4x4 & getNormalMatrix();
	void rot( float angle, float axisX, float axisY,float axisZ );

	void sendColorMap(colorMap &map);


	virtual void update(void * src, meshMsg msg) = 0;

	void attach(Observer<glDispMessage> * o);
	void detatch( Observer<glDispMessage> * obs );
	void updateObserver(glDispMessage msg);

private:
	QMatrix4x4 model2world, normalMatrix;
	std::vector<Observer<glDispMessage> * > observer;

	QGLShaderProgram m_shader; //actually part of a vao, but thats hidden.

protected:
	bool prepareShaderProgram( const QString & vspath, const QString & fspath , const QString & gspath = "");
	
	void setUniformValue(const char *name, QMatrix4x4 & mat);
	void setUniformValue(const char *name, QVector3D & vec);
	void setUpBuffer(const char *name, QGLBuffer & buffer, std::vector<tuple3f> & values, QGLBuffer::UsagePattern type);
	void setUpIndexBuffer(QGLBuffer & buffer, std::vector<tuple3i> & values, QGLBuffer::UsagePattern type);
	
};

