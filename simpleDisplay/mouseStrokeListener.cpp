#include "mouseStrokeListener.h"
#include <GL/GLU.h>
#include "tuple3.h"
#include "qalgorithms.h"

mouseStrokeListener::mouseStrokeListener(void)
{
	strokedObject = NULL;
}


mouseStrokeListener::~mouseStrokeListener(void)
{
}

void mouseStrokeListener::setStrokedObject( mouseStrokeable & mouseStroked )
{
	strokedObject = &mouseStroked;
}

void mouseStrokeListener::addStrokeProcessor( mouseStrokeProcessor & who )
{
	strokeProcessor.push_back(&who);
}


void mouseStrokeListener::removeStrokeProcessor( mouseStrokeProcessor & processor )
{
	std::vector<mouseStrokeProcessor*>::iterator it;
	it =std::find(strokeProcessor.begin(), strokeProcessor.end(), &processor);
	if(it!=strokeProcessor.end()){
		strokeProcessor.erase(it);
	}
	else{
		std::cout << "\n***Warning: unsubscribing unexistent Strokeprocessor";
	}
}


void mouseStrokeListener::onMouseMove( QMouseEvent* event )
{
	if(resolve(event)){
		for(int i = 0; i < strokeProcessor.size(); i++){
			strokeProcessor[i]->process(intersec_vertex, intersec_face, intersec_pos);
		}
	}
}

void mouseStrokeListener::onMousePress( QMouseEvent* event )
{
	if(resolve(event)){
		for(int i = 0; i < strokeProcessor.size(); i++){
			strokeProcessor[i]->processOnPress(intersec_vertex, intersec_face, intersec_pos);
		}
	}
}

bool mouseStrokeListener::resolve( QMouseEvent* event )
{
	if(strokedObject == NULL||strokeProcessor.size() == 0){
		return false;
	}

	strokedObject->getProjectionMatrix(proj);
	strokedObject->getModelViewMatrix(model);
	strokedObject->getViewport(viewPort);


	float winx = (float) event->x() , 
		winy = (float) (viewPort[3] - event->y()), winz;//windows vs opengl battle


	glReadPixels(int(winx), int(winy), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);

	gluUnProject(winx, winy,0, model,proj,viewPort,&p1x,&p1y,&p1z);
	start.set(p1x,p1y,p1z);

	gluUnProject(winx,winy, 0.5, model,proj,viewPort,&p1x,&p1y,&p1z);
	stop.set(p1x,p1y,p1z);


	if(strokedObject->intersect(start, stop, &intersec_vertex, &intersec_face, &intersec_pos) == NULL){
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

void directionCollector::process( int intersec_vertex, int face, tuple3f & intersec_pos )
{
	if(!isActive){
		return;
	}
	tuple3f dirToPush;
	dirToPush.set(intersec_pos);
	dirToPush-= last_pos;
	dirToPush.normalize();
	std::vector<int>::iterator it = find(faces.begin(), faces.end(), face);
	if(it == faces.end()){
		faces.push_back(face);
		directions.push_back(dirToPush);
		positions.push_back(intersec_pos);
	}

	else{
		directions[it - faces.begin()] += dirToPush * 0.3f;
		directions[it - faces.begin()].normalize();
	}

	last_pos = intersec_pos;
}

void directionCollector::processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	last_pos = intersec_pos;
}

void directionCollector::reset()
{
	faces.clear();
	directions.clear();
	positions.clear();
}

directionCollector::directionCollector()
{
	isActive = false;
}

void directionCollector::setActive( bool what )
{
	isActive = what;
}

std::vector<tuple3f>* directionCollector::getPositions()
{
	return &positions;
}

std::vector<tuple3f>* directionCollector::getDirections()
{
	return &directions;
}


//////////////////////////////////////////////////////////////////////////
void vertexCollector::process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	if(!isActive){
		return;
	}

	std::vector<int>::iterator it = find(vertices.begin(), vertices.end(), intersec_vertex);
	if(it == vertices.end()){
		vertices.push_back(intersec_vertex);
		if(myMap!= NULL){
			myMap->mark(intersec_vertex, col);
		}
	}
}

void vertexCollector::processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	process(intersec_vertex, intersec_face, intersec_pos);
}

void vertexCollector::reset()
{
	vertices.clear();
}

void vertexCollector::setActive( bool what )
{
	isActive = what;
}

vertexCollector::vertexCollector()
{
	isActive = false;
	myMap = NULL;
}

void vertexCollector::mapTo( markupMap * map, tuple3f color )
{
	myMap = map;
	col = color;
}
