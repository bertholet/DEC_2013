#include "glDisplayable.h"
#include "OBIFileReader.h"
#include <QGLFormat>
#include <QGLWidget>
#include <QTOpenGL>
#include <GL/GL.h>
#include "glDebuggingStuff.h"
#include <algorithm>


glDisplayable::glDisplayable()//:
// m_vertexBuffer( QGLBuffer::VertexBuffer ), m_IndexBuffer(QGLBuffer::IndexBuffer), 
//	 m_normalBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer)
{
//	myMesh = mesh;
	model2world.setToIdentity();
	normalMatrix = model2world;
	
	//myMesh->normalize();

//	myMesh->attach(this);

}


glDisplayable::~glDisplayable(void)
{
	//myMesh->detatch(this);

	//m_shader.release();
	//m_shader.removeAllShaders();
}

/*void glDisplayable::sendToGPU()
{


	if(myMesh->getVertices().size() == 0){
		return;
	}

	// Prepare a complete shader program...
	if ( !prepareShaderProgram( "./flat.vert", "./flat.frag", "./flat.geo" ) ){
		glDebuggingStuff::didIDoWrong();
		return;
	}


	//---------------------------------------------
	m_vertexBuffer.create();
	m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );

	glDebuggingStuff::didIDoWrong();

	if ( !m_vertexBuffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	m_vertexBuffer.allocate( &(myMesh->getVertices()[0].x), 3 * myMesh->getVertices().size()* sizeof( float ) );

	glDebuggingStuff::didIDoWrong();


	//-------------------- vertex vao bound, add indices------------
	m_IndexBuffer.create();
	m_IndexBuffer.setUsagePattern( QGLBuffer::StaticDraw );

	glDebuggingStuff::didIDoWrong();

	if ( !m_IndexBuffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	m_IndexBuffer.allocate( &(myMesh->getFaces()[0].a), 3 * myMesh->getFaces().size()* sizeof( int ) );

	glDebuggingStuff::didIDoWrong();



	//---------------------------------------------
	m_normalBuffer.create();
	m_normalBuffer.setUsagePattern( QGLBuffer::StaticDraw );

	glDebuggingStuff::didIDoWrong();

	if ( !m_normalBuffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}

	m_normalBuffer.allocate( &(myMesh->getNormals()[0].x), 3 * myMesh->getNormals().size()* sizeof( float ) );


	glDebuggingStuff::didIDoWrong();



	// Bind the shader program so that we can associate variables from
	// our application to the shaders
	if ( !m_shader.bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}

	glDebuggingStuff::didIDoWrong();


	//bind this buffer to the context to relate it to the 'vertex' symbol.
	m_vertexBuffer.bind();
	m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray( "vertex" );
	glDebuggingStuff::didIDoWrong();

	m_normalBuffer.bind();
	m_shader.setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray( "normal" );

	glDebuggingStuff::didIDoWrong();

	this->sendColorMap(constColor(*myMesh, tuple3f(0.8,0.3,0.3)));
	//additional uniforms
	m_shader.setUniformValue("light_pos", QVector3D(0,-4,4));
	m_shader.setUniformValue("light", QVector3D(1,1,1));
	//m_shader.setUniformValue("color", QVector3D(0.8,0.3,0.3));



}*/

bool glDisplayable::prepareShaderProgram( const QString & vspath, const QString & fspath , const QString & gspath)
{
	// First we load and compile the vertex shader...
	m_shader.removeAllShaders();
	bool result = m_shader.addShaderFromSourceFile( QGLShader::Vertex, vspath );
	if ( !result )
		qWarning() << m_shader.log();

	// ...now the fragment shader...
	result = m_shader.addShaderFromSourceFile( QGLShader::Fragment, fspath );
	if ( !result )
		qWarning() << m_shader.log();

	if(gspath != ""){
		result = m_shader.addShaderFromSourceFile( QGLShader::Geometry, gspath );
		if ( !result )
			qWarning() << m_shader.log();
	}

	// ...and finally we link them to resolve any references.
	result = m_shader.link();
	if ( !result )
		qWarning() << "Could not link shader program:" << m_shader.log();

	if(!m_shader.bind()){
		qWarning() << "Could not bind shader program to the context";
	}

	glDebuggingStuff::didIDoWrong();
	return result;
}



/*void glDisplayable::draw(QMatrix4x4 & world2view, QVector3D & eye)
{
	if(myMesh->getVertices().size() == 0){
		return;
	}
	//m_IndexBuffer.bind();
	//m_vertexBuffer.bind();
	m_shader.setUniformValue("MVmat", world2view * model2world);
	m_shader.setUniformValue("model2World", model2world);
	m_shader.setUniformValue("normalMat", normalMatrix);
	m_shader.setUniformValue("eye", eye);
	m_vertexBuffer.bind();
	//m_colorBuffer.bind();
	m_IndexBuffer.bind();
	glDebuggingStuff::didIDoWrong();

	glDrawElements(GL_TRIANGLES,3*myMesh->getFaces().size(),GL_UNSIGNED_INT,0);
}*/

