#include "application_vfDesign.h"
#include "meshMath.h"


application_vfDesign::application_vfDesign(void)
{
}


application_vfDesign::~application_vfDesign(void)
{
}


void application_vfDesign::computeField( MODEL & model, 
	std::vector<int> & source_verts, 
	std::vector<int> & sink_verts, 
	std::vector<int> & constrainedEdges, 
	std::vector<float> & edgeConstraints, 
	oneForm & target )
{
  
	cpuCSRMatrix duald1_border = model.getBorder1();
	cpuCSRMatrix duald1_border_transposed = model.getD0();
	cpuCSRMatrix star0_inv = model.getStar0_mixed();
	star0_inv.elementWiseInv();

	cpuCSRMatrix mat = model.getBorder2()* model.getStar2() * model.getD1();
	mat = mat +(model.getStar1_mixed()*duald1_border_transposed * star0_inv* (duald1_border) *model.getStar1_mixed());

	
	//add constraint for the constrained edges
//	std::vector<int> diag_ind;
//	mat.getDiagonalIndices(diag_ind);
	float weight = 100;
	float srcsink_flow = 10;

	for(int i = 0; i < constrainedEdges.size(); i++){
		mat.add(constrainedEdges[i], constrainedEdges[i],weight);
	}

	//////////////////////////////////////////////////////////////////////////
	//setup b
	//////////////////////////////////////////////////////////////////////////
	floatVector b(model.getMesh()->getEdges().size());

	for(int i = 0; i < constrainedEdges.size(); i++){
		b[constrainedEdges[i]]+=weight*edgeConstraints[i];
	}

	//lightweight implementation of star1 *d *source_flow for the source sink constraints
	int vert;
	for(int i =0; i < sink_verts.size(); i++){
		vert =sink_verts[i];
		wingedEdge* edg = & model.getMesh()->getAnEdge(vert);
		wingedEdge* first= edg;
		do{
			b[edg->getIndex()] -= edg->orientation(vert) * srcsink_flow
				*meshMath::dualEdge_edge_ratio(edg->getIndex(),* model.getMesh());
			edg = & edg->getNext_bc(vert);
		}
		while(edg!=first);
	}
	for(int i =0; i < source_verts.size(); i++){
		vert =source_verts[i];
		wingedEdge* edg = & model.getMesh()->getAnEdge(vert);
		wingedEdge* first= edg;
		do{
			b[edg->getIndex()] += edg->orientation(vert) * srcsink_flow
				*meshMath::dualEdge_edge_ratio(edg->getIndex(),* model.getMesh());
			edg = & edg->getNext_bc(vert);
		}
		while(edg!=first);
	}


	//////////////////////////////////////////////////////////////////////////
	//solving.....
	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("vf_laplace1.m");
	b.saveVector("b", "vf_b.m");

	target.loadVector("vf_x");

	//mat_border =	DDGMatrices::dual_d0(*msh) * DDGMatrices::star2(*msh) * DDGMatrices::d1(*msh);
}
