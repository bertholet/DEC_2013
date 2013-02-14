#include "mainwindow.h"
#include <iostream>
#include <QCheckBox>
#include <QPushButton>
//#include "mesh.h"
//#include "curvVisualizingMesh.h"
//#include "generatemeshgui.h"

#include "widget_vfDesign.h"
#include "widget_smoothing.h"
#include "widget_fluidSimulation.h"
#include "widget_meshParam.h"

#include "glDisplayable.h"
#include "glDebuggingStuff.h"
#include "colorMap.h"

#include "MODEL.h"
#include "mySolver.h"

#define SLIDER_STEPSPERUNIT 20


MainWindow::MainWindow(QGLFormat & format): QMainWindow()
{
	AllocConsole();
	freopen("CONOUT$", "wb",stdout);
	
	setupMenubar();
	setupComponents(format);
	setupQTabs();

	addAction();
	layoutGui();

	this->show();
}


MainWindow::~MainWindow()
{
	fclose(stdout);
	FreeConsole();
	MODEL::freeModel();
}

/************************************************************************/
/* The menubar                                                                     */
/************************************************************************/
void MainWindow::setupMenubar() 
{
	this->fileMenu = menuBar()->addMenu("File");
	this->openObjFileAct = new QAction("Open Obj File...", this);
	this->generateMeshAct = new QAction("Genereate Mesh", this);
	this->fileMenu->addAction(openObjFileAct);
	this->fileMenu->addAction(generateMeshAct);
}

/************************************************************************/
/* Seeting up Buttons, Qcombobox, etc                                   */
/************************************************************************/
void MainWindow::setupComponents(QGLFormat & format) 
{
	myGLDisp = new Displayer(format, this);
	this->subscribeResetable( & (myGLDisp->getMarkupMap()));

	comboBox = new QComboBox();
	comboBox->setEditable(false);
	comboBox->addItem("Lines");
	comboBox->addItem("Faces");
	comboBox->addItem("Border");
	comboBox->addItem("Curvature");
	comboBox->addItem("Selections");
	comboBox->addItem("Border Selection");
	comboBox->addItem("FluidSimulation");
	comboBox->addItem("Texture");
	comboBox->addItem("TextureMap");
	comboBox->addItem("Embedding");

	//cbox = new QCheckBox("Draw strokes",this);
	cbox2 = new QCheckBox("Normed Field",this);
	cbox3 = new QCheckBox("Smooth",this);
	butt_reset = new QPushButton("Reset", this);
	cBoxLines = new QCheckBox("Lines", this);
	cBoxLines->setChecked(false);

	fieldSlider = new QSlider(Qt::Horizontal, this);
	fieldSlider->setMinimum(0);
	fieldSlider->setMaximum(2* SLIDER_STEPSPERUNIT);
	fieldSlider->setTickPosition(QSlider::TicksAbove);
	fieldSlider->setValue(SLIDER_STEPSPERUNIT);

	linewidthSlider = new QSlider(Qt::Horizontal, this);
	linewidthSlider->setMinimum(0);
	linewidthSlider->setMaximum(4* SLIDER_STEPSPERUNIT);
	linewidthSlider->setTickPosition(QSlider::TicksAbove);
	linewidthSlider->setValue(SLIDER_STEPSPERUNIT);
}

/************************************************************************/
/* Sets up the Contol elements in the Qtabpanel                         */
/************************************************************************/
void MainWindow::setupQTabs() 
{
	this->tabs = new QTabWidget(this);

	QWidget * fluidSimWidget = new widget_fluidSimulation(this);
	tabs->addTab(fluidSimWidget, "Fluid Simulation");

	vectorfieldWidget * vfWidget = new vectorfieldWidget(this);
	//vfWidget->setMainWindow(this);
	tabs->addTab(vfWidget, "Vector Fields");

	QWidget * paramWidget = new meshParamWidget();
	tabs->addTab(paramWidget, "Mesh Parametrization");


	QWidget * tab1Widget = new smoothingWidget();
	tabs->addTab(tab1Widget, "Smoothing");
}

