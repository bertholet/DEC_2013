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
private:
	QTimer * smootherTimer, * implicitSmootherTimer;
	QLabel * timeLabel;
	QSlider * timeStepSlider;
	float timeStep;

	application_smoothing smoother;
};

