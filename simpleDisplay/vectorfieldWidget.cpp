#include "vectorfieldWidget.h"

#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
//#include <QFileDialog>
#include <iostream>
#include <math.h>

#include "MODEL.h"
#include "application_vfDesign.h"
#include "meshMath.h"



vectorfieldWidget::vectorfieldWidget(MainWindow *parent)
	: QWidget(parent)
{

	weightStep = 250;
	weightMax = 500;
	srcFlowStep = 20;
	lengthStep = 100;

	useBorderMat = true;
	this->mainWindow = parent;

	mainWindow->getDisplayer()->subscribeToMousestrokes(& sources);
	mainWindow->getDisplayer()->subscribeToMousestrokes(& sinks);
	mainWindow->getDisplayer()->subscribeToMousestrokes(&dirs);

	vfSelectionDisplay = new glVectorfield();
	vfSelectionDisplay->setColor(QVector3D(0,0,1));
	vfSelectionDisplay->display(dirs.getPositions(), dirs.getDirections());

	computedVFDisplay = new glVectorfield();
	computedVFDisplay->setColor(QVector3D(1,0,0));
	//vfSelectionDisplay->display(dirs.getPositions(), dirs.getDirections());


	mainWindow->getDisplayer()->subscribeDisplayable(vfSelectionDisplay);
	mainWindow->getDisplayer()->subscribeToMousestrokes(vfSelectionDisplay);

	mainWindow->getDisplayer()->subscribeDisplayable(computedVFDisplay);

	mainWindow->subscribeResetable(&dirs);
	mainWindow->subscribeResetable(&sources);
	mainWindow->subscribeResetable(&sinks);
	
	mainWindow->subscribeResizables(vfSelectionDisplay);
	mainWindow->subscribeResizables(computedVFDisplay);
	
	sources.mapTo( &mainWindow->getDisplayer()->getMarkupMap(), tuple3f(1,0.1,0.1));
	sinks.mapTo( &mainWindow->getDisplayer()->getMarkupMap(), tuple3f(0.1,1,0.1));

	setupSliders();
	layoutGui();



//	this->solver = new VectorFieldSolver(Model::getModel()->getMesh(), 
//		*Model::getModel()->getMeshInfo()->getHalfedges(), //
//		*Model::getModel()->getMeshInfo()->getFace2Halfedges());
	//this->solver = NULL;
	//Model::getModel()->attach(this);

}

vectorfieldWidget::~vectorfieldWidget()
{
	//mainWindow->getDisplayer()->unsubscribeDisplayable(myField);
	delete vfSelectionDisplay;
	delete computedVFDisplay;
}


