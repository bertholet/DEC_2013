#include "glDisplayableMesh.h"
#include "glDebuggingStuff.h"
#include <QGLContext>

glDisplayableMesh::glDisplayableMesh(wfMesh * mesh):glDisplayableIntersectable(mesh),
m_vertexBuffer( QGLBuffer::VertexBuffer ), m_IndexBuffer(QGLBuffer::IndexBuffer), 
	m_normalBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer),
	m_texBuffer(QGLBuffer::VertexBuffer)
{
	myMesh = mesh;
	mesh->attach(this);
}


glDisplayableMesh::~glDisplayableMesh(void)
{
	myMesh->detatch(this);
	/*m_vertexBuffer.release();
	m_colorBuffer.release();
	m_IndexBuffer.release();
	m_normalBuffer.release();*/
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
	setUpBuffer("tex", m_texBuffer, myMesh->getTexCoords(), QGLBuffer::StaticDraw);

	setUpIndexBuffer(m_IndexBuffer,myMesh->getFaces(),QGLBuffer::StaticDraw);

	this->sendColorMap(constColor(*myMesh, tuple3f(0.8,0.3,0.3)));

	// Prepare a complete shader program...
	/*if ( !prepareShaderProgram( "./flat.vert", "./flat.frag", "./flat.geo" ) ){
		glDebuggingStuff::didIDoWrong();
		return;
	}*/

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

	m_texBuffer.bind();
	m_shader.setAttributeBuffer( "tex", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray("tex");


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
	m_shader.disableAttributeArray("tex");

	glDebuggingStuff::didIDoWrong();
}


void glDisplayableMesh::update( void * src, meshMsg msg )
{
	if(msg == POS_CHANGED && src == myMesh){
		if(m_vertexBuffer.usagePattern() != QGLBuffer::DynamicDraw){
			m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		}

		m_vertexBuffer.bind();
		glDebuggingStuff::didIDoWrong();
		m_vertexBuffer.write(0,& (myMesh->getVertices()[0].x), 3 * myMesh->getVertices().size()* sizeof(float));
		glDebuggingStuff::didIDoWrong();

		updateObserver(REFRESH_DISPLAY);
	}
	else if(msg == TEX_CHANGED && src == myMesh){
		if(m_texBuffer.usagePattern() != QGLBuffer::DynamicDraw){
			m_texBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		}

		m_texBuffer.bind();
		glDebuggingStuff::didIDoWrong();
		m_texBuffer.write(0,& (myMesh->getTexCoords()[0].x), 3 * myMesh->getTexCoords().size()* sizeof(float));
		glDebuggingStuff::didIDoWrong();
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

wfMesh * glDisplayableMesh::getWfMesh()
{
	return myMesh;
}

std::string glDisplayableMesh::colorBufferName()
{
	return "color";
}

void glDisplayableMesh::switchStyle( SHADER what )
{
	if(what== PHONG){
		prepareShaderProgram("./phong.vert", "./phong.frag", "./phong.geo");
	}
	else if(what == FLAT){
		prepareShaderProgram("./flat.vert", "./flat.frag", "./flat.geo");
	}
	else if(what == LINES){
		prepareShaderProgram("./lines.vert", "./lines.frag", "./lines.geo");
	}
	else if(what == TEXTURE){
		prepareShaderProgram("./tex.vert", "./tex.frag", "./tex.geo");
	}
}
