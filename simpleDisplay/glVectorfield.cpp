#include "glVectorfield.h"
#include "glDebuggingStuff.h"
#include "MODEL.h"

glVectorfield::glVectorfield():
	m_position(QGLBuffer::VertexBuffer), m_direction(QGLBuffer::VertexBuffer)
//	m_normal(QGLBuffer::VertexBuffer)
{
	dummy.resize(1);
	positions = &dummy;
	directions = &dummy;
	length =1;
	myColor = QVector3D(1,0,0);
}


glVectorfield::~glVectorfield(void)
{
}

void glVectorfield::sendToGPU()
{
	if(!linkedAndReady()){
		if(!prepareShaderProgram("./vfield.vert","./vfield.frag", "./vfield.geo")){
			qWarning()<< " Shader link error ";
			return;
		}
	}
/*	if(m_position.isCreated()){
		m_position.destroy();
	}
	if(m_direction.isCreated()){
		m_direction.destroy();
	}*/
	m_bufferSize = max(100, positions->size());
	std::vector<tuple3f> bla(m_bufferSize);
	//setUpBuffer("position",m_position,bla,QGLBuffer::DynamicDraw);
	
	//setUpBuffer("direction", m_direction, bla/**directions*/,QGLBuffer::DynamicDraw);
	setUpBuffer("position",m_position,bla,QGLBuffer::DynamicDraw);
	setUpBuffer("direction", m_direction, bla,QGLBuffer::DynamicDraw);

	updateBuffersOnGPU();


	
}

void glVectorfield::draw( QMatrix4x4 & cam2View,QVector3D & eye )
{
	glDebuggingStuff::didIDoWrong();
	setUniformValue("MVmat", cam2View * getModel2world());
	setUniformValue("normalMat", getNormalMatrix());
	setUniformValue("color", myColor);
	setUniformValue("len", length);
	setUniformValue("eye", eye);

	enableBuffer("position", m_position);
	//enableBuffer("normal", m_normal);
	enableBuffer("direction", m_direction);
	/*m_position.bind();
	m_shader.setAttributeArray("position", GL_FLOAT,0,3);
	m_shader.enableAttributeArray("position");

	m_normal.bind();
	m_shader.setAttributeArray("normal", GL_FLOAT,0,3);
	m_shader.enableAttributeArray("normal");

	m_direction.bind();
	m_shader.setAttributeArray("direction", GL_FLOAT,0,3);
	m_shader.enableAttributeArray("direction");*/

	if(!m_position.bind()){
		qWarning() << "Could not bind vertex buffer to the context";
	}
		glDebuggingStuff::didIDoWrong();

	//glDrawElements(GL_POINTS, 5*(1+positions->size()),GL_UNSIGNED_INT,0);   
	glDrawArrays(GL_POINTS,0,(positions->size()));

	glDebuggingStuff::didIDoWrong();

	disableBuffer("position");
	//disableBuffer("normal");
	disableBuffer("direction");

}

void glVectorfield::update( void * src, meshMsg msg )
{
	throw std::exception("The method or operation is not implemented.");
}

void glVectorfield::setLength( float l )
{
	length = l;
}

void glVectorfield::setColor( QVector3D & col )
{
	myColor = col;
}

void glVectorfield::process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	//sendToGPU();
	updateBuffersOnGPU();
}

void glVectorfield::processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	
}

void glVectorfield::updateBuffersOnGPU()
{
	if(positions->size() > m_bufferSize){
		m_position.destroy();
		m_direction.destroy();
		std::vector<tuple3f> bla(positions->size() + 100);
		m_bufferSize = positions->size() + 100;
		setUpBuffer("position",m_position,bla,QGLBuffer::DynamicDraw);
		setUpBuffer("direction",m_direction,bla,QGLBuffer::DynamicDraw);
	}
	if(positions->size()!=0){
		m_position.bind();
		m_position.write(0, & (*positions)[0].x, 3*positions->size()*sizeof(float));
		m_direction.bind();
		m_direction.write(0, & (*directions)[0].x, 3*positions->size()*sizeof(float));
	}
}


void glVectorfield::display( std::vector<tuple3f>* pos, std::vector<tuple3f>* dir )
{
	positions = pos;
	directions = dir;

	if(linkedAndReady()){
		updateBuffersOnGPU();
	}
}
