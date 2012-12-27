#ifndef SIMPLEDISPLAY_H
#define SIMPLEDISPLAY_H

#include <QtGui/QMainWindow>
#include "ui_simpledisplay.h"

class simpleDisplay : public QMainWindow
{
	Q_OBJECT

public:
	simpleDisplay(QWidget *parent = 0, Qt::WFlags flags = 0);
	~simpleDisplay();

private:
	Ui::simpleDisplayClass ui;
};

#endif // SIMPLEDISPLAY_H
