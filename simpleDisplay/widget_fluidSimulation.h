#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <qspinbox.h>
#include <QTimer>
#include "mainwindow.h"
#include "glVectorfield.h"

#include "application_fluidSimulation.h"

class widget_fluidSimulation: public QWidget
{
	Q_OBJECT
public:
	widget_fluidSimulation(MainWindow *parent);
	~widget_fluidSimulation(void);


	float getViscosity();
	float getForceStrength();
	float getTimestep();
	int getForceAge();

private:
	//float stepSize;

	//input gui elements
	QSlider * stepSlider;
	QSlider * viscositySlider;
	QSlider * forceAgeSlider;
	QSlider * forceStrengthSlider;

	QTimer * animationTimer;
	QLabel * label_viscosity;
	QLabel * label_stepSlider;
	QLabel * label_animation;
	QLabel * label_forceAge;
	QLabel * label_forceStrength;
	QLineEdit * borderCondInput;

	QPushButton * but_resetFlux;
	QPushButton * but_simStep;
	QPushButton * but_startSim;
	QPushButton * but_borderconstr;
	QPushButton * but_debug, *but_dbg1, *but_dbg2;
	QCheckBox *cbox_streamLines, *cbox_interpolation;
	QCheckBox *cbox_vortNotSpeed, *cbox_texturedLine;

	QSpinBox * lineLength, * colorScale ;

	//display stuff
	MainWindow * mainwindow;
	//harmonic component display
	std::vector<tuple3f> harm_vField,harm_circumcenters;
	glVectorfield * harm_component;
	
	void setUpComponents();
	void addAction();
	void doLayout();

protected slots:
	void doSimulation();

	void resetFlux();
	void singleSimulationStep();
	void startSim();
	void defineBorderConstraints();
	void harmonicComponent();
	void pathtrace();
	void showVorticityPart();
	void timeStepChanged();
	void viscosityChanged();
	void forceAgeChanged();
	void forceStrengthChanged();
	void borderDirInput(const QString & str);
	void showStreamLn( int what );
	void doInterpl( int what );
	void showVorticity( int what );
	void showTexLines( int what );
	void streamLineLengthChanged( int what );
	void colorScaleChanged( int what );
};

