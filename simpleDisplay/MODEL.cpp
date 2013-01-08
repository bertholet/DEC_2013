#include "MODEL.h"
#include "ball.h"
#include "DDGMatrices.h"
#include <QTime>

MODEL * MODEL::instance = 0;
MODEL::MODEL(void)
{

	myMesh = new wingedMesh(new ball(1, 10,5));
	myMesh->getWfMesh()->attach(this);

	d0 = d1 = border1 =star0 = star1 = star0_mixed = 
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
	amesh->normalize();
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
	if(id0_valid!= INVALID){
		delete id0;
	}
	if(border1_valid!= INVALID){
		delete border1;
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
		border1_valid=
		id0_valid =
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
		if(laplace0_mixed_valid== INVALID){
			laplace0_mixed = new cpuCSRMatrix();
		}
		cout<< "**************Laplace********\n";
		QTime timer;
		timer.start();
		cpuCSRMatrix & cod = getCoderiv1_mixed();
		cout << "coderivative: " << timer.restart()<<"\n";
		cpuCSRMatrix & d = getD0();
		cout << "D0: " << timer.restart()<<"\n";
		*laplace0_mixed =cod*d;
		cout << "cod*D0: " << timer.restart()<<"\n";
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
		//*coderiv1_mixed = DDGMatrices::coderiv1_mixed(*myMesh, std::vector<float>(), getBorder1());
		QTime timer;
		timer.start();
		DDGMatrices::coderiv1(getStar1_mixed(),getBorder1(),getStar0_mixed(),*coderiv1_mixed);
		cout << "coderivative: " << timer.restart()<<"\n";
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
		if(d0_valid == INVALID){
			d0 = new cpuCSRMatrix();
		}
		QTime timer;
		timer.start();
		*d0 = DDGMatrices::d0(*myMesh);
		cout << "* D0 creation: " << timer.restart()<<"\n";
		d0_valid = VALID;
		return *d0;
	}
}

cpuCSRMatrix & MODEL::getBorder1()
{
	if(border1_valid == VALID){
		return *border1;
	}
	else{
		QTime timer;
		timer.start();
		if(border1_valid == INVALID){
			border1 = new cpuCSRMatrix();
		}
		
		*border1 = DDGMatrices::border1(*myMesh);//cpuCSRMatrix::transpose(getD0());

		cout << "*Border1 created: " << timer.restart() <<"\n";
		border1_valid = VALID;
		return *border1;
	}
}

cpuCSRMatrix & MODEL::getStar1_mixed()
{
	if(star1_mixed_valid == VALID){
		return *star1_mixed;
	}
	else{
		QTime timer;
		timer.start();

		if(star1_mixed_valid == INVALID){
			star1_mixed = new cpuCSRMatrix();
		}
		*star1_mixed = DDGMatrices::star1_mixed(*myMesh, std::vector<float>());
		cout << "*star1 created: " << timer.restart() <<"\n";
		star1_mixed_valid = VALID;
		return *star1_mixed;
	}
}

cpuCSRMatrix & MODEL::getStar0_mixed()
{
	if(star0_mixed_valid == VALID){
		return *star0_mixed;
	}
	else{
		QTime timer;
		timer.start();
		if(star0_mixed_valid == INVALID){
			star0_mixed = new cpuCSRMatrix();
		}

		*star0_mixed = DDGMatrices::star0_mixed(*myMesh, std::vector<float>());
		cout << "*star0 created: " << timer.restart() <<"\n";
		star0_mixed_valid = VALID;
		return *star0_mixed;
	}
}


