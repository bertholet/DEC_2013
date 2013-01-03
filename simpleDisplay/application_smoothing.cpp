#include "application_smoothing.h"
#include <QTime>
#include "meshMath.h"

application_smoothing::application_smoothing()
{
}


application_smoothing::~application_smoothing(void)
{
}

void application_smoothing::explicitEuler(MODEL * model, float timeStep)
{
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

	float scale= meshMath::volume(*(model->getMesh()));
	myMesh->translateVertices(buffer, -timeStep);
	scale= pow(scale/meshMath::volume(*(model->getMesh())),1.f/3);
	myMesh->scaleVertices(scale);

//	myMesh->normalize();


	cout << "aftermath: " <<timer.restart()<< "\n";
	myMesh->updateObserver(POS_CHANGED);
}
