#include "glPhongMesh.h"
#include "glDebuggingStuff.h"

glPhongMesh::glPhongMesh( wfMesh * aMesh ):glDisplayableIntersectable(aMesh),
	m_vertexBuffer( QGLBuffer::VertexBuffer ), m_IndexBuffer(QGLBuffer::IndexBuffer), 
	m_normalBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer)
{
	myMesh = aMesh;
	aMesh->attach(this);
}


glPhongMesh::~glPhongMesh(void)
{
	myMesh->detatch(this);
	/*m_vertexBuffer.release();
	m_colorBuffer.release();
	m_IndexBuffer.release();
	m_normalBuffer.release();*/

}

tuple3i * glPhongMesh::intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position )
{
	return myMesh->intersect(start,stop,closestVertex,face,*position);
}

void glPhongMesh::sendToGPU()
{
	if(myMesh->getVertices().size() == 0){
		return;
	}

	// Prepare a complete shader program...
	if ( !prepareShaderProgram( "./phong.vert", "./phong.frag", "./phong.geo") ){
	//if ( !prepareShaderProgram( "./flat.vert", "./flat.frag", "./flat.geo" ) ){
		glDebuggingStuff::didIDoWrong();
		return;
	}

	// Set up buffers
	setUpBuffer("vertex", m_vertexBuffer, myMesh->getVertices(),QGLBuffer::StaticDraw);
	setUpBuffer("normal", m_normalBuffer, myMesh->getNormals(), QGLBuffer::StaticDraw);

	setUpIndexBuffer(m_IndexBuffer,myMesh->getFaces(),QGLBuffer::StaticDraw);

	this->sendColorMap(constColor(*myMesh, tuple3f(0.8,0.3,0.3)));

	// Set up additional uniforms
	setUniformValue("light_pos", QVector3D(0,-4,4));
	setUniformValue("light", QVector3D(1,1,1));
}

void glPhongMesh::draw( QMatrix4x4 & cam2View,QVector3D & eye )
{
	if(myMesh->getVertices().size() == 0){
		return;
	}
	setUniformValue("MVmat", cam2View * getModel2world());
	setUniformValue("model2World", getModel2world());
	setUniformValue("normalMat", getNormalMatrix());
	setUniformValue("eye", eye);
	setUniformValue("light_pos", QVector3D(0,-4,4));
	setUniformValue("light", QVector3D(1,1,1));

	m_vertexBuffer.bind();
	m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray("vertex");

	m_normalBuffer.bind();
	m_shader.setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray("normal");

	m_colorBuffer.bind();
	m_shader.setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray("color");


	if(!m_vertexBuffer.bind()){
		qWarning() << "Could not bind vertex buffer to the context";
	}

	//m_colorBuffer.bind();
	if(!m_IndexBuffer.bind()){
		qWarning() << "Could not bind vertex buffer to the context";
	}

	glDrawElements(GL_TRIANGLES, 3*myMesh->getFaces().size(),GL_UNSIGNED_INT,0);


	m_shader.disableAttributeArray("vertex");
	m_shader.disableAttributeArray("normal");
	m_shader.disableAttributeArray("color");

	glDebuggingStuff::didIDoWrong();
}

void glPhongMesh::update( void * src, meshMsg msg )
{
	if(msg == POS_CHANGED && src == myMesh){
		if(m_vertexBuffer.usagePattern() != QGLBuffer::DynamicDraw){
			m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		}
		m_vertexBuffer.write(0,& (myMesh->getVertices()[0].x), 3 * myMesh->getVertices().size()* sizeof(float));

		updateObserver(REFRESH_DISPLAY);
	}

	else if(msg == MESH_DELETED && src == myMesh){
		myMesh = & dummyMesh;
		dummyMesh.attach(this);
	}
}

void glPhongMesh::sendColorMap( colorMap &map )
{
	setUpBuffer("color", m_colorBuffer,map.getColors(),QGLBuffer::DynamicDraw);
}

wfMesh * glPhongMesh::getWfMesh()
{
	return myMesh;
}
