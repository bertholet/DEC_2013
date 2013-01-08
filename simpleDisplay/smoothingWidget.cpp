#include "smoothingWidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include <sstream>
#include <math.h>
#include "MODEL.h"

smoothingWidget::smoothingWidget(QWidget *parent):QWidget(parent)
{
	//implicitSmoother = NULL;
	timeStep = 0.001f; 
	smootherTimer = new QTimer(this);
	connect( smootherTimer, SIGNAL(timeout()), this, SLOT(doSmoothing()) );
	implicitSmootherTimer = new QTimer(this);
	connect( implicitSmootherTimer, SIGNAL(timeout()), this, SLOT(doImplicitSmoothing()) );


	QPushButton * smoothButton = new QPushButton("Explicit Smoothing");
	connect(smoothButton,SIGNAL(released()), this, SLOT(startDirectSmoothing()));

	QPushButton * implicitSmoothButton = new QPushButton("Implicit Smoothing");
	connect(implicitSmoothButton,SIGNAL(released()), this, SLOT(startImplicitSmoothing()));

	QPushButton * addNoiseButton = new QPushButton("Add Noise");
	connect(addNoiseButton,SIGNAL(released()), this, SLOT(addNoise()));

	timeLabel = new QLabel("Timestep ()");
	noiseLabel = new QLabel("Noise ()");

	timeStepSlider = new QSlider(Qt::Horizontal, this);
	timeStepSlider->setMinimum(1);
	timeStepSlider->setMaximum(1000);
	timeStepSlider->setTickPosition(QSlider::TicksAbove);
	connect(timeStepSlider,SIGNAL(sliderReleased()), this, SLOT(updateTimeStep()));

	noiseSlider = new QSlider(Qt::Horizontal, this);
	noiseSlider->setMinimum(1);
	noiseSlider->setMaximum(1000);
	noiseSlider->setTickPosition(QSlider::TicksAbove);
	connect(noiseSlider,SIGNAL(sliderReleased()), this, SLOT(updateNoise()));


	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(timeLabel);
	layout->addWidget(timeStepSlider);
	layout->addWidget(smoothButton);
	layout->addWidget(implicitSmoothButton);
	layout->addWidget(noiseLabel);
	layout->addWidget(noiseSlider);
	layout->addWidget(addNoiseButton);

	this->setLayout(layout);
}

smoothingWidget::~smoothingWidget(void)
{
}

void smoothingWidget::updateTimeStep()
{
	this->timeStep = pow(10.f,10*(0.f + this->timeStepSlider->value()) /this->timeStepSlider->maximum() -5);
	std::stringstream ss;
	ss << "Timestep (" << this->timeStep << "):";
	this->timeLabel->setText(ss.str().c_str());

//	smoother.setdt(timeStep);
}

void smoothingWidget::updateNoise()
{
	this->noiseLevel =  (0.f+this->noiseSlider->value()) /this->noiseSlider->maximum();//pow(10.f,10*(0.f + this->noiseSlider->value()) /this->noiseSlider->maximum() -5);
	std::stringstream ss;
	ss << "Noise (" << this->noiseLevel << "):";
	this->noiseLabel->setText(ss.str().c_str());
}

void smoothingWidget::startDirectSmoothing()
{
	if(smootherTimer->isActive()){
		smootherTimer->stop();
	}
	else{
		smootherTimer->start(10);
	}
}

void smoothingWidget::doSmoothing()
{

	smoother.explicitEuler( MODEL::getModel(), timeStep);
	//	smoother.smootheMesh_explicitEuler(*(Model::getModel()->getMesh()));
	//Model::getModel()->getMesh()->updateObserver(meshMsg::POS_CHANGED);
//	Model::getModel()->updateObserver(Model::DISPLAY_CHANGED);
}

void smoothingWidget::startImplicitSmoothing()
{
	smoother.implicitEuler(MODEL::getModel(), timeStep);

/*	if(implicitSmootherTimer->isActive()){
		implicitSmootherTimer->stop();
		delete implicitSmoother;
		implicitSmoother = NULL;
	}
	else{
		assert(implicitSmoother == NULL);
		implicitSmoother = new ImplicitEulerSmoothing(*(Model::getModel()->getMesh()),1,timeStep);
		implicitSmootherTimer->start(10);
	}*/
}

void smoothingWidget::doImplicitSmoothing()
{
	/*assert(implicitSmoother != NULL);
	//hack
	delete implicitSmoother;
	implicitSmoother = new ImplicitEulerSmoothing(*(Model::getModel()->getMesh()),1,timeStep);
	//kcah
	implicitSmoother->smootheMesh(*(Model::getModel()->getMesh()));
	Model::getModel()->updateObserver(Model::DISPLAY_CHANGED);*/
}

void smoothingWidget::addNoise()
{
	MODEL::getModel()->getMesh()->getWfMesh()->addOrthogonalNoise(noiseLevel);
	MODEL::getModel()->getMesh()->getWfMesh()->updateObserver(POS_CHANGED);
}



