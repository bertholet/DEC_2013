#pragma once
#include "SolverIF.h"
#include "SolverConfig.h"

#ifdef PARDISOSOLVER
#define PARDISO_REFINEMENTSTEPS 3
//0 is direct, 1 is iterative.
#define PARDISO_SOLVERTYPE 0

class PardisoSolver:
	public SolverIF
{
private:
	int int_params[128];
	double double_params[128];
	void* intern_memory[128];

	//the b, x and values a of the matrix, which have to be of type double
	std::vector<double> b,x, a;
	//the ia, ja values of the matrix, ays they have to be 1-based for pardiso...
	std::vector<int> ia,ja;
	//other matrix types do exist, see manual.
	static const int MT_ANY=11,MT_SYMMETRIC=-2,MT_STRUCTURALLY_SYMMETRIC=1;

	cpuCSRMatrix * matrix;
	int matrix_type;
	int error;
	int print_stats;
	//In principle pardiso could solve for more than one righthandside.
	int nrhs;

	void init_intParams(int nrRefinementSteps);
	void checkError_init();
	void checkMatrix( int matrix_type, std::vector<double> & a, std::vector<int> & ia, std::vector<int> & ja );

public:

	PardisoSolver(MatrixType type);
	~PardisoSolver(void);

	virtual void setMatrix( cpuCSRMatrix & mat );

	virtual void preconditionSystem();

	virtual void solve( floatVector & target, floatVector & b );

};

#endif //PARDISOSOLVER