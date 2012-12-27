#include "application_smoothing.h"


application_smoothing::application_smoothing(wingedMesh & m)
{
	Laplace0_mixed = DDGMatrices::coderiv1_mixed(m, std::vector<float>())* DDGMatrices::d0(m);
	myMesh = &m;
}


application_smoothing::~application_smoothing(void)
{
}

void application_smoothing::explicitEuler(MODEL * model, float timeStep)
{
	buffer = myMesh->getVertices();
	model->getLaplace0_mixed().mult(model->getMesh()->getWfMesh()->getVertices(),buffer);
	
	myMesh->getWfMesh()->translateVertices(buffer, -timeStep);
	myMesh->getWfMesh()->normalize();
	myMesh->getWfMesh()->updateObserver(POS_CHANGED);
}
