#include "application_smoothing.h"
#include <QTime>
#include "meshMath.h"
#include "SolverConfig.h"

application_smoothing::application_smoothing()
{
}


application_smoothing::~application_smoothing(void)
{
}

void application_smoothing::explicitEuler(MODEL * model, float timeStep)
{
	
	// local variabes and times set up
	wfMesh * myMesh = model->getMesh()->getWfMesh();
	QTime timer;

	//set up the matrix
	cpuCSRMatrix & laplace = model->getLaplace0_mixed();
	cout << "recalc L0:" <<timer.restart()<< "\n";

	//init buffer to right size
	buffer = myMesh->getVertices();
	timer.start();
	
	//compute curvature normals and store them in the buffer
	laplace.mult(model->getMesh()->getWfMesh()->getVertices(),buffer);

	//forward integration step: add delta times curvature normals
	float scale= meshMath::volume(*(model->getMesh()));
	myMesh->translateVertices(buffer, -timeStep);
	scale= pow(scale/meshMath::volume(*(model->getMesh())),1.f/3);
	myMesh->scaleVertices(scale);

	//trigger matrix and display update due to changed positions.
	myMesh->updateObserver(POS_CHANGED);
}

void application_smoothing::implicitEuler( MODEL * model, float timeStep )
{

	wfMesh * myMesh = model->getMesh()->getWfMesh();
//	buffer = myMesh->getVertices();

	//Set up the matrix
	cpuCSRMatrix Id_min_t_laplace = model->getLaplace0_mixed();
	Id_min_t_laplace*= timeStep;
	Id_min_t_laplace.addOnDiagonal(1.f);

	//initiate local variables
	floatVector b(myMesh->getVertices(), 0);
	floatVector x = b,y=b,z=b;


	SolverIF * solver = new solverInstance(SolverIF::MATRIX_STRUCTURALLY_SYMMETRIC);
	solver->setMatrix(Id_min_t_laplace);
	solver->preconditionSystem();

	//do the implicit integration step
	solver->solve(x,b);

	b.set(myMesh->getVertices(),1);
	solver->solve(y,b);

	b.set(myMesh->getVertices(),2);
	solver->solve(z,b);

	delete solver;

	//keep the volume constant to prevent shrinkage
	float scale= meshMath::volume(*(model->getMesh()));
	myMesh->setVertices(x,y,z);
	scale= pow(scale/meshMath::volume(*(model->getMesh())),1.f/3);
	myMesh->scaleVertices(scale);

	myMesh->normalize();
	//trigger a display and matrix update due to changed mesh positions
	myMesh->updateObserver(POS_CHANGED);


	//additional information which shows the mesh degeneration
	model->getMesh()->checkAreaRatios();



}
