#include "MODEL.h"
#include "ball.h"
#include "DDGMatrices.h"

MODEL * MODEL::instance = 0;
MODEL::MODEL(void)
{

	myMesh = new wingedMesh(new ball(1, 10,5));
	myMesh->getWfMesh()->attach(this);

	d0 = d1 = star0 = star1 = star0_mixed = 
		star1_mixed = laplace0_mixed = coderiv1_mixed = 
		NULL;
	invalidateAll();
}


MODEL::~MODEL(void)
{
	if(myMesh!= NULL){
		delete myMesh;
	}
	freeAll();
}

MODEL * MODEL::getModel()
{
	if(MODEL::instance == NULL){
		MODEL::instance = new MODEL();
	}
	return MODEL::instance;
}

void MODEL::setMesh( wingedMesh * aMesh )
{
	if(myMesh != NULL){
		delete myMesh;
	}

	myMesh = aMesh;
	aMesh->getWfMesh()->attach(this);
	
	//this->updateObserver(NEW_MESH_CREATED);
}

wingedMesh* MODEL::getMesh()
{
	return myMesh;
}

void MODEL::loadMesh( const char* file )
{
	wfMesh * amesh = new wfMesh(file);

	wingedMesh * wingedM = new wingedMesh(amesh);

	//model takes care of the memory management of mesh and wingedMesh
	//should encapsulate creation in Model!!!!
	this->setMesh(wingedM);
	invalidateAll();

}

void MODEL::update( void * src, meshMsg msg )
{
	if(msg == CONNECTIVITY_CHANGED){
		freeAll();
		//invalidateAll();
	}
	else if(msg == POS_CHANGED){
		unrefreshedStars();
	}

}

void MODEL::freeAll()
{
	if(d0_valid!= INVALID){
		delete d0;
	}
	if(d1_valid!= INVALID){
		delete d1;
	}
	if(star0_valid!= INVALID){
		delete star0;
	}
	if(star1_valid!= INVALID){
		delete star1;
	}
	if(star0_mixed_valid!= INVALID){
		delete star0_mixed;
	}
	if(star1_mixed_valid!= INVALID){
		delete star1_mixed;
	}
	if(laplace0_mixed_valid!= INVALID){
		delete laplace0_mixed;
	}
	if(coderiv1_mixed_valid!= INVALID){
		delete coderiv1_mixed;
	}

	invalidateAll();
}

void MODEL::invalidateAll()
{
	d0_valid = 
		d1_valid = 
		star0_valid = 
		star1_valid = 
		star0_mixed_valid =  
		star1_mixed_valid = 
		coderiv1_mixed_valid =
		laplace0_mixed_valid =INVALID;
}

void MODEL::unrefreshedStars()
{
	star0_valid = (star0_valid != INVALID? UNREFRESHED: INVALID);
	star1_valid = (star1_valid != INVALID? UNREFRESHED: INVALID);
	star0_mixed_valid =  (star0_mixed_valid != INVALID? UNREFRESHED: INVALID);
	star1_mixed_valid = (star1_mixed_valid != INVALID? UNREFRESHED: INVALID);
	laplace0_mixed_valid = (laplace0_mixed_valid != INVALID? UNREFRESHED: INVALID);
	coderiv1_mixed_valid = (coderiv1_mixed_valid != INVALID? UNREFRESHED: INVALID);
}

cpuCSRMatrix & MODEL::getLaplace0_mixed()
{
	if(laplace0_mixed_valid == VALID){
		return *laplace0_mixed;
	}
	else{
		laplace0_mixed = new cpuCSRMatrix();
		cpuCSRMatrix & cod = getCoderiv1_mixed();
		cpuCSRMatrix & d = getD0();
		*laplace0_mixed =cod*d;
		laplace0_mixed_valid = VALID;
		return *laplace0_mixed;
	}

}

cpuCSRMatrix & MODEL::getCoderiv1_mixed()
{
	if(coderiv1_mixed_valid == VALID){
		return *coderiv1_mixed;
	}
	else{
		if(coderiv1_mixed_valid == INVALID){
			coderiv1_mixed = new cpuCSRMatrix();
		}
		*coderiv1_mixed = DDGMatrices::coderiv1_mixed(*myMesh, std::vector<float>());
		coderiv1_mixed_valid = VALID;
		return *coderiv1_mixed;
	}
}

cpuCSRMatrix & MODEL::getD0()
{
	if(d0_valid == VALID){
		return *d0;
	}
	else{
		d0 = new cpuCSRMatrix();
		*d0 = DDGMatrices::d0(*myMesh);
		d0_valid = VALID;
		return *d0;
	}
}


