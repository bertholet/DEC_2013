#include "widget_fluidSimulation.h"
#include <QVBoxLayout>
#include <sstream>
#include <stdlib.h>

#include "meshMath.h"

widget_fluidSimulation::widget_fluidSimulation(MainWindow *parent)
	:QWidget(parent)
{
	setUpComponents();
	addAction();
	doLayout();

	animationTimer = new QTimer(this);
	connect( animationTimer, SIGNAL(timeout()), this, SLOT(doSimulation())); 

	harm_component = new glVectorfield();
	mainwindow = parent;

	forceAgeChanged();
	forceStrengthChanged();
	viscosityChanged();
	//timeStepChanged();
	updateTimestepLabel();

	sim = NULL;
	
}


widget_fluidSimulation::~widget_fluidSimulation(void)
{
	delete harm_component;
	if(sim != NULL){
		delete sim;
		sim = NULL;
	}
}

void widget_fluidSimulation::setUpComponents()
{
	but_resetFlux = new QPushButton("Reset Flux!");
	but_simStep = new QPushButton("Do 1 Timestep");
	but_startSim = new QPushButton("Start/Stop Simulation");
	but_borderconstr = new QPushButton("Define Border Constraints");
	but_debug = new QPushButton("Harmonic");

	but_dbg1 = new QPushButton("PathTr");
	but_dbg2= new QPushButton("VortPart");


	label_stepSlider = new QLabel("Timestep Size ()");
	label_viscosity = new QLabel("Viscosity [0,10]");
	label_forceAge = new QLabel("ForceAge (nr Iteratons): ");
	label_forceStrength = new QLabel("Force Strength (): ");
	label_animation = new QLabel("");

	borderCondInput = new QLineEdit();
	borderCondInput->setText("");


	stepSlider = new QSlider(Qt::Horizontal, this);
	stepSlider->setMinimum(0);
	stepSlider->setMaximum(1000);
	stepSlider->setTickPosition(QSlider::TicksAbove);
	stepSlider->setValue(0);

	viscositySlider = new QSlider(Qt::Horizontal, this);
	viscositySlider->setMinimum(0);
	viscositySlider->setMaximum(200);
	viscositySlider->setTickPosition(QSlider::TicksAbove);
	viscositySlider->setValue(0);

	forceAgeSlider = new QSlider(Qt::Horizontal, this);
	forceAgeSlider->setMinimum(0);
	forceAgeSlider->setMaximum(100);
	forceAgeSlider->setTickPosition(QSlider::TicksAbove);
	forceAgeSlider->setValue(5);

	forceStrengthSlider = new QSlider(Qt::Horizontal, this);
	forceStrengthSlider->setMinimum(0);
	forceStrengthSlider->setMaximum(480);
	forceStrengthSlider->setTickPosition(QSlider::TicksAbove);
	forceStrengthSlider->setValue(80);


	cbox_streamLines = new QCheckBox();
	cbox_streamLines->setChecked(false);
	cbox_interpolation = new QCheckBox();
	cbox_interpolation->setChecked(true);
	cbox_vortNotSpeed = new QCheckBox();
	cbox_vortNotSpeed->setChecked(true);
	cbox_texturedLine = new QCheckBox();
	cbox_texturedLine->setChecked(true);


	lineLength = new QSpinBox(this);
	lineLength->setMinimum(1);
	lineLength->setMaximum(50);
	lineLength->setSingleStep(1);

	colorScale = new QSpinBox(this);
	colorScale->setMinimum(1);
	colorScale->setMaximum(100);
	colorScale->setSingleStep(1);
	colorScale->setValue(50);

}

