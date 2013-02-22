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

class widget_fluidSimulation: public QWidget, public meshObserver
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
	application_fluidSimulation * sim;

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
	QPushButton * but_dbg_harmonic, *but_dbg_pathtrace, *but_dbg_vort, *but_dbg_diffusion;
	QCheckBox *cbox_streamLines, *cbox_interpolation;
	QCheckBox *cbox_vortNotSpeed, *cbox_texturedLine;

	QSpinBox * lineLength, * colorScale ;

	//display stuff
	MainWindow * mainwindow;
	//harmonic component display
	std::vector<tuple3f> harm_vField,harm_circumcenters;
	glVectorfield * harm_component;
	//vorticity display
	vorticityMap colormap_vorts;
	
	void setUpComponents();
	void addAction();
	void doLayout();

	virtual void update( void * src, meshMsg msg );
	void ensureSimulationInitialized();

protected slots:
	void doSimulation();

	void resetFlux();
	void singleSimulationStep();
	void startSim();

	

	void harmonicComponent();
	void pathtrace();
	void showVorticityPart();
	void diffuse();
	
	//input stuff
	void defineBorderConstraints();

	void timeStepChanged();
	void updateTimestepLabel();

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

