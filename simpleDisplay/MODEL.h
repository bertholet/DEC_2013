#pragma once
#include "cpuCSRMatrix.h"
//#include "Observer.h"
#include "wingedMesh.h"


/*extern template Observer<meshMsg>;*/

/************************************************************************/
/* For simplicity the mesh actually under consideration is stored
/* as a singleton. Matrices are created lazily when demanded and then
/* kept in stored. Upon mesh creation, deletion, position change etc
/* the matrices are invalidated appropriately.
/************************************************************************/
class MODEL:public meshObserver
{
protected:
	MODEL(void);
public:
	static MODEL * getModel();
	~MODEL(void);

	wingedMesh* getMesh();

	//////////////////////////////////////////////////////////////////////////
	// The Memory Management of mesh is taken care of by model.
	// i.e. it is deleted at destruction or mesh change.
	//////////////////////////////////////////////////////////////////////////
	void setMesh(wingedMesh * aMesh);
	void loadMesh( const char* file );

	virtual void update( void * src, meshMsg msg );

	// Matrix management
	void freeAll();

	cpuCSRMatrix & getLaplace0_mixed();
	cpuCSRMatrix & getCoderiv1_mixed();
	cpuCSRMatrix & getD0();

private:
	static MODEL* instance;
	wingedMesh * myMesh;

	enum validity {INVALID, UNREFRESHED, VALID};
	cpuCSRMatrix *d0, *d1, *star0, *star1, 
		*star0_mixed, *star1_mixed, *laplace0_mixed, * coderiv1_mixed;
	//to track the validity of the stored matrices.
	validity d0_valid,d1_valid,star0_valid,star1_valid, 
		star0_mixed_valid, star1_mixed_valid, 
		laplace0_mixed_valid, coderiv1_mixed_valid;


	//matrix management
	void invalidateAll();
	void unrefreshedStars();

};

