#include "application_smoothing.h"


application_smoothing::application_smoothing()
{
}


application_smoothing::~application_smoothing(void)
{
}

void application_smoothing::explicitEuler(MODEL * model, float timeStep)
{
	wfMesh * myMesh = model->getMesh()->getWfMesh();
	buffer = myMesh->getVertices();
	cpuCSRMatrix & the_other_laplace = model->getLaplace0_mixed();
	model->getLaplace0_mixed().mult(model->getMesh()->getWfMesh()->getVertices(),buffer);
	
	myMesh->translateVertices(buffer, -timeStep);
	myMesh->normalize();
	myMesh->updateObserver(POS_CHANGED);
}
