#include "simpledisplay.h"

simpleDisplay::simpleDisplay(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
}

simpleDisplay::~simpleDisplay()
{

}
