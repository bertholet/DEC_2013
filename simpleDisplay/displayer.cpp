#include "displayer.h"
//#include "mesh.h"
#include <math.h>
#include <QtGui/QMouseEvent>
#include "glDebuggingStuff.h"
#include "MODEL.h"
#include "glPhongMesh.h"
#include "glVectorfield.h"
#include <algorithm>

#include <QGLContext>
//#include "fluidSimulation.h"

Displayer::Displayer(QGLFormat & format, QWidget *parent)
	: QGLWidget(format,parent), tBallListener(this)
{

	this->setMinimumHeight(300);
	this->setMinimumWidth(300);
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	myDisplayable = /**/new glDisplayableMesh(MODEL::getModel()->getMesh()->getWfMesh());//*/ new glPhongMesh(MODEL::getModel()->getMesh()->getWfMesh());//
	myDisplayable->attach(this);

	/*glVfield = new glVectorfield(MODEL::getModel()->getMesh()->getWfMesh());
	otherDisplayables.push_back(glVfield);*/

	mouseMode = TRACKBALLMODE;
	strokeListener.setStrokedObject(*this);
//	strokeListener.addStrokeProcessor(mousestrokemap);
	mousestrokemap.associateTo(* MODEL::getModel()->getMesh()->getWfMesh());

	eye = QVector3D(0, 0 ,4);
	up = QVector3D(0,1,0);
	camMatrix.lookAt(eye, QVector3D(0,0,0), up );
	projMatrix.perspective(60, this->width()/this->height(),1,500);

	actualMap = NULL;



}

Displayer::~Displayer()
{
	delete myDisplayable;
	//deleteAllDisplayables();
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
	//deleteAllDisplayables();

	delete myDisplayable;
	myDisplayable = new glDisplayableMesh(aMesh);
	mousestrokemap.associateTo(*aMesh);
	myDisplayable->sendToGPU();
	myDisplayable->attach(this);
	glDebuggingStuff::didIDoWrong();
	
	for(int i = 0; i < subscr_displayables.size(); i++ ){
		subscr_displayables[i]->set(myDisplayable->getModel2world(), myDisplayable->getNormalMatrix());
	}
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

	sendOtherDisplayablesToGPU();
	glDebuggingStuff::didIDoWrong();

	//modelViewMatrix = projMatrix*camMatrix * myDisplayable.getModel2world();

	glEnable(GL_DEPTH_TEST);

}

