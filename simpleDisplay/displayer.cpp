#include "displayer.h"
//#include "mesh.h"
#include <math.h>
#include <QtGui/QMouseEvent>
#include "glDebuggingStuff.h"
#include "MODEL.h"
//#include "fluidSimulation.h"

Displayer::Displayer(QGLFormat & format, QWidget *parent)
	: QGLWidget(format,parent), tBallListener(this)
{

	this->setMinimumHeight(300);
	this->setMinimumWidth(300);
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	myDisplayable = new glDisplayable(MODEL::getModel()->getMesh()->getWfMesh());
	myDisplayable->attach(this);

	//	mode = EDGEMODE;
	mouseMode = TRACKBALLMODE;

	eye = QVector3D(0, 0 ,4);
	up = QVector3D(0,1,0);
	camMatrix.lookAt(eye, QVector3D(0,0,0), up );
	projMatrix.perspective(60, this->width()/this->height(),1,500);

//	this->tex = new squareTexture();

	//this->map = NULL;
	//this->tmmap = new triangleMarkupMap();


	//Model::getModel()->attach(this);

	displayVField = true;
	normedVField = true;
	displayVectors = true;
	displayPointCloud = true;


	
	//strokeListener = new mouseStrokeListener(tmmap, this);
}

Displayer::~Displayer()
{
	delete myDisplayable;
	/*if(this->map != NULL){
		delete map;
	}
	delete tBallListener;
	delete strokeListener;
	delete tmmap;
	delete tex;*/
}

void Displayer::display( wfMesh * aMesh )
{
	delete myDisplayable;
	myDisplayable = new glDisplayable(aMesh);
	myDisplayable->sendToGPU();
	myDisplayable->attach(this);
	
}

void Displayer::initializeGL()
{
	QGLFormat glFormat = QGLWidget::format();
	if ( !glFormat.sampleBuffers() )
		qWarning() << "Could not enable sample buffers";


	glClearColor( 1, 1, 1, 1.0f );

	glViewport(0,0,this->width(),this->height());

	glDebuggingStuff::didIDoWrong();

	myDisplayable->sendToGPU();

	glDebuggingStuff::didIDoWrong();

	//modelViewMatrix = projMatrix*camMatrix * myDisplayable.getModel2world();

	glEnable(GL_DEPTH_TEST);

/*	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//antialias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_POLYGON_SMOOTH);
//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);


	//bg color
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//////////////////////////////////////////////////////////////////////////
	// textures for fluid sim
	//////////////////////////////////////////////////////////////////////////
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_1D, tex_id);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	GLfloat Texture4[4][3] =
	{

	//	{ 0.f, 0.f, 0.f },
		{ 0.1f, 0.1f, 0.1f },
		{ 0.8f,0.8f,0.8f }, // Blue
		{ 0.8f, 0.8f, 0.8f }, // Green
		{ 0.1f, 0.1f, 0.1f },
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f },
		{ 1, 1, 1 }, // Blue
		{ 1, 1, 1 }, // Green
		{ 0.f, 0.f, 0.f },
		{ 0.f, 0.f, 0.f }
	};


	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4 ,0,GL_RGB, GL_FLOAT, 
		Texture4);

	//Load 2dTexture 
	//GLuint tex_id;
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->szx,tex->szy,0,GL_RGBA, GL_FLOAT, 
	//	&(tex->checkboard[0]));
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//glEnable(GL_TEXTURE_1D);

	//////////////////////////////////////////////////////////////////////////

	glLineWidth(3.0);
	glPointSize(3.f);*/
}

void Displayer::paintGL()
{
	// Clear the buffer with the current clearing color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Draw stuff
	//glDrawArrays( GL_TRIANGLES, 0, 3 );
	myDisplayable->draw(projMatrix*camMatrix, eye);

	glDebuggingStuff::didIDoWrong();

	
}

void Displayer::resizeGL(int w, int h)
{
	//glViewport(0, 0, this->width(), this->height());   
	glViewport( 0, 0, w, h );
	projMatrix.setToIdentity();
	projMatrix.perspective(60, (0.f + w)/h,1,1000);
}

/*void Displayer::setMode( DisplayMode aMode )
{
	this->mode = aMode;
	this->updateGL();
}*/

//////////////////////////////////////////////////////////////////////////
//the displayer will take care of freeing map
void Displayer::setColormap( colorMap & map )
{
	myDisplayable->sendColorMap(map);
	this->updateGL();
}

void Displayer::mouseMoveEvent( QMouseEvent* event )
{
	if(mouseMode == TRACKBALLMODE){
		tBallListener.onMouseMove(event, *myDisplayable);
	}
	/*else if(mouseMode == INPUTMODE){
		this->strokeListener->onMouseMove(event);
	}*/
}

void Displayer::mousePressEvent( QMouseEvent* event )
{
	if(mouseMode == TRACKBALLMODE){
		tBallListener.onMousePress(event);
	}
	/*else if(mouseMode == INPUTMODE){
		this->strokeListener->onMousePress(event);
	}*/
}

void Displayer::setMouseMode( MouseInputMode aMode )
{
	this->mouseMode = aMode;
}

/*void Displayer::resetStrokes()
{
	this->tmmap->reset();
	updateGL();
}*/

void Displayer::setNormedFieldDisplay( bool what )
{
	this->normedVField = what;
}

void Displayer::setLineWidth( float param1 )
{
	glLineWidth(param1);
}

void Displayer::wheelEvent( QWheelEvent* ev )
{
	/*if(mode == COLORMAPMODE && mouseMode == COLORMAPSCROLL){
		this->map->scrollAction(ev->delta());
	}
	else{
		Model::getModel()->getMesh()->move((0.f + ev->delta())/800);
	}*/
	camMatrix.setToIdentity();
	eye -= eye.normalized()*0.002 * ev->delta();
	camMatrix.lookAt(eye,QVector3D(),up);
	updateGL();
}

void Displayer::setPointCloudDisplay( bool what)
{
	this->displayPointCloud = what;
}

/*void Displayer::update( void * src, Model::modelMsg msg )
{
	if(msg == Model::DISPLAY_CHANGED){
		updateGL();
	}
}*/

void Displayer::setVectorDisplay( bool )
{
	this->displayVectors = true;
	updateGL();
}

void Displayer::setSmooth( bool param1 )
{
	this->smoothShading = param1;
}

void Displayer::update( void * src, glDisplayable::glDispMessage msg )
{
	if(src == myDisplayable && msg == glDisplayable::REFRESH_DISPLAY){
		updateGL();
	}
}


