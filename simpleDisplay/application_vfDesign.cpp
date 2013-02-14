#include "application_vfDesign.h"
#include "meshMath.h"


#include "DDGMatrices.h"

application_vfDesign::application_vfDesign(void)
{
	adaptToBorder = true;
	weight = 100;
	srcsink_flow = 10;
	fieldLength = 1;
}


application_vfDesign::~application_vfDesign(void)
{
}

void application_vfDesign::setAdaptToBorder( bool what )
{
	adaptToBorder = what;
}


void application_vfDesign::setGuideFieldWeight( float wght )
{
	weight = wght;
}

void application_vfDesign::setGuideFieldScale( float constraintLength )
{
	fieldLength = constraintLength;
}

void application_vfDesign::setSourceSinkFlow( float flow )
{
	srcsink_flow = flow;
}



void application_vfDesign::computeField( MODEL & model, 
	std::vector<int> & source_verts, 
	std::vector<int> & sink_verts, 
	std::vector<int> & constrainedEdges, 
	std::vector<float> & edgeConstraints, 
	oneForm & target, bool directional)
{

	wingedMesh & mesh = *model.getMesh();
	
	//////////////////////////////////////////////////////////////////////////
	//if specified compute the approximate local field lengths in a first pass
	std::vector<tuple3f> firstPassField;
	if(directional){
		oneForm firstPass(&mesh);
		computeField(model,source_verts,sink_verts, 
			std::vector<int>(), std::vector<float>(), //do not use directional constraints for this estimation
			firstPass);
		firstPass.toVField(firstPassField);
	}

	//////////////////////////////////////////////////////////////////////////
	//Matrix set up
	cpuCSRMatrix st1 = DDGMatrices::star1(mesh);//model.getStar1_mixed();
	cpuCSRMatrix duald1_st1 =  model.getDualD1() * st1;//model.getBorder1() *st1* -1;
	cpuCSRMatrix duald1_st1_transp = st1*model.getDualD1_T();//st1 * model.getD0() * -1;
	if(adaptToBorder){
		duald1_st1 = duald1_st1 +DDGMatrices::d1dual_star1_borderDiff(mesh);
		duald1_st1_transp = duald1_st1_transp + DDGMatrices::d1dual_star1_borderDiff_transp(mesh);
	}
	cpuCSRMatrix star0_inv = model.getStar0_mixed();
	star0_inv.elementWiseInv();

	//computation of the main matrix
	cpuCSRMatrix mat = model.getBorder2()* model.getStar2() * model.getD1();
	mat = mat +(duald1_st1_transp * star0_inv* duald1_st1);
	
	//adaptation to enforce the constraints for fixed edge values
	for(int i = 0; i < constrainedEdges.size(); i++){
		mat.add(constrainedEdges[i], constrainedEdges[i],weight);
	}

	//////////////////////////////////////////////////////////////////////////
	//setup b
	floatVector b(mesh.getEdges().size());

	//fixed edge value constraints
	float local_scale = fieldLength;
	for(int i = 0; i < constrainedEdges.size(); i++){
		if(directional){
			tuple2i & fcs = mesh.getEdges()[constrainedEdges[i]].getAdjFaces();
			local_scale = (firstPassField[fcs.a].norm()
				+ firstPassField[fcs.b].norm())*0.5f;
		}
		b[constrainedEdges[i]]+=weight*edgeConstraints[i] * local_scale;
	}


	//lightweight implementation of b + star1*d0 *source_sink_constraints
	//matrices could be used instead.
	add_star1_d0_srcflow_to_b(b,sink_verts, source_verts, srcsink_flow, model);

	//////////////////////////////////////////////////////////////////////////
	//solving.....
	mat.saveMatrix("vf_laplace1.m");
	b.saveVector("b", "vf_b.m");

	target.loadVector("vf_x");

}

void application_vfDesign::add_star1_d0_srcflow_to_b( floatVector &b, std::vector<int> & sink_verts, std::vector<int> & source_verts, float srcsink_flow, MODEL & model )
{
	int vert;
	wingedEdge* edg, * first;
	wingedMesh & mesh =  * model.getMesh();

	for(int i =0; i < sink_verts.size(); i++){
		vert =sink_verts[i];

		//iterate over the 1 neighborhood vertex vert
		first =	edg = & mesh.getAnEdge(vert);
		do{
			//d1_dual is - border1, a sink has a negative weight, so +=
			b[edg->getIndex()] += edg->orientation(vert) * srcsink_flow
				* meshMath::dualEdge_edge_ratio(edg->getIndex(),mesh);
			edg = & edg->getNext_bc(vert);
		}
		while(edg!=first);
	}
	for(int i =0; i < source_verts.size(); i++){
		vert =source_verts[i];

		//iterate over the 1 neighborhood vertex vert
		first = edg = & mesh.getAnEdge(vert);
		do{
			b[edg->getIndex()] -= edg->orientation(vert) * srcsink_flow
				*meshMath::dualEdge_edge_ratio(edg->getIndex(),mesh);
			edg = & edg->getNext_bc(vert);
		}
		while(edg!=first);
	}
}


