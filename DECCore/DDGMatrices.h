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
	
	//this matrix is the border matrix but uses halfedges instead
	//of endges. This is a vertex x (2*nrEdges) matrix.
	static cpuCSRMatrix border1_halfedges( wingedMesh & aMesh );

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
	
	//////////////////////////////////////////////////////////////////////////
	//mixed Area based star matrices
	// star0 can optionally be computed such that voronoi areas at boundary vertices are computed 
	//according to the assumptions in the method star1_mixed_halfedges
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix star0_mixed( wingedMesh & aMesh, std::vector<float> & buffer, bool ignoreBorder = true);
	static cpuCSRMatrix star1_mixed( wingedMesh & aMesh, std::vector<float> & buffer);

	//////////////////////////////////////////////////////////////////////////
	//a variant which allows the dual edge, edge ratios
	// on boundary edges depend on the way you add an additional triangle
	// to the outside of a mesh. Say you consider the boundary edge (i,j), lying
	// on the border segment formed by the vertices p,i,j,n. Then you could either
	// add the triangle p,i,j or i,j,n and compute the ratios with either of those additional
	// triangles.
	//
	// the returned matrix has size (2*nrEdges x nrEdges), the upper half being the same as star1, considering
	// triengles (p,i,j) for every border edge (i,j)
	// the lower half considering triangles (i,j,n).
	//for inner vertices the upper and the lower matrix coincide.
	//////////////////////////////////////////////////////////////////////////
	
	static cpuCSRMatrix star1_mixed_halfedges( wingedMesh & aMesh);

	//////////////////////////////////////////////////////////////////////////
	//co derivative computation. Buffer is just a float vector that will be used during
	//internal calculations. If coderiv1_mixed is used often, like this the buffer can
	//be reused and does not need to be reallocated.
	//Optionally a precomuted matrix border1 can be used in order not to recompute it.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix coderiv1( wingedMesh & aMesh );
	static cpuCSRMatrix coderiv2( wingedMesh & aMesh );
	//As the coderivative is a simple concatenation of matrices these matrices can
	//be used.This is a faster memory efficient computation.
	static void coderiv1(
		cpuCSRMatrix & star_1, 
		cpuCSRMatrix & border_1,
		cpuCSRMatrix & star_0,
		cpuCSRMatrix & target);
	//The coderivative1 using mixed star1 matrices.
	static cpuCSRMatrix coderiv1_mixed( wingedMesh & aMesh , std::vector<float> & buffer);
	static cpuCSRMatrix coderiv1_mixed( wingedMesh & aMesh , std::vector<float> & buffer, cpuCSRMatrix & _border1);
	//////////////////////////////////////////////////////////////////////////
	//variant of the coderivative. Boundary vertices are treated as if they 
	//had a full neighborhood of  triangles. This one uses the border1_halfedges
	//and star1_mixed_halfedge matrices in the concatenation. see also their
	//method descriptions.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix coderiv1_mixed_ignoreBoundary( wingedMesh & aMesh);


	//////////////////////////////////////////////////////////////////////////
	// creates an identity Matrix of the appropriate k-form space dimension.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix id0(wingedMesh & aMesh);
	static cpuCSRMatrix id1(wingedMesh & aMesh);
	static cpuCSRMatrix id2(wingedMesh & aMesh);


	//////////////////////////////////////////////////////////////////////////
	// Laplace operator using coderiv1 ignore boundary matrix.
	//////////////////////////////////////////////////////////////////////////
	static cpuCSRMatrix laplaceIgnoreBoundary(wingedMesh & aMesh);
	
	//////////////////////////////////////////////////////////////////////////
	//The matrix as described in the Section Vector Field Design, which can
	// be used in an incompressible setting to adapt the no divergence
	// condition to be valid on boundary vertices.
	static cpuCSRMatrix d1dual_star1_borderDiff(wingedMesh & aMEsh);
	static cpuCSRMatrix d1dual_star1_borderDiff_transp( wingedMesh & aMEsh) ;

	//identity on boundary edges, zero else.
	static cpuCSRMatrix onesBorderEdges( wingedMesh & aMesh );

	static cpuCSRMatrix diagMatrix( floatVector & vals );

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
	static cpuCSRMatrix dual_d1_dualprimal( meshMetaInfo & aMesh );*/


};
