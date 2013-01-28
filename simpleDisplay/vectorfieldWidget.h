#pragma once

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include "mainwindow.h"

class vectorfieldWidget : public QWidget//, public Observer<Model::modelMsg>
{
	Q_OBJECT

public:
	vectorfieldWidget(QWidget *parent = 0);
	~vectorfieldWidget();

	//void setMainWindow(MainWindow *);
private:
	QSlider * gfWeihgtSlider;
	MainWindow * mainWindow;
	QSlider * flowSlider;
	QCheckBox * cBoxDirectional;

	int weightStep, srcFlowStep;
	QSlider * gfLengthSlider;
	int lengthStep;
	int weightMax;
	bool useBorderMat;
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

