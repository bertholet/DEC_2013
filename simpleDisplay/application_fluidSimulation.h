#pragma once
#include "oneForm.h"
#include <vector>
#include "MODEL.h"
#include "wingedMesh.h"

class application_fluidSimulation
{
private:
	wingedMesh * myMesh;
public:
	application_fluidSimulation(void);
	~application_fluidSimulation(void);

	//////////////////////////////////////////////////////////////////////////
	// calculate the harmonic flow for a bordered mesh, where
	// the constraints on the border are given by borderconstraints.
	// The constraints have to be understood as that the flow on the border
	// component i is constrained to be borderConstraints[i].
	// this will be added to the velocity field when pathtracing.
	//////////////////////////////////////////////////////////////////////////
	oneForm setHarmonicFlow(std::vector<tuple3f> & borderConstraints, MODEL & model);

	void setUpBorderConstraints( cpuCSRMatrix &Lflux, floatVector &fluxConstr, float weight, std::vector<tuple3f> &borderConstraints, MODEL &model);

	//////////////////////////////////////////////////////////////////////////
	//This adapts the matrices star0inv and duald1 such that when using them
	//to compute a least squares formulation of the laplacian, they automatically
	//enforce zero flux around inner boundaries
	//////////////////////////////////////////////////////////////////////////
	void adaptMatrices_zeroTotalBorderVort( cpuCSRMatrix & star0inv, cpuCSRMatrix & duald1, MODEL & model );
};

