#pragma once
#include "cpuCSRMatrix.h"
#include "floatVector.h"

/************************************************************************/
/* Interface for a black box solver to be used in the DEC applications   
/* For every sparse linear system Ax = b a solver will have to be created,
/* on which solve can be called any number of times with different left
/* hand sides b
/* It has to be possible that more than one solver in use at once. */
/************************************************************************/
class SolverIF
{
public:
	enum MatrixType {MATRIX_SYMMETRIC, MATRIX_STRUCTURALLY_SYMMETRIC, MATRIX_UNSYMMETRIC};

	SolverIF(MatrixType type);
	virtual ~SolverIF(void);

	virtual void setMatrix(cpuCSRMatrix & mat) = 0;
	virtual void preconditionSystem() = 0;

	virtual void solve(floatVector & target, floatVector & b) = 0;
};

