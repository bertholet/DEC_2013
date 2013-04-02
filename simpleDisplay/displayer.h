#ifndef DISPLAYER_H
#define DISPLAYER_H

#include <QGLWidget>
#include <QGLFormat>

#include "glDisplayable.h"
#include "glDisplayableMesh.h"
#include "trackBallListener.h"
#include "mouseStrokeListener.h"

#include <QMatrix4x4>
#include "colorMap.h"

#include "glVectorfield.h"
#include "Resetable.h"
//#include "Observer.h"
//#include "Model.h"
//#include "squareTexture.h"

enum MouseInputMode {TRACKBALLMODE,INPUTMODE, COLORMAPSCROLL};

class Displayer : public QGLWidget,Observer<glDisplayable::glDispMessage>, 
	mouseStrokeable, public Resetable//public Observer<Model::modelMsg>
{
	Q_OBJECT

public:
	Displayer(QGLFormat & format, QWidget *parent);
	~Displayer();

	//Display the mesh
	void display(wfMesh * amesh);


	//void setMode(DisplayMode aMode);
	void setMouseMode(MouseInputMode aMode);
	void setColormap(colorMap & map);
	void setMarkupMap();
	//void setSmooth( bool smooth );
	void setShader(glDisplayableMesh::SHADER);
	//void showLines();
	void setVFLength( float length);
	void setLineWidth( float wdth );

	markupMap & getMarkupMap();
	//void setNormedFieldDisplay(bool);
	//void setPointCloudDisplay(bool);
	//void setVectorDisplay(bool);
	//void setLineWidth( float param1 );
//	void update(void * src, Model::modelMsg msg);


	virtual void getProjectionMatrix( GLdouble proj[16] );
	virtual void getModelViewMatrix( GLdouble model[16] );
	virtual void getViewport( GLint viewPort[4] );
	virtual tuple3i * intersect( tuple3f & start, tuple3f & stop, int * closestVertex, int * face, tuple3f * position );


	void subscribeDisplayable(glDisplayable * disp);
	void unsubscribeDisplayable(glDisplayable * disp);


	void subscribeToMousestrokes(mouseStrokeProcessor *c);
	void unsubscribeToMousestrokes(mouseStrokeProcessor *c);

	virtual void reset();

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* ev);
	void keyPressEvent ( QKeyEvent * event );
	void keyReleaseEvent ( QKeyEvent * event);

	virtual void update( void * src, glDisplayable::glDispMessage msg );
	//void deleteAllDisplayables();
	void sendOtherDisplayablesToGPU();
	void drawOtherDisplayables();




private:
	glDisplayableMesh * myDisplayable;
	//glVectorfield * glVfield;

	//subscribers
	std::vector<glDisplayable*> subscr_displayables;
	std::vector<mouseStrokeProcessor*> subscr_strokeProcessors;

	QVector3D eye;
	QVector3D up;
	QMatrix4x4 camMatrix, projMatrix;

	//DisplayMode mode;
	MouseInputMode mouseMode;
	markupMap mousestrokemap;
	bool markupmap_isShown;
	//colorMap * actualMap;

	//colorMap * map;
	//triangleMarkupMap * tmmap;

	//bool normedVField;
	//bool displayVField;
	//bool displayPointCloud;
	//trackBallListener * tBallListener;
	mouseStrokeListener strokeListener;
	//bool displayVectors;
	//squareTexture * tex;

	GLuint tex_id;
	//bool smoothShading;
	trackBallListener tBallListener;

	//for the trackball
//	float lastx, lasty, lastz;
};



#endif // DISPLAYER_H
