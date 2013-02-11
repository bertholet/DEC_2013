#include "application_vfDesign.h"
#include "meshMath.h"


#include "DDGMatrices.h"

application_vfDesign::application_vfDesign(void)
{
	adaptToBorder = true;
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
	cpuCSRMatrix & st1 = model.getStar1_mixed();

	cpuCSRMatrix duald1_st1 =  model.getBorder1() *st1* -1;
	cpuCSRMatrix duald1_st1_transp = st1 * model.getD0() * -1;
	
	cout << "\n computing VF....\n";
	if(adaptToBorder){
		cout << "*using border adaptation \n";
		duald1_st1 = duald1_st1 - DDGMatrices::d1dual_star1_borderDiff(*model.getMesh());
		duald1_st1_transp = duald1_st1_transp - DDGMatrices::d1dual_star1_borderDiff_transp(*model.getMesh());
	}
	cpuCSRMatrix star0_inv = model.getStar0_mixed();
	star0_inv.elementWiseInv();

	//computation of the matrix
	cpuCSRMatrix mat = model.getBorder2()* model.getStar2() * model.getD1();
	mat = mat +(duald1_st1_transp * star0_inv* duald1_st1);

	
	//adapt constraint for the constrained edges
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
			//d1_dual is - border1, a sink has a negative weight, so +=
			b[edg->getIndex()] += edg->orientation(vert) * srcsink_flow
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
			b[edg->getIndex()] -= edg->orientation(vert) * srcsink_flow
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

void application_vfDesign::setAdaptToBorder( bool what )
{
	adaptToBorder = what;
}
