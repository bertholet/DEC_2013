#include "StdAfx.h"
#include "tests.h"
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

tests::tests(void)
{
}


tests::~tests(void)
{
}

void tests::testAndSaveDECMatrices()
{

	wfMesh * myMesh = new wfMesh("../objfiles/plane_32.obj");


//	*myMesh = ball(1,3,2);

	wingedMesh wingedM(myMesh);


	std::cout << "Hello, creating d0 matrix\n";
	cpuCSRMatrix d0_matrix = DDGMatrices::d0(wingedM);
	std::cout << "Saving matrix\n";
	d0_matrix.saveMatrix("d0_.m");
	std::cout << "Size is : " << d0_matrix.getm() << ", " << d0_matrix.getn() <<"\n\n";

	std::cout << "Hello, creating d1 matrix\n";
	cpuCSRMatrix d1_matrix = DDGMatrices::d1(wingedM);
	std::cout << "Saving matrix\n";
	d1_matrix.saveMatrix("d1_.m");
	std::cout << "Size is : " << d1_matrix.getm() << ", " << d1_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating duald0 matrix\n";
	cpuCSRMatrix d0d_matrix = DDGMatrices::dual_d0(wingedM);
	std::cout << "Saving matrix\n";
	d0d_matrix.saveMatrix("d0d_.m");
	std::cout << "Size is : " << d0d_matrix.getm() << ", " << d0d_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating border1 matrix\n";
	cpuCSRMatrix border1_matrix = DDGMatrices::border1(wingedM);
	std::cout << "Saving matrix\n";
	border1_matrix.saveMatrix("border1_.m");
	std::cout << "Size is : " << border1_matrix.getm() << ", " << border1_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating border2 matrix\n";
	cpuCSRMatrix border2_matrix = DDGMatrices::border2(wingedM);
	std::cout << "Saving matrix\n";
	border2_matrix.saveMatrix("border2_.m");
	std::cout << "Size is : " << border2_matrix.getm() << ", " << border2_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating duald1 matrix\n";
	cpuCSRMatrix d1d_matrix = DDGMatrices::dual_d1(wingedM);
	std::cout << "Saving matrix\n";
	d1d_matrix.saveMatrix("d1d_.m");
	std::cout << "Size is : " << d1d_matrix.getm() << ", " << d1d_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating star0 matrix\n";
	cpuCSRMatrix st0_matrix = DDGMatrices::star0(wingedM);
	std::cout << "Saving matrix\n";
	st0_matrix.saveMatrix("st0_.m");
	std::cout << "Size is : " << st0_matrix.getm() << ", " << st0_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating star1 matrix\n";
	cpuCSRMatrix st1_matrix = DDGMatrices::star1(wingedM);
	std::cout << "Saving matrix\n";
	st1_matrix.saveMatrix("st1_.m");
	std::cout << "Size is : " << st1_matrix.getm() << ", " << st1_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating star2 matrix\n";
	cpuCSRMatrix st2_matrix = DDGMatrices::star2(wingedM);
	std::cout << "Saving matrix\n";
	st2_matrix.saveMatrix("st2_.m");
	std::cout << "Size is : " << st2_matrix.getm() << ", " << st2_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating border Diff Matrix\n";
	cpuCSRMatrix bdiff_matrix = DDGMatrices::d1dual_star1_borderDiff(wingedM);
	std::cout << "Saving matrix\n";
	bdiff_matrix.saveMatrix("d1st1_bdiff.m");
	std::cout << "Size is : " << bdiff_matrix.getm() << ", " << bdiff_matrix.getn()<<"\n\n";

	std::cout << "Hello, creating border Diff transp Matrix\n";
	cpuCSRMatrix bdiff_t_matrix = DDGMatrices::d1dual_star1_borderDiff_transp(wingedM);
	std::cout << "Saving matrix\n";
	bdiff_t_matrix.saveMatrix("d1st1_bdiff_t.m");
	std::cout << "Size is : " << bdiff_t_matrix.getm() << ", " << bdiff_t_matrix.getn()<<"\n\n";

	//operations
	d0_matrix= d0_matrix*cpuCSRMatrix::transpose(d0_matrix);
	d0_matrix.addOnDiagonal(5);
	d0_matrix.saveMatrix("d0d0t_plus5ondiag.m");


	std::cout << "Now, enter a number and press Enter\n";
	int a;
	std::cin >> a;
}

void tests::testWfWingedMesh()
{
	wfMesh * myMesh = new wfMesh();

	*myMesh = ball(1,3,2);
	myMesh->updateObserver(CONNECTIVITY_CHANGED);


	std::cout << "Hello, my Mesh has " << myMesh->getVertices().size() << " vertices\n";
	std::cout << "Now, enter a number and press Enter\n";
	int a;
	std::cin >> a;

	wingedMesh wingedM(myMesh);
	std::cout << "Hello, winged Mesh is created " << wingedM.getEdges().size() << " edges\n I iterate over the 0 nbrhood: \n";
	wingedEdge e0 = wingedM.getEdges()[wingedM.getv2e()[0][0]];
	cout << "(" << e0.start() << "," << e0.end() << ")";
	wingedEdge e = e0.getNext_bc(0);
	while(!(e0 == e)){
		cout << "(" << e.start() << "," << e.end() << ")";
		e= e.getNext_bc(0);
	}
	cout << "\n Reverse order:\n";

	e = e0.getPrev_bc(0);
	cout << "(" << e0.start() << "," << e0.end() << ")";
	while(!(e0 == e)){
		cout << "(" << e.start() << "," << e.end() << ")";
		e= e.getPrev_bc(0);
	}

	std::cout << "\nNow, enter a number and press Enter\n";
	std::cin >> a;

}

void tests::testCudaCusp()
{
	CUDA_STUFF::main();
}