/************************************************************************/
/* All the connec methods                                               */
/************************************************************************/
void MainWindow::addAction() 
{

	connect(openObjFileAct, SIGNAL(triggered()), this, SLOT(openObjFile()));
	connect(generateMeshAct,SIGNAL(triggered()), this, SLOT(generateMesh()));
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDisplayMode(int)));
//	connect(cbox, SIGNAL(stateChanged(int)), this, SLOT(setMouseMode(int)));
	connect(cbox2, SIGNAL(stateChanged(int)), this, SLOT(setVFieldMode(int)));
	connect(cbox3, SIGNAL(stateChanged(int)), this, SLOT(setSmoothMode(int)));
	connect(butt_reset, SIGNAL(released()), this, SLOT(reset()));

	connect(linewidthSlider, SIGNAL(sliderReleased()), this, SLOT(lineWidthChanged()));
	connect(fieldSlider, SIGNAL(sliderReleased()), this, SLOT(fieldLengthChanged()));
	connect(cBoxLines, SIGNAL(stateChanged(int)), this, SLOT(showLines(int)));


	this->subscribeResetable( myGLDisp);
}

/************************************************************************/
/* Layout all the components                                            */
/************************************************************************/
void MainWindow::layoutGui() 
{
	//layout the gui
	QVBoxLayout * rightLayout = new QVBoxLayout();
	rightLayout->addWidget(comboBox);
	rightLayout->addWidget(tabs);
	QHBoxLayout * sublayout = new QHBoxLayout();
	//sublayout->addWidget(cbox);
	sublayout->addWidget(butt_reset);
	rightLayout->addLayout(sublayout);
	sublayout = new QHBoxLayout();
	sublayout->addWidget(cbox2);
	sublayout->addWidget(cBoxLines);
	sublayout->addWidget(cbox3);
	rightLayout->addLayout(sublayout);
	rightLayout->addWidget(fieldSlider);
	rightLayout->addWidget(linewidthSlider);	

	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->addWidget(myGLDisp,1);
	mainLayout->addLayout(rightLayout,0);

	QWidget * mainWidget = new QWidget();
	mainWidget->setLayout(mainLayout);
	this->setCentralWidget(mainWidget);

	this->setWindowTitle(
		QApplication::translate("toplevel", "DG2011"));
	this->resize(600, 500);
}


/************************************************************************/
/* to Update the GL display in this component                           */
/************************************************************************/
void MainWindow::update()
{
	glDebuggingStuff::didIDoWrong();
	this->myGLDisp->updateGL();
}

/************************************************************************/
/* Takies care of the open obj file Action                                                                     */
/************************************************************************/
void MainWindow::openObjFile()
{

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Obj. File"), "/home/", tr("Obj Files (*.obj)"));
	
	if(!fileName.endsWith("obj")){
		QMessageBox msgBox;
		msgBox.setText("Not an Obj file");
		return;
	}



	MODEL::getModel()->loadMesh(fileName.toAscii());

	myGLDisp->display(MODEL::getModel()->getMesh()->getWfMesh());

	this->update();

}

//////////////////////////////////////////////////////////////////////////
//The Slots.
//////////////////////////////////////////////////////////////////////////


/************************************************************************/
/* Takes care of the generate Mesh action.                              */
/************************************************************************/
void MainWindow::generateMesh()
{
/*	GenMeshDialog genDialog(this);
	genDialog.exec();*/
}