void vectorfieldWidget::solveVField()
{

	// aliases
	wingedMesh & mesh = * MODEL::getModel()->getMesh();
	std::vector<wingedEdge> & edgs = mesh.getEdges();
	std::vector<tuple3i> & f2e = mesh.getf2e();
	std::vector<tuple3f> & vert = mesh.getVertices();
	std::vector<int> & collected_fc = dirs.getFaces();
	std::vector<tuple3f> & collected_dir = * dirs.getDirections();

	//collect all the data needed
	std::vector<int> edges;
	std::vector<float> edges_constr;
	int edg_nr;
	tuple2i edg;

	for(int i = 0; i < collected_fc.size(); i++){
		//iterate over the edges around the ith collected face
		for(int j = 0; j < 3; j++){
			edg_nr = f2e[collected_fc[i]][j];
			edg = edgs[edg_nr].getTuple();
			edges.push_back(edg_nr);
			edges_constr.push_back((vert[edg.b]-vert[edg.a]).dot(collected_dir[i]));
		}
		
	}

	/*///////////////////*** ID IE IB IU IG IG II IN IG *** /////////////////////
	
	for(int i = 0; i < collected_fc.size(); i++){
		//iterate over the edges around the ith collected face
		for(int j = 0; j < 3; j++){
			edg_nr = f2e[collected_fc[i]][j];
			edg = edgs[edg_nr].getTuple();
			edges.push_back(edg_nr);
			edges_constr.push_back((vert[edg.b]-vert[edg.a]).dot(tuple3f(1,0,0)));
		}

	}

	oneForm aSolution(&mesh);
	aSolution.resize(mesh.getEdges().size());
	for(int i = 0; i < aSolution.size(); i++){
		edg = edgs[i].getTuple();
		aSolution[i] = (vert[edg.b]-vert[edg.a]).dot(tuple3f(1,0,0));
	}
	aSolution.saveVector("sol", "vf_aSolution.m");

	oneForm dualSolution(&mesh);
	dualSolution.resize(mesh.getEdges().size());
	for(int i = 0; i < dualSolution.size(); i++){
		edg = edgs[i].getTuple();
		dualSolution[i] = (vert[edg.b]-vert[edg.a]).dot(tuple3f(0,1,0));
	}
	dualSolution.saveVector("dual_sol", "vf_dualSolution.m");

	oneForm onesOnInner(&mesh);
	onesOnInner.onesOnInnerEdges();
	onesOnInner.saveVector("innerEdges", "vf_inneredges.m");

	floatVector onesBoundaryVertices(mesh.getVertices().size());
	for(int i = 0; i < edgs.size(); i++){
		if(edgs[i].isOnBorder()){
			onesBoundaryVertices[edgs[i].start()] = 1;
			onesBoundaryVertices[edgs[i].end()] = 1;
		}
	}
	onesBoundaryVertices.saveVector("bVerts", "vf_bv.m");

	floatVector cornerVertices(mesh.getVertices().size());
	for(int i = 0; i < vert.size(); i++){
		if(mesh.getv2e()[i].size() == 2){
			cornerVertices[i] = 1;
		}
	}
	cornerVertices.saveVector("cVerts", "vf_cv.m");
	//////////////////////////////////////////////////////////////////////////*/
	

	float srcFlow = flowSlider->value();
	srcFlow /= srcFlowStep;
	srcFlow = pow(10, srcFlow-1);

	float weight = gfWeihgtSlider->value();
	weight = 8*weight/weightStep;
	weight = pow(10, weight-8) - pow(10,-8.f);
	weight = (weight > 0 ? weight: 0.f);
	float constraintLength = pow(10, -1 + (0.f + gfLengthSlider->value())/lengthStep);


	//set up local vars
	application_vfDesign vf_design;
	vf_design.setAdaptToBorder(useBorderMat);
	oneForm field(MODEL::getModel()->getMesh());

	// do the vector field computation
	vf_design.computeField(*MODEL::getModel(),
		sources.getVertices(), 
		sinks.getVertices(),
		edges, 
		edges_constr,
		field);


	//postprocessing: convert 1-form to vector field
	field.toVField(vf_dirs);
	//meshMath::circumcenters(mesh,vf_pos);
	meshMath::centroids(mesh,vf_pos);


	//display
	computedVFDisplay->display(&vf_pos, & vf_dirs);

	/*for(int i = 0; i < collector.sinkVert.size(); i++){
		verts.push_back(collector.sinkVert[i]);
		constr.push_back(srcFlow);
	}
	for(int i = 0; i < collector.sourceVert.size(); i++){
		verts.push_back(collector.sourceVert[i]);
		constr.push_back(-srcFlow);
	}

	solver->perturb(verts, constr);*/



/*	if(! cBoxDirectional->isChecked()){
		solver->solve(verts, constr,
			Model::getModel()->getInputCollector().getEdges(),
			Model::getModel()->getInputCollector().getEdgeDirs(),
			weight * (Model::getModel()->getMeshInfo()->getHalfedges()->size()),
			constraintLength,
			Model::getModel()->getVField(),
			useBorderMat);
	}
	else{
		solver->solveLengthEstimated(verts, constr,
			Model::getModel()->getInputCollector().getFaces(),
			Model::getModel()->getInputCollector().getFaceDir(),
			weight * (Model::getModel()->getMeshInfo()->getHalfedges()->size()),
			Model::getModel()->getVField(),
			useBorderMat);
	}

	if(mainWindow != NULL){
		mainWindow->update();
	}*/
}

void vectorfieldWidget::sourceSelection( bool active )
{
	if(active){
	 sources.setActive(true);
	 sinks.setActive(false);
	 dirs.setActive(false);
	}
}

void vectorfieldWidget::sinkSelection( bool active )
{
	if(active){
		sources.setActive(false);
		sinks.setActive(true);
		dirs.setActive(false);
	}
}

void vectorfieldWidget::fieldSelection( bool active )
{
	if(active){
		sources.setActive(false);
		sinks.setActive(false);
		dirs.setActive(true);
	}
}

/*void vectorFieldControlWidget::update( void * src, Model::modelMsg msg )
{
	if(msg == Model::NEW_MESH_CREATED){
		if(solver != NULL){
			delete solver;
		}
		solver = NULL;
	}
}*/

/*void vectorFieldControlWidget::setMainWindow( MainWindow * w)
{
	this->mainWindow = w;
}*/

