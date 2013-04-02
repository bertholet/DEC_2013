#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <qspinbox.h>
#include <QDoubleSpinBox>
#include <QTimer>
#include "mainwindow.h"
#include "glVectorfield.h"
#include "mouseStrokeListener.h"
#include "Resetable.h"

#include "application_fluidSimulation.h"

class widget_fluidSimulation: public QWidget, public meshObserver,public Activable
{
	Q_OBJECT
public:
	widget_fluidSimulation(MainWindow *parent);
	~widget_fluidSimulation(void);


	float getViscosity();
	float getForceStrength();
	float getTimestep();
	int getForceAge();
	float getColorScale();

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

	QPushButton * but_resetFlux;
	QPushButton * but_simStep;
	QPushButton * but_startSim;
	QPushButton * but_borderconstr;
	QPushButton * but_dbg_harmonic, *but_dbg_pathtrace, *but_dbg_vort, *but_dbg_diffusion,
		*but_dbg_vort2flux, *but_dbg_resetForces, * but_dbg_addForces, *but_dbg_total;
	QCheckBox *cbox_streamLines, *cbox_interpolation;
	QCheckBox *cbox_vortNotSpeed, *cbox_texturedLine;

	QSpinBox * lineLength, * colorScale ;
	QDoubleSpinBox *spin_x, *spin_y, *spin_z;

	//display stuff
	MainWindow * mainwindow;
	//harmonic component display
	std::vector<tuple3f> vfield_vectors,vfield_pos;
	glVectorfield * gl_vfiled;

	//force collection and display
	directionCollector forceCollector;
	glVectorfield * gl_vfield_forces;


	//vorticity display
	vorticityMap colormap_vorts;
	
	void setUpComponents();
	void addAction();
	void doLayout();
	void setUpForceCollection();
	virtual void activateInput();
	virtual void desactivateInput();

	virtual void update( void * src, meshMsg msg );
	void ensureSimulationInitialized();

protected slots:
	void doSimulation();

	void resetFlux();
	void resetForces();
	void singleSimulationStep();
	void startSim();

	
	//substeps, accessible through the GUI.
	void harmonicComponent();
	void pathtrace();
	void showVorticityPart();
	void addForces( void );
	void diffuse();
	void vort2flux();
	void total( void );
	
	//input stuff

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

