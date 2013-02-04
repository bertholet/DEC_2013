#pragma once

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include "mainwindow.h"
#include "mouseStrokeListener.h"
#include "glVectorfield.h"

class vectorfieldWidget : public QWidget//, public Observer<Model::modelMsg>
{
	Q_OBJECT

public:
	vectorfieldWidget(MainWindow *parent);
	~vectorfieldWidget();

	//void setMainWindow(MainWindow *);
private:
	MainWindow * mainWindow;

	QSlider * gfWeihgtSlider;
	QSlider * flowSlider;
	QSlider * gfLengthSlider;

	QCheckBox * cBoxDirectional;

	int weightStep, srcFlowStep, lengthStep, weightMax;

	bool useBorderMat;

	glVectorfield * myFieldDisplay;
	vertexCollector sources, sinks;
	directionCollector dirs;


	void setupSliders();
	void layoutGui();

//	void initSolver();
private slots:
	void genAxisAllignedField();
	void solveVField();
	void sourceSelection( bool active );
	void sinkSelection( bool active );
	void fieldSelection( bool active );

	//virtual void update( void * src, Model::modelMsg msg );
	//void storeField();
	void useBorderMatrix(int val);
};