QMatrix4x4 & glDisplayable::getModel2world()
{
	return model2world;
}


QMatrix4x4 & glDisplayable::getNormalMatrix()
{
	return normalMatrix;
}


void glDisplayable::rot( float angle, float axisX, float axisY,float axisZ )
{
	QMatrix4x4 rot;
	rot.setToIdentity();
	rot.rotate(-angle*180.f/3.14159f,axisX,axisY,axisZ);
	model2world = rot * model2world;
	normalMatrix = model2world.inverted();
}

/*void glDisplayable::update( void * src, meshMsg msg )
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
}*/


/************************************************************************/
/* Attach an Observer
/* Observers will be notified about changes that need a display refresh
/************************************************************************/
void glDisplayable::attach( Observer<glDispMessage> * o )
{
	this->observer.push_back(o);
}
/************************************************************************/
/* Detatch an observer from this mesh                       */
/************************************************************************/
void glDisplayable::detatch( Observer<glDispMessage> * obs )
{
	vector<Observer<glDispMessage> * >::iterator it;
	it = find(observer.begin(), observer.end(), obs);
	if(it != observer.end()){
		this->observer.erase(it);
	}
	else{
		assert(false);
	}
}

void glDisplayable::updateObserver(glDispMessage msg )
{
	for(int i = 0; i < observer.size(); i++){
		observer[i]->update(this, msg);
	}
}

void glDisplayable::setUpBuffer( const char *name, QGLBuffer & buffer, std::vector<tuple3f> & values, QGLBuffer::UsagePattern type )
{
	if ( !m_shader.bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}
	glDebuggingStuff::didIDoWrong();

	if(!buffer.isCreated()){
		buffer.create();
		buffer.setUsagePattern( type );
		glDebuggingStuff::didIDoWrong();
	}


	if ( !buffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	buffer.allocate( &(values[0].x), 3 * values.size()* sizeof( float ) );

	glDebuggingStuff::didIDoWrong();

	m_shader.setAttributeBuffer( name, GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray( name);
	glDebuggingStuff::didIDoWrong();
	
}

void glDisplayable::setUpIndexBuffer(QGLBuffer & buffer, std::vector<tuple3i> & values, QGLBuffer::UsagePattern type )
{
	if ( !m_shader.bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}
	glDebuggingStuff::didIDoWrong();


	buffer.create();
	buffer.setUsagePattern( type);

	glDebuggingStuff::didIDoWrong();

	if ( !buffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	buffer.allocate( &(values[0].a), 3 * values.size()* sizeof( int ) );

	glDebuggingStuff::didIDoWrong();
}

void glDisplayable::setUniformValue( const char *name, QMatrix4x4 & mat )
{
	if(!m_shader.bind()){
		qWarning() << "Could not bind shader program to the context";
	}
	m_shader.setUniformValue(name, mat);
	glDebuggingStuff::didIDoWrong();
}

void glDisplayable::setUniformValue( const char *name, QVector3D & vec )
{
	if(!m_shader.bind()){
		qWarning() << "Could not bind shader program to the context";
	}
	m_shader.setUniformValue(name, vec);
	glDebuggingStuff::didIDoWrong();
}

void glDisplayable::setUniformValue( const char *name, float val )
{
	if(!m_shader.bind()){
		qWarning() << "Could not bind shader program to the context";
	}
	m_shader.setUniformValue(name, val);
	glDebuggingStuff::didIDoWrong();
}

void glDisplayable::enableBuffer( const char *name, QGLBuffer & buffer )
{
	if(!buffer.bind()){
		qWarning() << "*** Could not bind buffer";
	}
	m_shader.setAttributeArray(name, GL_FLOAT,0,3);
	m_shader.enableAttributeArray(name);
	glDebuggingStuff::didIDoWrong();

}

void glDisplayable::disableBuffer( const char * name )
{
	m_shader.disableAttributeArray(name);
	glDebuggingStuff::didIDoWrong();
}

bool glDisplayable::linkedAndReady()
{
	return m_shader.isLinked();
}

void glDisplayable::set( QMatrix4x4 & m2w, QMatrix4x4 &normalMat )
{
	model2world = m2w;
	normalMatrix = normalMat;
}

/*void glDisplayable::sendColorMap( colorMap &map )
{
	if(!m_colorBuffer.isCreated()){
		m_colorBuffer.create();
		m_colorBuffer.setUsagePattern( QGLBuffer::DynamicDraw );
		glDebuggingStuff::didIDoWrong();
	}


	if ( !m_shader.bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}
	if ( !m_colorBuffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	m_colorBuffer.allocate( &(map.getColors()[0].x), 3 * myMesh->getVertices().size()* sizeof( float ) );
	//m_colorBuffer.write(0,&(map.getColors()[0].x), 3 * myMesh->getVertices().size()* sizeof( float ) );

	m_shader.setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
	m_shader.enableAttributeArray( "color" );
	glDebuggingStuff::didIDoWrong();
	
}*/

/*tuple3i * glDisplayable::intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position )
{
	return myMesh->intersect(start,stop,closestVertex,face,*position);
}*/
