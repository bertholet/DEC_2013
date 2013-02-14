#pragma once

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include "mainwindow.h"
#include "mouseStrokeListener.h"
#include "glVectorfield.h"
#include "Resetable.h"

class vectorfieldWidget : public QWidget, public Resetable//, public Observer<Model::modelMsg>
{
	Q_OBJECT

public:
	vectorfieldWidget(MainWindow *parent);
	~vectorfieldWidget();

	//void setMainWindow(MainWindow *);
private:
	MainWindow * mainWindow;

	//input arguments
	int weightStep, srcFlowStep, lengthStep, weightMax;
	QSlider * gfWeihgtSlider;
	QSlider * flowSlider;
	QSlider * gfLengthSlider;
	QCheckBox * cBoxDirectional;
	bool useBorderMat;

	//mousestroke collectors for input constraints
	vertexCollector sources, sinks;
	directionCollector dirs;

	//buffers to store the computed vector field in.
	std::vector<tuple3f> vf_dirs, vf_pos;

	//display facility for the current constraints and
	//the current computed vector field.
	glVectorfield * vfSelectionDisplay, *computedVFDisplay;

	
	void setupSliders();
	void layoutGui();

	void getCollectedEdgeConstraints( wingedMesh &mesh, std::vector<int> &edges, std::vector<float> &edges_constr );
	float getSourceFlow();
	float getGuideFieldWeight();
	float getGuideFieldLength();

	virtual void reset();
//	void initSolver();
private slots:
	void solveVField();
	void sourceSelection( bool active );
	void sinkSelection( bool active );
	void fieldSelection( bool active );

	//virtual void update( void * src, Model::modelMsg msg );
	//void storeField();
	void useBorderMatrix(int val);


};

