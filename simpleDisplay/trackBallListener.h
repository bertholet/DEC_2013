#pragma once
#include <QGLWidget>
#include <QMouseEvent>
#include "glDisplayable.h"

class trackBallListener
{
	int a;
public:
	trackBallListener(QGLWidget * display);
	~trackBallListener(void);

	void onMouseMove(QMouseEvent* event, glDisplayable & target, std::vector<glDisplayable*> & furtherTargets = std::vector<glDisplayable*>() );
	void onMousePress(QMouseEvent* event );

private:
	QGLWidget * displyer;
	float lastx, lasty, lastz;
};