void Displayer::paintGL()
{
	glDebuggingStuff::didIDoWrong();
	// Clear the buffer with the current clearing color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Draw stuff

	myDisplayable->draw(projMatrix*camMatrix, eye);

	drawOtherDisplayables();

	//glDebuggingStuff::didIDoWrong();

	
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

void Displayer::setColormap( colorMap & map )
{
	actualMap = &map;
	myDisplayable->sendColorMap(map);
	this->updateGL();
}


void Displayer::reset()
{
	if(actualMap != NULL){
		myDisplayable->sendColorMap(*actualMap);
		this->updateGL();
	}
}


void Displayer::mouseMoveEvent( QMouseEvent* event )
{
	if(mouseMode == TRACKBALLMODE){
		tBallListener.onMouseMove(event, *myDisplayable, subscr_displayables);
	}
	else if(mouseMode == INPUTMODE){
		strokeListener.onMouseMove(event);
		myDisplayable->sendColorMap(mousestrokemap);
		this->updateGL();
	}
}

void Displayer::mousePressEvent( QMouseEvent* event )
{
	this->grabKeyboard();
	if(mouseMode == TRACKBALLMODE){
		tBallListener.onMousePress(event);
	}
	else if(mouseMode == INPUTMODE){
		strokeListener.onMousePress(event);
	}
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

/*void Displayer::setNormedFieldDisplay( bool what )
{
	this->normedVField = what;
}

void Displayer::setLineWidth( float param1 )
{
	glLineWidth(param1);
}*/

void Displayer::wheelEvent( QWheelEvent* ev )
{
	camMatrix.setToIdentity();
	eye -= eye.normalized()*0.002 * ev->delta();
	camMatrix.lookAt(eye,QVector3D(),up);
	updateGL();
}

/*void Displayer::setPointCloudDisplay( bool what)
{
	this->displayPointCloud = what;
}*/

/*void Displayer::update( void * src, Model::modelMsg msg )
{
	if(msg == Model::DISPLAY_CHANGED){
		updateGL();
	}
}*/

/*void Displayer::setVectorDisplay( bool )
{
	this->displayVectors = true;
	updateGL();
}

void Displayer::setSmooth( bool param1 )
{
	this->smoothShading = param1;
}*/

void Displayer::update( void * src, glDisplayable::glDispMessage msg )
{
	if(src == myDisplayable && msg == glDisplayable::REFRESH_DISPLAY){
		updateGL();
	}
}


void Displayer::keyPressEvent( QKeyEvent * event )
{
	if(event->key()==Qt::Key_Control){
		setMouseMode(INPUTMODE);
		setColormap(mousestrokemap);
	}

}

void Displayer::keyReleaseEvent( QKeyEvent * event )
{
	if(event->key()==Qt::Key_Control){
		setMouseMode(TRACKBALLMODE);
	}
}

void Displayer::getProjectionMatrix( GLdouble proj[16] )
{
	projMatrix.transposed().copyDataTo(proj);
	//projMatrix.copyDataTo( proj);
}

void Displayer::getModelViewMatrix( GLdouble model[16] )
{
	QMatrix4x4 bla;
	bla = camMatrix * myDisplayable->getModel2world();
	bla.transposed().copyDataTo(model);
	//bla.copyDataTo( model);
}

void Displayer::getViewport( GLint viewPort[4] )
{
	glGetIntegerv(GL_VIEWPORT, viewPort);
}

tuple3i * Displayer::intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position )
{
	return myDisplayable->intersect(start,stop,closestVertex, face, position );
}

void Displayer::setSmooth( bool smooth )
{
	((glDisplayableMesh*) myDisplayable)->switchStyle((smooth==true? glDisplayableMesh::PHONG : glDisplayableMesh::FLAT));
	
	/*wfMesh * msh = myDisplayable->getWfMesh();
	delete myDisplayable;
	if(smooth){
		myDisplayable =  new glPhongMesh(msh);
		myDisplayable->sendToGPU();
		myDisplayable->attach(this);
	}
	else{
		myDisplayable = new glDisplayableMesh(msh);
		myDisplayable->sendToGPU();
			myDisplayable->attach(this);
	}*/
}

/*void Displayer::deleteAllDisplayables()
{
	for(int i = 0; i < otherDisplayables.size(); i++){
		delete otherDisplayables[i];
	}
	otherDisplayables.clear();
}*/

void Displayer::sendOtherDisplayablesToGPU()
{
	for(int i = 0; i < subscr_displayables.size(); i++){
		subscr_displayables[i]->sendToGPU();
	}
}

void Displayer::drawOtherDisplayables()
{
	for(int i = 0; i < subscr_displayables.size(); i++){
		subscr_displayables[i]->draw(projMatrix*camMatrix, eye);
	}
}

void Displayer::setVFLength( float param1 )
{
	/*if(glVfield != NULL){
		glVfield->setLength(param1);
	}*/
}

void Displayer::subscribeDisplayable( glDisplayable * disp )
{
	std::vector<glDisplayable*>::iterator it = std::find(subscr_displayables.begin(), subscr_displayables.end(), disp);
	if(it!= subscr_displayables.end()){
		return;
	}
	subscr_displayables.push_back(disp);
	if(!disp->linkedAndReady()&& QGLContext::currentContext()!= NULL && QGLContext::currentContext()->isValid()){
		disp->sendToGPU();
	}
}

void Displayer::unsubscribeDisplayable( glDisplayable * disp )
{
	std::vector<glDisplayable*>::iterator it = std::find(subscr_displayables.begin(), subscr_displayables.end(), disp);
	if(it!=subscr_displayables.end()){
		subscr_displayables.erase(it);
	}
	else{
		cout << "\n**Warning: Unsubscribing unexisting displayable\n";
	}

}

void Displayer::subscribeToMousestrokes( mouseStrokeProcessor *c )
{
	//subscr_strokeProcessors.push_back(c);
	strokeListener.addStrokeProcessor(*c);
}

void Displayer::unsubscribeToMousestrokes( mouseStrokeProcessor *c )
{
	strokeListener.removeStrokeProcessor(*c);
}

markupMap & Displayer::getMarkupMap()
{
	return mousestrokemap;
}

void Displayer::showLines()
{
	myDisplayable->switchStyle(glDisplayableMesh::LINES);
}

void Displayer::setLineWidth( float wdth )
{
	glLineWidth(wdth);
}





