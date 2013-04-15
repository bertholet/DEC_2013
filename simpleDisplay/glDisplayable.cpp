#include "glDisplayable.h"
#include "OBIFileReader.h"
#include <QGLFormat>
#include <QGLWidget>
#include <QTOpenGL>
#include <GL/GL.h>
#include "glDebuggingStuff.h"
#include <algorithm>


glDisplayable::glDisplayable()
{

	model2world.setToIdentity();
	normalMatrix = model2world;
	
}


glDisplayable::~glDisplayable(void)
{
}


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

	//...optionally the geo shader...
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
		result = false;
	}

	glDebuggingStuff::didIDoWrong();
	return result;
}



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

void glDisplayable::setUpBuffer( const char *name, QGLBuffer & buffer, std::vector<tuple3f> & values, QGLBuffer::UsagePattern type, myVAO & vao )
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


	if(!vao.isCreated()){
		vao.create();
	}
	vao.bind();


	m_shader.setAttributeBuffer( name, GL_FLOAT, 0, 3 );
	glDebuggingStuff::didIDoWrong();
	m_shader.enableAttributeArray( name);
	glDebuggingStuff::didIDoWrong();

		//generate vao etc old style...
	/*typedef void (WINAPI *GL_GEN_VARRAYS)(GLsizei , GLuint *);
	typedef void (WINAPI *GL_BIND_VARRAY)(GLuint);
	GL_GEN_VARRAYS glGenVertexArrays = (GL_GEN_VARRAYS) wglGetProcAddress("glGenVertexArrays");
	GL_BIND_VARRAY glBindVertexArray = (GL_BIND_VARRAY) wglGetProcAddress("glBindVertexArray");
		uint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	glDebuggingStuff::didIDoWrong();
	int loc = m_shader.attributeLocation(name);
	if(loc < 0){
		qWarning()<< "*** Invalid attribute name";
	}
	glDebuggingStuff::didIDoWrong();
	m_shader.setAttributeBuffer(loc,GL_FLOAT,0,3);
	m_shader.enableAttributeArray( name);*/

	buffer.release();
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

glDisplayableIntersectable::glDisplayableIntersectable( wfMesh * bla )
{
	actualMap = NULL;
}
