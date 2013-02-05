#pragma once
#include "glDisplayable.h"
#include "mouseStrokeListener.h"
#include "Resetable.h"

class glVectorfield:public glDisplayable, public mouseStrokeProcessor, public Resizable
{
public:
	glVectorfield();
	virtual ~glVectorfield(void);

	virtual void sendToGPU();

	virtual void draw( QMatrix4x4 & cam2View,QVector3D & eye );

	virtual void update( void * src, meshMsg msg );
	void setLength( float l );
	void setColor( QVector3D & col );

	virtual void process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );
	virtual void processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos );
	void updateBuffersOnGPU();
	///void updateDirectionsOnGPU();
	void display( std::vector<tuple3f>* pos, std::vector<tuple3f>* dir );
private:
	wfMesh * myMesh;
	vector<tuple3f>*  positions;
	vector<tuple3f> * directions;
	vector<tuple3f> dummy;
	QGLBuffer m_position, m_direction;//, m_normal;
	int m_bufferSize;//the buggers have dynamic length
	float length;
	QVector3D myColor;
};

