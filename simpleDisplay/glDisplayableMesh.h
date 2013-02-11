#pragma once
#include "glDisplayable.h"
#include "wfmesh.h"
#include <QGLBuffer>
#include "colorMap.h"

class glDisplayableMesh:public glDisplayableIntersectable
{
public:
	glDisplayableMesh(wfMesh *);
	virtual ~glDisplayableMesh(void);

	void sendToGPU();
	void draw(QMatrix4x4 & cam2View,QVector3D & eye);
	void update( void * src, meshMsg msg );

	void sendColorMap( colorMap &map );
	tuple3i * intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position );

	virtual wfMesh * getWfMesh();

	void switchStyle(bool what){
		if(what){
			prepareShaderProgram("./phong.vert", "./phong.frag", "./phong.geo");
		}
		else{
			prepareShaderProgram("./flat.vert", "./flat.frag", "./flat.geo");
		}
	}

	virtual std::string colorBufferName();

private:
	QGLBuffer m_vertexBuffer, m_IndexBuffer, m_normalBuffer, m_colorBuffer;
	wfMesh * myMesh;
	wfMesh dummyMesh;
};