void widget_fluidSimulation::addAction()
{
	connect(but_resetFlux, SIGNAL(released()), this, SLOT(resetFlux()));
	connect(but_simStep, SIGNAL(released()), this, SLOT(singleSimulationStep()));
	connect(but_startSim , SIGNAL(released()), this, SLOT(startSim()));
	connect(but_borderconstr , SIGNAL(released()), this, SLOT(defineBorderConstraints()));
	connect(but_debug , SIGNAL(released()), this, SLOT(harmonicComponent()));

	connect(but_dbg1 , SIGNAL(released()), this, SLOT(pathtrace()));
	connect(but_dbg2 , SIGNAL(released()), this, SLOT(showVorticityPart()));


	connect(stepSlider,SIGNAL(sliderReleased()), this, SLOT(timeStepChanged()));
	connect(viscositySlider,SIGNAL(sliderReleased()), this, SLOT(viscosityChanged()));
	connect(forceAgeSlider,SIGNAL(sliderReleased()), this, SLOT(forceAgeChanged()));
	connect(forceStrengthSlider,SIGNAL(sliderReleased()), this, SLOT(forceStrengthChanged()));

	connect(borderCondInput,SIGNAL(textChanged( const QString& )), this, SLOT(borderDirInput(const QString & )));

	//////////////////////////////////////////////////////////////////////////
	//display settings
	//////////////////////////////////////////////////////////////////////////

	connect(cbox_streamLines, SIGNAL(stateChanged(int)), this, SLOT(showStreamLn(int)));
	connect(cbox_interpolation, SIGNAL(stateChanged(int)), this, SLOT(doInterpl(int)));
	connect(cbox_vortNotSpeed, SIGNAL(stateChanged(int)), this, SLOT(showVorticity(int)));
	connect(cbox_texturedLine, SIGNAL(stateChanged(int)), this, SLOT(showTexLines(int)));

	connect(lineLength, SIGNAL(valueChanged(int)), this, SLOT(streamLineLengthChanged(int)));
	connect(colorScale, SIGNAL(valueChanged(int)), this, SLOT(colorScaleChanged(int)));
}

void widget_fluidSimulation::doLayout()
{
	QLabel * streamLinesInterpolationLable = new QLabel("Lines/Interpol/Linelength/Vort/Tex/Color");


	QVBoxLayout * layout = new QVBoxLayout();

	layout->addWidget(label_animation);
	layout->addWidget(borderCondInput);
	layout->addWidget(but_borderconstr);
	//layout->addWidget(butt);
	//layout->addWidget(butt_defForce);
	layout->addWidget(but_startSim);
	layout->addWidget(but_resetFlux);
	layout->addWidget(but_simStep);

	layout->addWidget(label_stepSlider);
	layout->addWidget(stepSlider);
	layout->addWidget(label_viscosity);
	layout->addWidget(viscositySlider);
	layout->addWidget(label_forceAge);
	layout->addWidget(forceAgeSlider);
	layout->addWidget(label_forceStrength);
	layout->addWidget(forceStrengthSlider);
	//	layout->addWidget(viscosityAndTimestep);



	layout->addWidget(streamLinesInterpolationLable);

	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(cbox_streamLines);
	hlayout->addWidget(cbox_interpolation);
	hlayout->addWidget(lineLength);
	hlayout->addWidget(cbox_vortNotSpeed);
	hlayout->addWidget(cbox_texturedLine);
	hlayout->addWidget(colorScale);
	layout->addLayout(hlayout);

	layout->addWidget(but_debug);


	QHBoxLayout * hlayout2 = new QHBoxLayout();
	hlayout2->addWidget(but_dbg1);
	hlayout2->addWidget(but_dbg2);
	layout->addLayout(hlayout2);

	this->setLayout(layout);
}

//////////////////////////////////////////////////////////////////////////
// Getters
//////////////////////////////////////////////////////////////////////////
float widget_fluidSimulation::getViscosity()
{
	float temp = this->viscositySlider->value()*6 - 3*this->viscositySlider->maximum();
	temp/= this->viscositySlider->maximum();
	temp = pow(10,temp)- 0.001;
	temp = temp < 0.000001?
		0:
		temp;
	return temp;
}

