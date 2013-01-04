#include "application_smoothing.h"
#include <QTime>
#include "meshMath.h"
#include "mySolver.h"

application_smoothing::application_smoothing()
{
}


application_smoothing::~application_smoothing(void)
{
}

void application_smoothing::explicitEuler(MODEL * model, float timeStep)
{
	

	implicitEuler(model, timeStep);
	QTime timer;
	wfMesh * myMesh = model->getMesh()->getWfMesh();
	buffer = myMesh->getVertices();
	timer.start();
	cpuCSRMatrix & laplace = model->getLaplace0_mixed();
	cout << "recalc L0:" <<timer.restart()<< "\n";
	laplace.mult(model->getMesh()->getWfMesh()->getVertices(),buffer);

	float maxNorm, minNorm, norm;
	maxNorm = 0;
	minNorm = buffer[0].norm();
	for(int i = 0; i < buffer.size(); i++){
		norm = buffer[i].norm();
		if(norm>maxNorm){
			maxNorm = norm;
		}
		if(norm < minNorm){
			minNorm = norm;
		}
	}

	//forward integration step:
	float scale= meshMath::volume(*(model->getMesh()));
	myMesh->translateVertices(buffer, -timeStep);
	scale= pow(scale/meshMath::volume(*(model->getMesh())),1.f/3);
	myMesh->scaleVertices(scale);

//	myMesh->normalize();


	cout << "aftermath: " <<timer.restart()<< "\n";
	myMesh->updateObserver(POS_CHANGED);
}

void application_smoothing::implicitEuler( MODEL * model, float timeStep )
{

	wfMesh * myMesh = model->getMesh()->getWfMesh();
	buffer = myMesh->getVertices();

	cpuCSRMatrix Id_min_t_laplace = model->getLaplace0_mixed();
	Id_min_t_laplace*=timeStep;
	Id_min_t_laplace.addOnDiagonal(1.f);


	floatVector b(myMesh->getVertices(), 0);
	floatVector x = b;
	mySolver solver;
	solver.setMatrix(Id_min_t_laplace);
	solver.solve(x,b);


}
