#include "glDisplayableMesh.h"
#include "glDebuggingStuff.h"

glDisplayableMesh::glDisplayableMesh(wfMesh * mesh):glDisplayable(mesh),
m_vertexBuffer( QGLBuffer::VertexBuffer ), m_IndexBuffer(QGLBuffer::IndexBuffer), 
	m_normalBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer)
{
	myMesh = mesh;
	mesh->attach(this);
}


glDisplayableMesh::~glDisplayableMesh(void)
{
	myMesh->detatch(this);
}

tuple3i * glDisplayableMesh::intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position )
{
	return myMesh->intersect(start,stop,closestVertex,face,*position);
}

void glDisplayableMesh::sendToGPU()
{
	if(myMesh->getVertices().size() == 0){
		return;
	}

	// Prepare a complete shader program...
	if ( !prepareShaderProgram( "./flat.vert", "./flat.frag", "./flat.geo" ) ){
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

void glDisplayableMesh::draw( QMatrix4x4 & world2view, QVector3D & eye)
{
	if(myMesh->getVertices().size() == 0){
		return;
	}
	setUniformValue("MVmat", world2view * getModel2world());
	setUniformValue("model2World", getModel2world());
	setUniformValue("normalMat", getNormalMatrix());
	setUniformValue("eye", eye);


	m_vertexBuffer.bind();
	//m_colorBuffer.bind();
	m_IndexBuffer.bind();
	glDebuggingStuff::didIDoWrong();

	glDrawElements(GL_TRIANGLES,3*myMesh->getFaces().size(),GL_UNSIGNED_INT,0);
}


void glDisplayableMesh::update( void * src, meshMsg msg )
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


void glDisplayableMesh::sendColorMap( colorMap &map )
{
	setUpBuffer("color", m_colorBuffer,map.getColors(),QGLBuffer::DynamicDraw);

}