float widget_fluidSimulation::getForceStrength()
{
	float temp = this->forceStrengthSlider->value()*6 - this->forceStrengthSlider->maximum();
	temp/= this->forceStrengthSlider->maximum();
	temp = pow(10,temp)- 0.1;
	temp = temp < 0.00001?
		0:
		temp;
	return temp;
}

float widget_fluidSimulation::getTimestep()
{
	return pow(10.f,-4 + 4*(0.f +this->stepSlider->value())/this->stepSlider->maximum())-pow(10.f,-4);
}

int widget_fluidSimulation::getForceAge()
{
	return forceAgeSlider->value();
}


//////////////////////////////////////////////////////////////////////////
//The Slots
//////////////////////////////////////////////////////////////////////////
void widget_fluidSimulation::doSimulation()
{

}


void widget_fluidSimulation::resetFlux()
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::singleSimulationStep()
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::startSim()
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::defineBorderConstraints()
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::harmonicComponent()
{
	MODEL & model = *MODEL::getModel();
	if(model.getMesh()->getBoundaryEdges().size()==0){
		return;
	}
	std::vector<tuple3f> constr;
	constr.assign(model.getMesh()->getBoundaryEdges().size(), tuple3f());
	constr[0].set(tuple3f(0,1,0));

	//application_fluidSimulation sim(model);
//	sim.setUpSimulation(model);

	ensureSimulationInitialized();
	sim->computeHarmonicFlow(constr,model);
	//harmonic.dualToVField(harm_vField);
	harm_vField = sim->getHarmonicVel();
	harm_circumcenters = sim->getDualVertices();
//	meshMath::circumcenters(*model.getMesh(), harm_circumcenters);

	harm_component->display(&harm_circumcenters,&harm_vField);
	mainwindow->getDisplayer()->subscribeDisplayable(harm_component);
	mainwindow->subscribeResizables(harm_component);

}

void widget_fluidSimulation::pathtrace()
{
	ensureSimulationInitialized();
	sim->pathTraceDualVertices(getTimestep());
	harm_circumcenters = sim->getTracedDualVertices();

	harm_component->display(&harm_circumcenters,&harm_vField);
	mainwindow->getDisplayer()->subscribeDisplayable(harm_component);
	mainwindow->subscribeResizables(harm_component);
}

void widget_fluidSimulation::showVorticityPart()
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::timeStepChanged()
{
	updateTimestepLabel();
	pathtrace();
	mainwindow->update();
}

void widget_fluidSimulation::viscosityChanged()
{
	float viscy = getViscosity();

	std::stringstream ss;
	ss << "Viscosity (" << viscy<< ")";
	this->label_viscosity->setText(ss.str().c_str());
}

void widget_fluidSimulation::forceAgeChanged()
{
	int maxForceAge = getForceAge();

	std::stringstream ss;
	ss << "ForceAge (nr Iteratons): " << maxForceAge;
	this->label_forceAge->setText(ss.str().c_str());
}

void widget_fluidSimulation::forceStrengthChanged()
{
	std::stringstream ss;
	ss << "Force Strength (" << getForceStrength() << "):";
	this->label_forceStrength->setText(ss.str().c_str());
}

void widget_fluidSimulation::borderDirInput( const QString & str)
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::showStreamLn( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::doInterpl( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::showVorticity( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::showTexLines( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::streamLineLengthChanged( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::colorScaleChanged( int what )
{
	throw std::exception("The method or operation is not implemented.");
}

void widget_fluidSimulation::ensureSimulationInitialized()
{
	if(sim == NULL){
		sim = new application_fluidSimulation(*MODEL::getModel());
		//sim->setUpSimulation(*MODEL::getModel());
	}
}

void widget_fluidSimulation::update( void * src, meshMsg msg )
{
	//any tampering with the mesh invalidates the simulation
	if(sim != NULL){
		delete sim;
	}
	sim = NULL;
}

void widget_fluidSimulation::updateTimestepLabel()
{
	float stepSize = getTimestep();

	std::stringstream ss;
	ss << "Timestep Size (" << stepSize << ")";
	this->label_stepSlider->setText(ss.str().c_str());
}




