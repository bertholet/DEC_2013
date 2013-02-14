#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <qspinbox.h>

#include "application_fluidSimulation.h"

class widget_fluidSimulation: public QWidget
{
	Q_OBJECT
public:
	widget_fluidSimulation(QWidget *parent = NULL);
	~widget_fluidSimulation(void);

private:
	float stepSize;

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


	void setUpComponents();
	void addAction();
	void doLayout();

protected slots:
	void resetFlux();
	void singleSimulationStep();
	void startSim();
	void defineBorderConstraints();
	void debugSome();
	void pathtrace();
	void showVorticityPart();
	void updateTimeStep();
	void updateViscosity();
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