void MainWindow::setDisplayMode( int mode )
{
	if(mode == 0){
		//this->myGLDisp->setMode(EDGEMODE);
	}
	else if(mode == 1){
		this->myGLDisp->setColormap(
			constColor(* MODEL::getModel()->getMesh()->getWfMesh(), tuple3f(0.8,0.3,0.3)));
	}
	else if(mode == 2){
		this->myGLDisp->setColormap(
			borderMap(* MODEL::getModel()->getMesh()));
	}
	else if(mode == 3){
		this->myGLDisp->setColormap(
			angleMap(* MODEL::getModel()->getMesh()));
	}
/*	else if(mode == 4){
		Model & model = *Model::getModel();
		this->myGLDisp->setMode(MOUSEINPUTMODE);
	}
	else if(mode == 5){
		Model & model = *Model::getModel();
		borderMarkupMap * mp = new borderMarkupMap(Model::getModel()->getMeshInfo()->getBorder());
		mp->attach(this->fluidcontWidget);

		this->myGLDisp->setColormap((colorMap *) mp);
		this->myGLDisp->setMode(COLORMAPMODE);

	}
	else if(mode == 6){
		this->myGLDisp->setMode(FLUIDSIMMODE);
	}
	else if(mode == 7){
		this->myGLDisp->setMode(TEXMODE);
	}
	else if(mode == 8){
		this->myGLDisp->setMode(TEXMODE2);
	}
	else if(mode == 9){
		this->myGLDisp->setMode(TEXMODE3);
	}*/

	if(mode == 5){
		this->myGLDisp->setMouseMode(COLORMAPSCROLL);
	}
	else if (mode == 6){
		this->myGLDisp->setMouseMode(INPUTMODE);
	}
	else{
		this->myGLDisp->setMouseMode(TRACKBALLMODE);
	}
}

/*void MainWindow::setMouseMode( int state )
{
	if(state == 0){//unchecked
		this->myGLDisp->setMouseMode(TRACKBALLMODE);
	}
	if(state == 2){//checked
		this->myGLDisp->setMouseMode(INPUTMODE);
		//this->myGLDisp->setMode(MOUSEINPUTMODE);
	}
}*/

void MainWindow::setVFieldMode( int state)
{
	/*if(state == 0){//unchecked
		this->myGLDisp->setNormedFieldDisplay(false);
	}
	if(state == 2){//checked
		this->myGLDisp->setNormedFieldDisplay(true);
	}
	this->update();*/
}

void MainWindow::reset()
{
	for(int i = 0; i < resetables.size(); i++){
		resetables[i]->reset();
	}
	this->update();
}

void MainWindow::lineWidthChanged()
{
	this->myGLDisp->setLineWidth((0.f + this->linewidthSlider->value())/SLIDER_STEPSPERUNIT);
	this->update();
}

void MainWindow::fieldLengthChanged()
{
	float howmuch=  pow(100, (0.f + this->fieldSlider->value())/SLIDER_STEPSPERUNIT -1) - 0.01;
	myGLDisp->setVFLength( pow(100, (0.f + this->fieldSlider->value())/SLIDER_STEPSPERUNIT -1) - 0.01);

	for(int i = 0; i< resizables.size(); i++){
		resizables[i]->setLength(howmuch);
	}
	this->update();
}

void MainWindow::showLines( int val)
{
	if(val == 2){
		myGLDisp->showLines();
	}
	else{
		myGLDisp->setSmooth(cbox3->isChecked());
	}
	this->update();
	//Model::getModel()->setShowArrows(val==2);
	/*if(Model::getModel()->getVField() != NULL){
		Model::getModel()->getVField()->setShowArrows(val==2);
	}*/
}

void MainWindow::setSmoothMode( int what )
{
	this->myGLDisp->setSmooth(what==2);
	this->update();
}

Displayer * MainWindow::getDisplayer()
{
	return myGLDisp;
}

void MainWindow::subscribeResetable( Resetable * r )
{
	resetables.push_back(r);
}

void MainWindow::unSubscribeResetable( Resetable * r )
{
	std::vector<Resetable*>::iterator it = std::find(resetables.begin(), resetables.end(), r);
	if(it!= resetables.end()){
		resetables.erase(it);
	}
}

void MainWindow::subscribeResizables( Resizable * r )
{
	resizables.push_back(r);
}

void MainWindow::unSubscribeResizable( Resizable * r )
{
	std::vector<Resizable*>::iterator it = std::find(resizables.begin(), resizables.end(), r);
	if(it!= resizables.end()){
		resizables.erase(it);
	}
}
