/************************************************************************/
/* Configure which Solver to use
/* actually there are wrappers for two libraries implemented, the library files needed
/* are not provided with this project, they need to be compiled/downloaded separately:
/*
/* 1. the Suitesparse Library, which is open source but a real pain to compile
/* 2. The Solver from the Pardiso project, which is shareware with a 1-year academic
/* license as well as a one-month trial license, and can be downloaded precompiled on
/*
/************************************************************************/
#pragma once

//un-comment depending of the solver you want to use
#define PARDISOSOLVER
//#define SUITESPARSESOLVER


//syntax: new solverInstance(MatrixType type), returns SolverIf *.
#ifdef PARDISOSOLVER
#define solverInstance PardisoSolver  
#include "PardisoSolver.h"
#endif //PARDISOSOLVER

#ifdef SUITESPARSESOLVER
#define solverInstance SuiteSparseSolver
#include "SuiteSparseSolver.h"
#endif//SUITESPARSESOLVER
