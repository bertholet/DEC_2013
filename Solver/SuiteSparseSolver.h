#pragma once
#include "solverif.h"
#include "DDGMatrices.h"


/************************************************************************/
/* Implements the solver interface, using the opensource
/* SuiteSparse libraries, see the corresponding licencing and copyright
/* terms.
/* The Cholmod library provided is supposedly good for symmetric positive 
/" definite problems, while umfpack handles any more general matrices
/************************************************************************/
class SuiteSparseSolver :
	public SolverIF
{
public:
	SuiteSparseSolver(MatrixType type);
	virtual ~SuiteSparseSolver(void);

	// As suite sparse uses a compressed column format
	//as opposed to the compressed row format used in DECCORE
	//an internal ('transposed') copy of the matrix is created and kept.
	//might be too inefficient... Maybe this is not needed.
	//CHOLMOD is called only for symmetric matrices so it is ok anyway,
	//UMFPACK on the other hand can directly handle transposed matrices.
	virtual void setMatrix( cpuCSRMatrix & mat);

	virtual void preconditionSystem();

	virtual void solve( floatVector & target, floatVector & b );

private:
	enum SolverType {CHOLMOD, UMFPACK, NONE};
	SolverType solver;
	MatrixType matType;

	//umfpack stuff
	void *umf_Symbolic, *umf_Numeric;
	double *umf_Control, *umf_Info;
	//internal matrix representation, B, X vector
	int *Ap, *Ai,n,m;
	std::vector<double> Ax, B, X;

	//system type
	int umf_systype;
};

/*int main(int a, char  ** arg){
	
	floatVector lambdas(50);
	for(unsigned int i = 0; i < lambdas.size(); i++){
		lambdas[i] = 1 + (i % 7) *0.05f + (i%13) * 0.05f - (i % 5) * 0.1f;
	}
	cpuCSRMatrix testMat = DDGMatrices::diagMatrix(lambdas);


	floatVector x(lambdas.size()), b;
	b= lambdas;
	
	SuiteSparseSolver s(SolverIF::MATRIX_UNSYMMETRIC);
	s.setMatrix(testMat);
	s.preconditionSystem();
	s.solve(x,b);

}*/