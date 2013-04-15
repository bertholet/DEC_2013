#include "PardisoSolver.h"
#include <Windows.h>


#ifdef PARDISOSOLVER
/*
* Prototypes for the dll methods of the pardiso solver.
*
*/
extern "C" __declspec(dllimport) void pardisoinit (void   *, int    *,   int *, int *, double *, int *);
extern "C" __declspec(dllimport) void pardiso     (void   *, int    *,   int *, int *,    int *, int *, 
	double *, int    *,    int *, int *,   int *, int *,
	int *, double *, double *, int *, double *);
extern "C" __declspec(dllimport) void pardiso_chkmatrix  (int *, int *, double *, int *, int *, int *);
extern "C" __declspec(dllimport) void pardiso_chkvec     (int *, int *, double *, int *);
extern "C" __declspec(dllimport) void pardiso_printstats (int *, int *, double *, int *, int *, int *,
	double *, int *);



PardisoSolver::PardisoSolver(SolverIF::MatrixType type):
	SolverIF(type)
{
	int solver = PARDISO_SOLVERTYPE;
	matrix = NULL; 
	nrhs = 1;
	error = 0;
	print_stats = 0;
	matrix_type = (type == SolverIF::MATRIX_STRUCTURALLY_SYMMETRIC?MT_STRUCTURALLY_SYMMETRIC: 
					(type == SolverIF::MATRIX_SYMMETRIC? MT_SYMMETRIC:
					MT_ANY));

	init_intParams(PARDISO_REFINEMENTSTEPS);
	pardisoinit(intern_memory, &matrix_type, &solver, int_params, double_params, &error);
	checkError_init();

	//matrixWasSet = false;
}


void PardisoSolver::init_intParams( int nrRefinementSteps )
{
	SYSTEM_INFO sysinfo; 	GetSystemInfo( &sysinfo );  	
	int num_procs = sysinfo.dwNumberOfProcessors;    
	int_params[0] = 0; //use default params..
	int_params[2]  = num_procs;

	//all parameters have to be set after init!. (but for param 2 = num_procs)
	//int_params[3] = 91;	//hmhmhm
	int_params[7] = nrRefinementSteps;//1       /* Max numbers of iterative refinement steps. */
	int_params[6] = 0;	//int_params[6] = 1; then the result is placed in b.
	int_params[32] = 1; // compute determinant 
}


void PardisoSolver::checkError_init()
{
	if (error != 0) 
	{
		if (error == -10 )
			printf("No license file found \n");
		else if (error == -11 )
			printf("License is expired \n");
		else if (error == -12 )
			printf("Wrong username or hostname \n");
		else
			printf("Error %d has occurred\n", error);

	}
	else
		printf("[PARDISO]: License check was successful ... \n");
}


PardisoSolver::~PardisoSolver(void)
{
	int phase = -1;

	int maxfct =1; /*max nr of factorizations*/
	int mnum =1; /* Which factorization to use. */
	int n = matrix->dim();

	if(matrix->geta().size() > 0){
		int idumm;
		pardiso (intern_memory, &maxfct, &mnum, &matrix_type, &phase,
			&n, &a[0], &matrix->getia()[0], &matrix->getja()[0], &idumm, &nrhs,
			int_params, &print_stats, NULL, NULL, &error, double_params);

	}
	else{
		double ddumm;
		int idumm;
		pardiso (intern_memory, &maxfct, &mnum, &matrix_type, &phase,
			&n, &ddumm, &matrix->getia()[0], &idumm, &idumm, &nrhs,
			int_params, &print_stats, &ddumm, &ddumm, &error, double_params);
	}


	if(error != 0){
		throw std::runtime_error("Exception in pardiso solve-");
	}
}

void PardisoSolver::setMatrix( cpuCSRMatrix & mat )
{
	assert(matrix == NULL);
	assert(mat.getn() == mat.getm());
	matrix = &mat;

	//prepare the internal x,b, and a variables
	x.assign(mat.getn(),0);
	b = x;
	a.resize(mat.geta().size());
	for(int i = 0; i < a.size(); i++){
		a[i] = 0.0 + mat.geta()[i];
	}

	if(mat.geta().size() > 0){
		//checkMatrix(matrix_type, mat);

		//factorization: symbolic and numerical
		int phase = 12;
		int maxfct =1; /*max nr of factorizations*/
		int mnum =1; /* Which factorization to use. */
		int n = mat.dim();

		pardiso (intern_memory, &maxfct, &mnum, &matrix_type, &phase,
			&n, &a[0], &mat.getia()[0], &mat.getja()[0], NULL, &nrhs,
			int_params, &print_stats, NULL, NULL, &error, double_params);
	}
	else{
		//dummy initialization
		int phase = 12;
		int maxfct =1; 
		int mnum =1; 
		int n = mat.dim();
		double ddumm;
		int idumm;
		pardiso (intern_memory, &maxfct, &mnum, &matrix_type, &phase,
			&n, &ddumm, &mat.getia()[0], &idumm, NULL, &nrhs,
			int_params, &print_stats, NULL, NULL, &error, double_params);
	}
	if(error != 0){
		checkMatrix(this->matrix_type, mat);
		throw std::runtime_error("Exception in pardiso solve-");
	}

}

void PardisoSolver::preconditionSystem()
{
	//already done.
}

void PardisoSolver::solve( floatVector & target, floatVector & b_ )
{
	assert(matrix->getn() == matrix->getm());
	int phase = 33;
	int maxfct =1; /*max nr of factorizations*/
	int mnum =1; /* Which factorization to use. */
	int n = matrix->dim();

	for(int i = 0; i < b_.size(); i++){
		b[i] = 0.0 + b_[i];
	}

	error = 0;
	pardiso (intern_memory, &maxfct, &mnum, &matrix_type, &phase,
		&n, &a[0], &matrix->getia()[0], &matrix->getja()[0], NULL, &nrhs,
		int_params, &print_stats, &b[0], &x[0], &error, double_params);

	if(error != 0){
		throw std::runtime_error("Exception in pardiso solve-");
	}

	for(int i = 0; i < target.size(); i++){
		target[i] = x[i];
	}

}

void PardisoSolver::checkMatrix( int matrix_type, cpuCSRMatrix & mat )
{
	int n = mat.dim();
	int err;
	pardiso_chkmatrix  (&matrix_type, &n, & a[0], & mat.getia()[0], 
		& mat.getja()[0], &err);

	if (err != 0) {
		printf("\nERROR in consistency of matrix: %d", err);
	}
}

#endif //PARDISOSOLVER