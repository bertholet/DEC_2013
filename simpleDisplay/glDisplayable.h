#pragma once
#include "wfmesh.h"
#include <QGLWidget>

#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include "Observer.h"


//extern template Observer<meshMsg>;

class glDisplayable: 
	meshObserver
{
public:

	enum glDispMessage {REFRESH_DISPLAY};
	//constructor
	glDisplayable(wfMesh * mesh);
	~glDisplayable(void);

	void sendToGPU();
	void draw(QMatrix4x4 & cam2View,QVector3D & eye);
	QMatrix4x4 & getModel2world();
	void rot( float angle, float axisX, float axisY,float axisZ );


	virtual void update(void * src, meshMsg msg);

	void attach(Observer<glDispMessage> * o);
	void detatch( Observer<glDispMessage> * obs );
	void updateObserver(glDispMessage msg);

private:
	wfMesh * myMesh;
	wfMesh dummyMesh;
	bool vaoLoaded, vboLoaded;
	GLuint vertexshader, fragmentshader,shaderprogram;
	GLuint vao, vbo[2];
	QMatrix4x4 model2world, normalMatrix;

	QGLShaderProgram m_shader; //actually part of a vao, but thats hidden.
	QGLBuffer m_vertexBuffer, m_IndexBuffer, m_normalBuffer, m_normalIndexBuffer;
	std::vector<Observer<glDispMessage> * > observer;
	bool prepareShaderProgram( const QString & vspath, const QString & fspath , const QString & gspath = "");
};

