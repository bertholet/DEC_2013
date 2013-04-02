#include "SuiteSparseSolver.h"
#include "cholmod.h"
#include "umfpack.h"
#include "assert.h"
#include <vector>
#include <stdio.h>

SuiteSparseSolver::SuiteSparseSolver(MatrixType type):SolverIF(type)
{
	matType = type;

	if(type == SolverIF::MATRIX_SYMMETRIC){
		solver = CHOLMOD;
		cholmod_start(&c);
		cholmod_print_common("Cholmod setup", & c);
		c.print = 3;
		mat_cholmod = NULL;
		L = NULL;
	}
	else{
		solver = UMFPACK;
		umf_Symbolic = umf_Numeric = NULL;
		umf_Control= new double[UMFPACK_CONTROL], umf_Info = new double[UMFPACK_INFO];
		b = NULL;

		umfpack_di_defaults(umf_Control);
		//umf_Control[UMFPACK_PRL] =2; //print information
		umfpack_di_report_control(umf_Control);
	}
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
		if(mat_cholmod != NULL){
			cholmod_free_sparse(&mat_cholmod, &c);
		}
		if(L != NULL){
			cholmod_free_factor(&L, &c);
		}
		if(b != NULL){
			cholmod_free_dense(&b, &c);
		}
		cholmod_finish(&c);
		
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
		assert(n == m);
	//allocate matrix
		mat_cholmod = cholmod_allocate_sparse(n,m,nz,true,true,1,CHOLMOD_REAL, &c);
		
		//initialize matrix
		//the internal type of mat_cholmod is double, it is ugly, but
		//the internal (void* typed) arrays need to be initialized by hand...
		double * Ax = (double*) mat_cholmod->x; //values
		int * Ap = (int*) mat_cholmod->p; //column /row pointers (matrix symmetric so nvm)
		int * Ai = (int *) mat_cholmod->i;
		
		for(unsigned int i = 0; i < mat.geta().size(); i++){
			Ax[i] = 0.0 + mat.geta()[i];
		}
		for(unsigned int i = 0; i < mat.getia().size(); i++){
			Ap[i] = mat.getia()[i];
		}
		for(unsigned int i = 0; i < mat.getja().size(); i++){
			Ai[i] = mat.getja()[i];
		}

		cout << "Checking matrix : (";
		cout << (cholmod_check_sparse(mat_cholmod, &c)? "OK": "NO!");
		cout << ")\n\n";
		cholmod_print_sparse(mat_cholmod, "Info about matrix", &c);


		b= cholmod_allocate_dense(n,1,n,mat_cholmod->xtype,&c);
		cout << "Checking allocated b : (";
		cout << (cholmod_check_dense(b, &c)? "OK": "NO!");
		cout << ")\n\n";
	}

}

void SuiteSparseSolver::preconditionSystem()
{
	if(solver == UMFPACK){
		int status = umfpack_di_symbolic(m,n, Ap, Ai, &(Ax[0]), &umf_Symbolic,umf_Control, umf_Info);
		status = umfpack_di_numeric(Ap, Ai, &(Ax[0]), umf_Symbolic, &umf_Numeric, umf_Control, umf_Info);
		umfpack_di_report_status(umf_Control, status);
	}
	else if(solver == CHOLMOD){
		L =cholmod_analyze(mat_cholmod, &c);
		cholmod_factorize(mat_cholmod, L, &c);

		if(!cholmod_check_common( & c)){
			cholmod_print_common("****Cholmod status Error \n\n", & c);
		}
	}
}

void SuiteSparseSolver::solve( floatVector & target, floatVector & b_ )
{
	if(solver == UMFPACK){
		assert(b_.size() == B.size());
		for(unsigned int i = 0; i < b_.size(); i++){
			B[i] = 0.0 + b_[i];
		}

		int status = umfpack_di_solve(umf_systype, Ap, Ai, &(Ax[0]), &(X[0]), &(B[0]), umf_Numeric, umf_Control, umf_Info);
		umfpack_di_report_info(umf_Control, umf_Info);
		umfpack_di_report_status(umf_Control, status);

		target.resize(X.size());
		for(int i = 0; i < X.size(); i++){
			target[i] = X[i];
		}
	}
	else if (solver == CHOLMOD){
		//fill b
		double * b_val = (double *) b->x;
		assert(b_.size() ==  b->nrow);
		for(unsigned int i  = 0; i < b_.size(); i++){
			b_val[i] = 0.0 + b_[i];
		}
		cholmod_dense * x = cholmod_solve(CHOLMOD_A,L, b, &c);

		
		double * x_val = (double *) x->x;
		target.resize(b_.size());
		for(unsigned int i  = 0; i < b_.size(); i++){
			target[i] = x_val[i];
		}
		cholmod_free_dense(&x, &c);

		if(!cholmod_check_common( & c)){
			cholmod_print_common("****Cholmod status Error \n\n", & c);
		}

	}
}
