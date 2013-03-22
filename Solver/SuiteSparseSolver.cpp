#include "SuiteSparseSolver.h"
#include "cholmod.h"
#include "umfpack.h"
#include "assert.h"
#include <vector>
#include <stdio.h>

SuiteSparseSolver::SuiteSparseSolver(MatrixType type):SolverIF(type)
{
	matType = type;

//	if(type == SolverIF::MATRIX_SYMMETRIC){
//		solver = CHOLMOD;
//	}
//	else{
		solver = UMFPACK;
		umf_Symbolic = umf_Numeric = NULL;
		//initialize umf_control parameters.
		umf_Control= new double[UMFPACK_CONTROL], umf_Info = new double[UMFPACK_INFO];

		umfpack_di_defaults(umf_Control);
		umf_Control[UMFPACK_PRL] =2;
		umfpack_di_report_control(umf_Control);
//	}
}


SuiteSparseSolver::~SuiteSparseSolver(void)
{

	if(solver == UMFPACK){
		delete[] umf_Control;
		delete[] umf_Info;
		if(umf_Symbolic!= NULL){
			umfpack_di_free_symbolic(&umf_Symbolic);
		}
		if(umf_Numeric != NULL){
			umfpack_di_free_numeric(&umf_Numeric);
		}
	}
	else if(solver == CHOLMOD){

	}
}

void SuiteSparseSolver::setMatrix( cpuCSRMatrix & mat )
{
	
	int  nz = mat.geta().size();
	n = mat.getn(), m = mat.getm();
	assert(mat.getia().size()== n+1);
	assert(mat.getja().size()== nz);

	//init B and X buffer.
	B.resize(n); X.resize(n);

	if(solver == UMFPACK){
	//initialize internal matrix
	//UMFPACK uses column compressed format, which is the transformed of csr
	//therefore solve the transposed.
		assert(umf_Numeric == NULL);
		assert(umf_Symbolic == NULL);
		umf_systype = UMFPACK_At;

		Ap = & mat.getia()[0], Ai = & mat.getja()[0];//Ap[n+1], Ai[nz];
		Ax.resize(nz); 
		for(int i = 0; i < nz; i++){
			Ax[i] = 0.0 + mat.geta()[i];
		}
	}
	else if(solver == CHOLMOD){

	}

}

void SuiteSparseSolver::preconditionSystem()
{
	int status = umfpack_di_symbolic(m,n, Ap, Ai, &(Ax[0]), &umf_Symbolic,umf_Control, umf_Info);
	status = umfpack_di_numeric(Ap, Ai, &(Ax[0]), umf_Symbolic, &umf_Numeric, umf_Control, umf_Info);
	umfpack_di_report_status(umf_Control, status);
}

void SuiteSparseSolver::solve( floatVector & target, floatVector & b )
{
	assert(b.size() == B.size());
	for(unsigned int i = 0; i < b.size(); i++){
		B[i] = 0.0 + b[i];
	}
	int status = umfpack_di_solve(umf_systype, Ap, Ai, &(Ax[0]), &(X[0]), &(B[0]), umf_Numeric, umf_Control, umf_Info);
	umfpack_di_report_info(umf_Control, umf_Info);
	umfpack_di_report_status(umf_Control, status);

	target.resize(X.size());
	for(int i = 0; i < X.size(); i++){
		target[i] = X[i];
	}
}
