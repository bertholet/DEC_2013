#pragma once
#include "cpuCSRMatrix.h"
#include "wingedMesh.h"
#include "DECCORE_EXPORT.h"

class DECCORE_EXPORT DDGMatrices
{
public:
	DDGMatrices(void);
	~DDGMatrices(void);

	static cpuCSRMatrix border2(wingedMesh & aMesh);
	static cpuCSRMatrix border1(wingedMesh & aMesh);

	static cpuCSRMatrix d0( wingedMesh & aMesh );
	static cpuCSRMatrix d1( wingedMesh & aMesh );
	
	//The derivatives on the dual mesh
	static cpuCSRMatrix dual_d0( wingedMesh & aMesh );
	static cpuCSRMatrix dual_d1( wingedMesh & aMesh );

	//vornoi Area / 1
	static cpuCSRMatrix star0( wingedMesh & aMesh );
	//dual edge / edge
	static cpuCSRMatrix star1( wingedMesh & aMesh );
	//1 / volume
	static cpuCSRMatrix star2( wingedMesh & aMesh );
	//mixed Area based star matrices
	static cpuCSRMatrix star0_mixed( wingedMesh & aMesh, std::vector<float> & buffer);
	static cpuCSRMatrix star1_mixed( wingedMesh & aMesh, std::vector<float> & buffer);
	static cpuCSRMatrix coderiv1_mixed( wingedMesh & aMesh , std::vector<float> & buffer);
	static cpuCSRMatrix coderiv1_mixed( wingedMesh & aMesh , std::vector<float> & buffer, cpuCSRMatrix & _border1);

	//The coderivatives, simple concatenations of the star
	// Fast memory efficient computation.
	static void coderiv1(
		cpuCSRMatrix & star_1, 
		cpuCSRMatrix & border_1,
		cpuCSRMatrix & star_0,
		cpuCSRMatrix & target);
	//The coderivatives, simple concatenations of the star
	// abd d matrices.
	static cpuCSRMatrix coderiv1( wingedMesh & aMesh );
	static cpuCSRMatrix coderiv2( wingedMesh & aMesh );

	//////////////////////////////////////////////////////////////////////////
	// creates an identity Matrix of the appropriate k-form space dimension.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix id0(wingedMesh & aMesh);
	static cpuCSRMatrix id1(wingedMesh & aMesh);
	static cpuCSRMatrix id2(wingedMesh & aMesh);

/*	static cpuCSRMatrix onesBorder(std::vector<std::vector<int>> & border, int n, int m);

	//////////////////////////////////////////////////////////////////////////
	// special matrices for bordered meshs in an incompressibility setting
	// 
	// matrix takes a oneform and returns the dual values on the
	// primal simplices.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix dualVals1(meshMetaInfo & aMesh);
	//////////////////////////////////////////////////////////////////////////
	// duald1 + duald1border diff will be dual d1 in a setting where there are
	// borders and no compression i.e. the onforms sum up to zeor around 
	// each triangle.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix dual_d1_borderdiff( meshMetaInfo & aMesh );

	static cpuCSRMatrix dual_d1star1_borderdiff( meshMetaInfo & aMesh );

	//////////////////////////////////////////////////////////////////////////
	// matrix that maps oneforms to 0 forms and has for border vertex
	// lines the entries ...0.5 .... 0.5... for border edges adjacent
	// to the vertex. i.e it is the part of the dual values on primal
	// edges for rot calculation.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix dual_d1_dualprimal( meshMetaInfo & aMesh );


	static cpuCSRMatrix onesBorderEdges( meshMetaInfo & aMesh );*/
};
