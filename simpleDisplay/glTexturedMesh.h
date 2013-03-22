#pragma once
#include "glDisplayable.h"

class glPhongMesh:public glDisplayableIntersectable
{
public:
	glPhongMesh(wfMesh * aMesh);
	virtual ~glPhongMesh(void);

	virtual tuple3i * intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position );

	virtual void sendToGPU();

	virtual void draw( QMatrix4x4 & cam2View,QVector3D & eye );

	virtual void update( void * src, meshMsg msg );

	virtual void sendColorMap( colorMap &map );

	virtual wfMesh * getWfMesh();

private:
	QGLBuffer m_vertexBuffer, m_IndexBuffer, m_normalBuffer, m_colorBuffer, m_texBuffer;
	wfMesh * myMesh;
	wfMesh dummyMesh;
};

