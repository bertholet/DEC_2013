#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "Model.h"
#include <QtGui>
#include "displayer.h"
#include <QSlider>
#include <QGLFormat>
//#include "fluidcontrolwidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QGLFormat & format);
	~MainWindow();

	void update();

private:
	QMenu * fileMenu;
	QAction * openObjFileAct;
	QAction * generateMeshAct;

	Displayer * myGLDisp;
	QComboBox * comboBox;
	QTabWidget * tabs;

//	fluidControlWidget * fluidcontWidget;

	QCheckBox * cbox;
	QPushButton * butt;
	QCheckBox * cbox2;
	QCheckBox * cbox3;
	QCheckBox * cBoxArrow;
	QSlider * fieldSlider;
	QSlider * linewidthSlider;


	void setupQTabs();
	void setupMenubar() ;
	void setupComponents(QGLFormat & format);

	void addAction();
	void layoutGui();

private slots:
	void openObjFile();
	void generateMesh();
	void setDisplayMode(int);
	void resetStrokes();

	void setMouseMode(int state);
	void setVFieldMode( int );
	void lineWidthChanged();
	void fieldLengthChanged();
	void showArrows( int );
	void setSmoothMode( int what );
};

#endif // MAINWINDOW_H
