// DEC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "tuple3.h"
#include "wfmesh.h"
#include "wingedMesh.h"
#include "ATestClassForObserver.h"

#include "OBIFileReader.h"
#include "ball.h"

#include "cpuCSRMatrix.h"
#include "DDGMatrices.h"

#include "kernel.h"
#include "tests.h"


int _tmain(int argc, _TCHAR* argv[])
{

	tests::testAndSaveDECMatrices();
	//tests::testWfWingedMesh();
	//tests::testCudaCusp();

	return 0;
}

