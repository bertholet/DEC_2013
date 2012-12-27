#ifndef DISPLAYER_H
#define DISPLAYER_H

//#include "Model.h"
#include <QGLWidget>
#include <QGLFormat>

#include "glDisplayable.h"
#include "trackBallListener.h"

#include <QMatrix4x4>
//#include "curvVisualizingMesh.h"
//#include "trackBallListener.h"
//#include "mouseStrokeListener.h"
//#include "Observer.h"
//#include "Model.h"
//#include "squareTexture.h"

enum DisplayMode {EDGEMODE,FLATMODE,COLORMAPMODE, MOUSEINPUTMODE, FLUIDSIMMODE, TEXMODE, TEXMODE2,TEXMODE3};
enum MouseInputMode {TRACKBALLMODE,INPUTMODE, COLORMAPSCROLL};

class Displayer : public QGLWidget,Observer<glDisplayable::glDispMessage>//public Observer<Model::modelMsg>
{
	Q_OBJECT

public:
	Displayer(QGLFormat & format, QWidget *parent);
	~Displayer();

	//Display the mesh
	void display(wfMesh * amesh);


	void setMode(DisplayMode aMode);
	void setMouseMode(MouseInputMode aMode);
//	void setColormap(colorMap * map);
	void setNormedFieldDisplay(bool);
	void setPointCloudDisplay(bool);
	void setVectorDisplay(bool);
//	void resetStrokes();
	void setLineWidth( float param1 );
//	void update(void * src, Model::modelMsg msg);
	void setSmooth( bool param1 );

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* ev);

	virtual void update( void * src, glDisplayable::glDispMessage msg );

private:
	glDisplayable * myDisplayable;

	QVector3D eye;
	QVector3D up;
	QMatrix4x4 camMatrix, projMatrix;

	DisplayMode mode;
	MouseInputMode mouseMode;
	//colorMap * map;
	//triangleMarkupMap * tmmap;

	bool normedVField;
	bool displayVField;
	bool displayPointCloud;
	//trackBallListener * tBallListener;
	//mouseStrokeListener * strokeListener;
	bool displayVectors;
	//squareTexture * tex;

	GLuint tex_id;
	bool smoothShading;
	trackBallListener tBallListener;

	//for the trackball
//	float lastx, lasty, lastz;
};

#endif // DISPLAYER_H
