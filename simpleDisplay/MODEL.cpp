#include "MODEL.h"
#include "ball.h"
#include "DDGMatrices.h"
#include <QTime>

MODEL * MODEL::instance = 0;
MODEL::MODEL(void)
{
	wfMesh * amesh = new wfMesh("../Objfiles/obstacleTiny.obj");
	amesh->normalize();
	myMesh = new wingedMesh(amesh);
	
	//myMesh = new wingedMesh(new ball(1, 10,5));
	myMesh->getWfMesh()->attach(this);
	
	d0 = d1 = border1 =border2
		=duald1 = duald1_t
		=id0 
		=star0 = star1 = star2 =
		star0_mixed = star1_mixed = 
		coderiv1_mixed = coderiv1_ignoreBorder = 
		laplace0_mixed = laplace0_ignoreBorder=
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

void MODEL::freeModel()
{
	delete MODEL::instance;
	MODEL::instance = NULL;
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
	if(d1_valid!= INVALID){
		delete d1;
	}
	if(id0_valid!= INVALID){
		delete id0;
	}
	if(border1_valid!= INVALID){
		delete border1;
	}
	if(border2_valid!= INVALID){
		delete border2;
	}
	if(duald1_valid != INVALID){
		delete duald1;
	}
	if(duald1_t_valid != INVALID){
		delete duald1_t;
	}
	if(star0_valid!= INVALID){
		delete star0;
	}
	if(star1_valid!= INVALID){
		delete star1;
	}
	if(star2_valid!= INVALID){
		delete star2;
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
	if(laplace0_ignoreBorder_valid!= INVALID){
		delete laplace0_ignoreBorder;
	}
	if(coderiv1_ignoreBorder_valid!= INVALID){
		delete coderiv1_ignoreBorder;
	}
	invalidateAll();
}

void MODEL::invalidateAll()
{
	d0_valid = 
		d1_valid = 
		border1_valid=
		border2_valid =
		duald1_valid =
		duald1_t_valid =
		id0_valid =
		star0_valid = 
		star1_valid = 
		star2_valid =
		star0_mixed_valid =  
		star1_mixed_valid = 
		coderiv1_mixed_valid =
		coderiv1_ignoreBorder_valid=
		laplace0_ignoreBorder_valid =
		laplace0_mixed_valid =INVALID;
}

void MODEL::unrefreshedStars()
{
	star0_valid = (star0_valid != INVALID? UNREFRESHED: INVALID);
	star1_valid = (star1_valid != INVALID? UNREFRESHED: INVALID);
	star0_mixed_valid =  (star0_mixed_valid != INVALID? UNREFRESHED: INVALID);
	star1_mixed_valid = (star1_mixed_valid != INVALID? UNREFRESHED: INVALID);
	star2_valid = (star2_valid != INVALID? UNREFRESHED: INVALID);
	laplace0_ignoreBorder_valid = (laplace0_ignoreBorder_valid != INVALID? UNREFRESHED: INVALID);
	laplace0_mixed_valid = (laplace0_mixed_valid != INVALID? UNREFRESHED: INVALID);
	coderiv1_mixed_valid = (coderiv1_mixed_valid != INVALID? UNREFRESHED: INVALID);
	coderiv1_ignoreBorder_valid =(coderiv1_ignoreBorder_valid != INVALID? UNREFRESHED: INVALID);
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


cpuCSRMatrix & MODEL::getD1()
{
	if(d1_valid == VALID){
		return *d1;
	}
	else{
		if(d1_valid == INVALID){
			d1 = new cpuCSRMatrix();
		}
		QTime timer;
		timer.start();
		*d1 = DDGMatrices::d1(*myMesh);
		cout << "* D1 creation: " << timer.restart()<<"\n";
		d1_valid = VALID;
		return *d1;
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

cpuCSRMatrix & MODEL::getBorder2()
{
	if(border2_valid == VALID){
		return *border2;
	}
	else{
		QTime timer;
		timer.start();
		if(border2_valid == INVALID){
			border2 = new cpuCSRMatrix();
		}

		*border2 = DDGMatrices::border2(*myMesh);//cpuCSRMatrix::transpose(getD0());

		cout << "*Border2 created: " << timer.restart() <<"\n";
		border2_valid = VALID;
		return *border2;
	}
}


cpuCSRMatrix & MODEL::getDualD1()
{
	//the dual derivative D1 is minus the border1 matrix.
	if(duald1_valid == VALID){
		return *duald1;
	}
	else{
		if(duald1_valid == INVALID){
			duald1 = new cpuCSRMatrix();
		}

		*duald1 = getBorder1();
		*duald1 *=-1;

		cout << "*Duald1 created from border1 "<<"\n";
		duald1_valid = VALID;
		return *duald1;
	}
}


cpuCSRMatrix & MODEL::getDualD1_T()
{
	//the dual derivative D1 transposed is minus the d0 matrix.
	if(duald1_t_valid == VALID){
		return *duald1_t;
	}
	else{
		if(duald1_t_valid == INVALID){
			duald1_t = new cpuCSRMatrix();
		}

		*duald1_t = getD0();
		*duald1_t *=-1;

		cout << "*Duald1_t created from d0 "<<"\n";
		duald1_t_valid = VALID;
		return *duald1_t;
	}
}


cpuCSRMatrix & MODEL::getStar0()
{
	if(star0_valid == VALID){
		return *star0;
	}
	else{
		QTime timer;
		timer.start();

		if(star0_valid == INVALID){
			star0 = new cpuCSRMatrix();
		}
		*star0 = DDGMatrices::star0(*myMesh);
		cout << "*star0 created: " << timer.restart() <<"\n";
		star0_valid = VALID;
		return *star0;
	}
}


cpuCSRMatrix & MODEL::getStar1()
{
	
	if(star1_valid == VALID){
		return *star1;
	}
	else{
		QTime timer;
		timer.start();

		if(star1_valid == INVALID){
			star1 = new cpuCSRMatrix();
		}
		*star1 = DDGMatrices::star1(*myMesh);
		cout << "*star1 created: " << timer.restart() <<"\n";
		star1_valid = VALID;
		return *star1;
	}
}

cpuCSRMatrix & MODEL::getStar2()
{
	if(star2_valid == VALID){
		return *star2;
	}
	else{
		QTime timer;
		timer.start();

		if(star2_valid == INVALID){
			star2 = new cpuCSRMatrix();
		}
		*star2 = DDGMatrices::star2(*myMesh);
		cout << "*star2 created: " << timer.restart() <<"\n";
		star2_valid = VALID;
		return *star2;
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


cpuCSRMatrix & MODEL::getCoderiv1_ignoreBoundary()
{
	if(coderiv1_ignoreBorder_valid == VALID){
		return *coderiv1_ignoreBorder;
	}
	else{
		QTime timer;
		timer.start();
		if(coderiv1_ignoreBorder_valid == INVALID){
			coderiv1_ignoreBorder = new cpuCSRMatrix();
		}

		*coderiv1_ignoreBorder = DDGMatrices::coderiv1_mixed_ignoreBoundary(*myMesh);
		cout << "*coderiv1_ignoreBorder created: " << timer.restart() <<"\n";
		coderiv1_ignoreBorder_valid = VALID;
		return *coderiv1_ignoreBorder;
	}
}

cpuCSRMatrix & MODEL::getLaplace0_ignoreBoundary()
{
	if(laplace0_ignoreBorder_valid == VALID){
		return *laplace0_ignoreBorder;
	}
	else{
		QTime timer;
		timer.start();
		if(laplace0_ignoreBorder_valid == INVALID){
			laplace0_ignoreBorder = new cpuCSRMatrix();
		}

		*laplace0_ignoreBorder = getCoderiv1_ignoreBoundary() * getD0();
		cout << "*laplace0_ignoreBoundary created: " << timer.restart() <<"\n";
		laplace0_ignoreBorder_valid = VALID;
		return *laplace0_ignoreBorder;
	}
}

cpuCSRMatrix & MODEL::getD0_T()
{
	return getBorder1();
}

cpuCSRMatrix & MODEL::getD1_T()
{
	return getBorder2();
}






