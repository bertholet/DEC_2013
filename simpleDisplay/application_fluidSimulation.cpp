#include "application_fluidSimulation.h"
#include <algorithm>
#include "DDGMatrices.h"

application_fluidSimulation::application_fluidSimulation(void)
{
}


application_fluidSimulation::~application_fluidSimulation(void)
{
}


oneForm application_fluidSimulation::setHarmonicFlow( std::vector<tuple3f> & borderConstraints, MODEL & model )
{
	myMesh = model.getMesh();
	oneForm harmonicFlux(myMesh);
	if(myMesh->getBoundaryEdges().size() == 0){
		//only for bordered meshs.
		return harmonicFlux;
	}


	
	//compute a border adapted least square formulation laplacian.
	//adapting duald1 and star0 to consider inner border(s).
	cpuCSRMatrix star0inv = model.getStar0_mixed();
	star0inv.elementWiseInv();
	cpuCSRMatrix duald1 = model.getDualD1();
	adaptMatrices_zeroTotalBorderVort(star0inv, duald1, model);

	cpuCSRMatrix dt_star1_adapted = duald1 * model.getStar1_mixed();//DDGMatrices::star1(*myMesh) ;
	cpuCSRMatrix Lflux = (model.getD1_T()*DDGMatrices::star2(*myMesh)*model.getD1()) 
		+ cpuCSRMatrix::transpose(dt_star1_adapted)*star0inv*dt_star1_adapted;

	//enforce the fixed values on Boundaries: the matrix Lflux and the fluxConstr
	// vector, which play the role of b in Ax = b are adapted.
	floatVector fluxConstr = harmonicFlux;
	float weight = 10;
	setUpBorderConstraints_harm(Lflux, fluxConstr, weight, borderConstraints,model);

	
	//solving
	Lflux.saveMatrix("fs_LHarmonic.m");
	fluxConstr.saveVector("fs_b_harm", "fs_bHarmonic.m");
	harmonicFlux.loadVector("fs_x_harm");

	/*//UPDATE STUFF
	fluidTools::flux2Vorticity(harmonicFlux,vorticity,*myMesh,dt_star1);
	fluidTools::flux2Velocity(harmonicFlux,harmonicVelocities, *myMesh);
	updateVelocities();*/


	return harmonicFlux;

}

void application_fluidSimulation::adaptMatrices_zeroTotalBorderVort( cpuCSRMatrix & star0inv, cpuCSRMatrix & duald1, MODEL & model )
{
	int sz;
	std::vector<float> vals;
	std::vector<wingedEdge*> brdrs =model.getMesh()->getBoundaryEdges();
	std::vector<int> indices;
	wingedEdge *edge, *first;

	//iterate over inner borders:
	for(int i = 1; i < brdrs.size(); i++){
		indices.clear();
		//collect the indices of boundary vertices
		edge = first = brdrs[i];
		do{
			indices.push_back(edge->getFirstBoundaryVertex());
			edge = edge->nextBorderEdge();
		}
		while(edge!= first);

		//create a matrix that consists of one row with ones on
		//boundary vertices and zeros else.
		std::sort(indices.begin(), indices.end());
		vals.assign(indices.size(),1);
		
		cpuCSRMatrix borderVerticesT;
		borderVerticesT.addLine(indices, vals);
		borderVerticesT.forceNrColumns(model.getMesh()->getVertices().size());

		//borderVerticesT*duald1 computes the sum of dual values around the boundary
		//as we want to enforce that this is zero we just copy the matrix to the bottom
		//of duald1
		(borderVerticesT * duald1).getLine(0,indices,vals);
		duald1.addLine(indices,vals);

	}

	//additionally the star0_inv matrix needs to be adapted:
	//the dual faces around boundary vertices are ignored
	for(int i = 0; i < brdrs.size(); i++){

		//iterate over the boundary vertices and set the area/weight to zero.
		edge = first = brdrs[i];
		do{
			star0inv.setLineToZero(edge->getFirstBoundaryVertex());
			edge = edge->nextBorderEdge();
		}
		while(edge!= first);
	}

	//instead the new 'merged' boundary faces get an
	// (arbitrarily choosen but large) weight.
	for(int i = 1; i < brdrs.size(); i++){
		std::vector<int> bla;
		std::vector<float> bla2;
		bla.push_back(star0inv.getn());
		bla2.push_back(1000000);
		star0inv.addLine(bla,bla2);
	}
}

void application_fluidSimulation::setUpBorderConstraints_harm( cpuCSRMatrix &Lflux, floatVector &fluxConstr, float weight, std::vector<tuple3f> &borderConstraints, MODEL &model)
{
	//iterate over all boundaries, adapt the matrix and set up the constraint.
	wingedEdge * first, *edge;
	std::vector<tuple3f> & verts = model.getMesh()->getVertices();
	for(int i = 0; i <model.getMesh()->getBoundaryEdges().size(); i++){
		first = edge = model.getMesh()->getBoundaryEdges()[i];
		do{
			Lflux.add(edge->getIndex(),edge->getIndex(),weight);
			fluxConstr[edge->getIndex()] = borderConstraints[i].dot(verts[edge->end()] -verts[edge->start()]) * weight;
			edge = edge->nextBorderEdge();
		}while(edge != first);

	}
}
