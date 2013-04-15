#pragma once
#include "solverif.h"
#include "DDGMatrices.h"
#include "cholmod.h"
#include "SolverConfig.h"

/************************************************************************/
/* Implements the solver interface, using the opensource
/* SuiteSparse libraries, see the corresponding licencing and copyright
/* terms.
/* The Cholmod library provided is supposedly good for symmetric positive 
/" definite problems, while Umfpack is used to handle any more general matrices
/************************************************************************/

#ifdef SUITESPARSESOLVER
class SuiteSparseSolver :
	public SolverIF
{
public:
	SuiteSparseSolver(MatrixType type);
	virtual ~SuiteSparseSolver(void);

	// As suite sparse uses double precision opposed to the float cpuCSRMatrix,
	//an internal (double precision) copy of the matrix is created and kept.
	//rather inefficient... Change CPUCSR implementation if you want.
	virtual void setMatrix( cpuCSRMatrix & mat);

	virtual void preconditionSystem();

	virtual void solve( floatVector & target, floatVector & b );

private:
	enum SolverType {CHOLMOD, UMFPACK, NONE};
	SolverType solver;
	MatrixType matType;

	//////////////////////////////////////////////////////////////////////////
	//umfpack stuff
	void *umf_Symbolic, *umf_Numeric;
	double *umf_Control, *umf_Info;
	//internal matrix representation, B, X vector
	int *Ap, *Ai,n,m;
	std::vector<double> Ax, B, X;

	//system type
	int umf_systype;

	//////////////////////////////////////////////////////////////////////////
	//Cholmod stuff
	cholmod_common c;
	cholmod_sparse *mat_cholmod;
	cholmod_factor  *L;
	cholmod_dense *b;
};


#endif //SUITESPARSeSOLVER