#include "application_vfDesign.h"


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
  
	cpuCSRMatrix mat = model.getBorder2()* model.getStar2() * model.getD1();
	cpuCSRMatrix star0_inv = model.getStar0_mixed();
	star0_inv.elementWiseInv();
	cpuCSRMatrix duald1_border = model.getBorder1();
	cpuCSRMatrix duald1_border_transposed = model.getD0();
	mat = mat +(model.getStar1_mixed()*duald1_border_transposed * star0_inv* (duald1_border) *model.getStar1_mixed());

	
	//add constraint for the constrained edges
//	std::vector<int> diag_ind;
//	mat.getDiagonalIndices(diag_ind);
	float weight = 100;

	for(int i = 0; i < constrainedEdges.size(); i++){
		mat.add(constrainedEdges[i], constrainedEdges[i],weight);
	}

	//setup b
	floatVector b(model.getMesh()->getEdges().size());

	for(int i = 0; i < constrainedEdges.size(); i++){
		b[constrainedEdges[i]]+=weight*edgeConstraints[i];
	}

	//////////////////////////////////////////////////////////////////////////
	//solving.....
	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("vf_laplace1.m");
	b.saveVector("b", "vf_b.m");

	target.loadVector("vf_x");

	//mat_border =	DDGMatrices::dual_d0(*msh) * DDGMatrices::star2(*msh) * DDGMatrices::d1(*msh);
}
