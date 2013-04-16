/************************************************************************/
/* Configure which Solver to use
/* At the moment there are wrappers for two libraries implemented. The library files needed
/* are not provided with this project, they need to be compiled/downloaded separately.
/* The two libraries are:
/*
/* 1. the Suitesparse Library, which is open source but a real pain to compile
/* 2. The Solver from the Pardiso project, which is shareware with a 1-year academic
/* license as well as a one-month trial license, and can be downloaded precompiled on
/*
/* Using the config file solvers can be created transparently using th
/* syntax: new solverInstance(MatrixType type), which returns SolverIf *
/* of the configured type.
/************************************************************************/
#pragma once

//un-comment depending of the solver you want to use
#define PARDISOSOLVER
//#define SUITESPARSESOLVER


#ifdef PARDISOSOLVER
#define solverInstance PardisoSolver  
#include "PardisoSolver.h"
#endif //PARDISOSOLVER

#ifdef SUITESPARSESOLVER
#define solverInstance SuiteSparseSolver
#include "SuiteSparseSolver.h"
#endif//SUITESPARSESOLVER
