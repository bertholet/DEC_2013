#pragma once
#include "CUDASOLVER_EXPORT.h"
//#include "device_launch_parameters.h"
#include <cusp/csr_matrix.h>
#include "cpuCSRMatrix.h"


/************************************************************************/
/* Simple solver interface:
/* Every Solver can contain exactly one matrix, the matrix is
/************************************************************************/


class mySolver{
public:
	mySolver(){}
	~mySolver(){}
	
	//test function, to delete
	int  CUDASOLVER_EXPORT main();
	
	
	void CUDASOLVER_EXPORT setMatrix(cpuCSRMatrix & mat);
	void CUDASOLVER_EXPORT updateMatrix();
	void CUDASOLVER_EXPORT solve(double *x, double * b);

	static void CUDASOLVER_EXPORT checkMatrix(cpuCSRMatrix & mat );
private:
	cusp::csr_matrix<int,float,cusp::device_memory> * myMatrix;
	//cudaError_t addWithCuda_(int *c, const int *a, const int *b, size_t size);
	//__global__ void addKernel_(int *c, const int *a, const int *b);

};


