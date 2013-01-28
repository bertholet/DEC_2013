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
	
	strokeProcessor.erase(find(strokeProcessor.begin(), strokeProcessor.end(), &processor));
}


void mouseStrokeListener::onMouseMove( QMouseEvent* event )
{
	if(strokedObject == NULL||strokeProcessor.size() == 0){
		return;
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
		return;
	}

	for(int i = 0; i < strokeProcessor.size(); i++){
		strokeProcessor[i]->process(intersec_vertex, intersec_face, intersec_pos);
	}
}

void mouseStrokeListener::onMousePress( QMouseEvent* event )
{

}