/*void vectorFieldControlWidget::initSolver()
{
	myStatusBar bar(NULL);
	bar.open();
	bar.setBar(0,10);
	bar.updateBar(0);
	this->solver = new VectorFieldSolver(Model::getModel()->getMesh(), 
		*Model::getModel()->getMeshInfo()->getHalfedges(), 
		*Model::getModel()->getMeshInfo()->getFace2Halfedges(), &bar);
}*/

/*void vectorFieldControlWidget::storeField()
{

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Select Obj. File"), "/home/", tr("Obj Files (*.obj)"));

	if(!fileName.endsWith("obj")){
		QMessageBox msgBox;
		msgBox.setText("Not an Obj file");
		return;
	}

	ObjFileWriter writer;
	writer.writeObjFile(fileName.toAscii(),*(Model::getModel()->getMesh()),*(Model::getModel()->getVField()));
}*/

void vectorfieldWidget::useBorderMatrix( int val )
{
	useBorderMat = (val == 2);
}

void vectorfieldWidget::setupSliders()
{
	gfWeihgtSlider = new QSlider(Qt::Horizontal, this);
	gfWeihgtSlider->setMinimum(0);
	gfWeihgtSlider->setMaximum(weightMax);
	gfWeihgtSlider->setTickPosition(QSlider::TicksAbove);
	gfWeihgtSlider->setValue(weightStep);
	connect(gfWeihgtSlider, SIGNAL(sliderReleased()), this, SLOT(solveVField()));

	flowSlider = new QSlider(Qt::Horizontal, this);
	flowSlider->setMinimum(0);
	flowSlider->setMaximum(10*srcFlowStep);
	flowSlider->setTickPosition(QSlider::TicksAbove);
	flowSlider->setValue(srcFlowStep);
	connect(flowSlider, SIGNAL(sliderReleased()), this, SLOT(solveVField()));

	gfLengthSlider = new QSlider(Qt::Horizontal, this);
	gfLengthSlider->setMinimum(0);
	gfLengthSlider->setMaximum(5*lengthStep);
	gfLengthSlider->setTickPosition(QSlider::TicksAbove);
	gfLengthSlider->setValue(lengthStep);
	connect(gfLengthSlider, SIGNAL(sliderReleased()), this, SLOT(solveVField()));
}

void vectorfieldWidget::layoutGui()
{
	QPushButton *butt_solve = new QPushButton("Solve VField!");
	connect(butt_solve, SIGNAL(released()), this, SLOT(solveVField()));

	/*	QPushButton *butt3 =new QPushButton("Store VField!");
	connect(butt3, SIGNAL(released()), this, SLOT(storeField()));*/


	QRadioButton * rbutt = new QRadioButton("Select Sources", this);
	connect(rbutt, SIGNAL(toggled(bool)), this, SLOT(sourceSelection(bool)));
	QRadioButton * rbutt2 = new QRadioButton("Select Sinks", this);
	connect(rbutt2, SIGNAL(toggled(bool)), this, SLOT(sinkSelection(bool)));
	QRadioButton * rbutt3 = new QRadioButton("Select Guide Field", this);
	connect(rbutt3, SIGNAL(toggled(bool)), this, SLOT(fieldSelection(bool)));


	QLabel * sliderLabel1 = new QLabel("Guide Field Enforcement:", this);
	QLabel * sliderLabel2 = new QLabel("Source Flow:", this);
	QLabel * sliderLabel3 = new QLabel("Constraint Field length:", this);

	cBoxDirectional = new QCheckBox("Directional Constraint", this);

	QCheckBox * cBoxBorderMatrix = new QCheckBox("Border Matrix", this);
	cBoxBorderMatrix->setChecked(useBorderMat);
	connect(cBoxBorderMatrix, SIGNAL(stateChanged(int)), this, SLOT(useBorderMatrix(int)));

	//	QCheckBox * cBoxArrow = new QCheckBox("Show Arrows", this);
	//	cBoxArrow->setChecked(false);
	//	connect(cBoxArrow, SIGNAL(stateChanged(int)), this, SLOT(showArrows(int)));

	QVBoxLayout * layout = new QVBoxLayout();
	//	layout->addWidget(cbox);
	layout->addWidget(rbutt);
	layout->addWidget(rbutt2);
	layout->addWidget(rbutt3);
	layout->addWidget(sliderLabel2);
	layout->addWidget(flowSlider);
	layout->addWidget(sliderLabel1);
	layout->addWidget(gfWeihgtSlider);
	layout->addWidget(sliderLabel3);
	layout->addWidget(gfLengthSlider);
	layout->addWidget(butt_solve);

	layout->addWidget(cBoxDirectional);
	layout->addWidget(cBoxBorderMatrix);
	//layout->addWidget(cBoxArrow);
	this->setLayout(layout);
}



