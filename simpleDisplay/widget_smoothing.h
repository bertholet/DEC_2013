#pragma once
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include "application_smoothing.h"

class smoothingWidget: public QWidget
{
	Q_OBJECT
public:
	smoothingWidget(QWidget *parent = 0);
	~smoothingWidget(void);

	public slots:
		void updateTimeStep();
		void startDirectSmoothing();
		void doSmoothing();
		void startImplicitSmoothing();
		void doImplicitSmoothing();
		void addNoise(void);
		void updateNoise();
private:
	QTimer * smootherTimer, * implicitSmootherTimer;
	QLabel * timeLabel, *noiseLabel;
	QSlider * timeStepSlider, *noiseSlider;
	float timeStep, noiseLevel;

	application_smoothing smoother;
};

