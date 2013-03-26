#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "Model.h"
#include <QtGui>
#include "displayer.h"
#include <QSlider>
#include <QGLFormat>
#include "Resetable.h"
//#include "fluidcontrolwidget.h"



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QGLFormat & format);
	~MainWindow();

	void update();
	Displayer * getDisplayer();


	void subscribeResetable(Resetable * r);
	void unSubscribeResetable(Resetable * r);
	void subscribeResizables(Resizable * r);
	void unSubscribeResizable(Resizable * r);
private:
	QMenu * fileMenu;
	QAction * openObjFileAct;
	QAction * generateMeshAct;

	Displayer * myGLDisp;
	QComboBox * cbox_shader, *cbox_colormap;
	QTabWidget * tabs;
	Activable * vd_widget, *fs_widget;

//	fluidControlWidget * fluidcontWidget;

	//QCheckBox * cbox;
	QPushButton * butt_reset;
	QCheckBox * cbox2;
	QCheckBox * cbox3;
	QCheckBox * cBoxLines;
	QSlider * fieldSlider;
	QSlider * linewidthSlider;

	std::vector<Resetable * > resetables;
	std::vector<Resizable * > resizables;
	int fluidSim_tabID,vfield_tabID;

	void setupQTabs();
	void setupMenubar() ;
	void setupComponents(QGLFormat & format);

	void addAction();
	void layoutGui();

private slots:
	void openObjFile();
	void generateMesh();
	void setShader(int);
	void setColorMode(int);
	void reset();

	//void setMouseMode(int state);
	void setVFieldMode( int );
	void lineWidthChanged();
	void fieldLengthChanged();
	void showLines( int );
	void setSmoothMode( int what );
	void tabChanged( int tab );
};


#endif // MAINWINDOW_H
