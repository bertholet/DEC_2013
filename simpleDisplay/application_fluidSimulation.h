#pragma once
#include "oneForm.h"
#include <vector>
#include "MODEL.h"
#include "wingedMesh.h"

class application_fluidSimulation
{
private:
	wingedMesh * myMesh;
	//dual vertices
	std::vector<tuple3f> backtracedDualVertices, dualVertices,
		//velocities
		velocities, harmonic_velocities, 
		backtracedVelocity, backtracedVelocity_noHarmonic;
	//triangles the backtraced vertices lie in
	std::vector<int> triangle_btVel;
	// the vorticity 0form
	floatVector vorticity;
	//the viscosity
	float viscosity;
	//speed up:
	std::vector<bool> isOnBorder;

public:
	application_fluidSimulation(MODEL &model);
	~application_fluidSimulation(void);

	void setUpSimulation(MODEL &model);


	//////////////////////////////////////////////////////////////////////////
	// step 0 
	//
	//calculate the harmonic flow for a bordered mesh, where
	// the constraints on the border are given by borderconstraints.
	// The constraints have to be understood as that the flow on the border
	// component i is constrained to be borderConstraints[i].
	// this will be added to the velocity field when pathtracing.
	//////////////////////////////////////////////////////////////////////////
	oneForm computeHarmonicFlow(std::vector<tuple3f> & borderConstraints, MODEL & model);

	//////////////////////////////////////////////////////////////////////////
	// step 1 
	//
	//Pathtrace all dualvertices
	//////////////////////////////////////////////////////////////////////////
	void pathTraceDualVertices(float t);

	//////////////////////////////////////////////////////////////////////////
	// step 2
	//
	// to calculate backtraced velocities at the traced dual positions
	//////////////////////////////////////////////////////////////////////////
	void updateBacktracedVelocities();

	//////////////////////////////////////////////////////////////////////////
	// step 3
	//
	// compute the vorticities using the backtraced velocities
	//////////////////////////////////////////////////////////////////////////
	void computeBacktracedVorticities();

	std::vector<tuple3f> & getHarmonicVel();
	std::vector<tuple3f>& getDualVertices();
	std::vector<tuple3f> & getTracedDualVertices();
	std::vector<tuple3f> & getTracedVelocities();
	floatVector & getVorticities();

private:
	//////////////////////////////////////////////////////////////////////////
	//Adapts Lflux and b to enforce the boundary constraints specified by
	// borderConstraints, using the information in model.
	//////////////////////////////////////////////////////////////////////////
	void setUpBorderConstraints_harm( cpuCSRMatrix &Lflux, floatVector &b, float weight, std::vector<tuple3f> &borderConstraints, MODEL &model);

	//////////////////////////////////////////////////////////////////////////
	//This adapts the matrices star0inv and duald1 such that when using them
	//to compute a least squares formulation of the laplacian, they automatically
	//enforce zero flux around inner boundaries
	//////////////////////////////////////////////////////////////////////////
	void adaptMatrices_zeroTotalBorderVort( cpuCSRMatrix & star0inv, cpuCSRMatrix & duald1, MODEL & model );


	//////////////////////////////////////////////////////////////////////////
	// walk the path for at most a timestep t in the given triangle. t is updated
	// to the remaining t to go, pos is updated and the new actual triangle
	// if the border of the triangle is reached.
	// If the border of the mesh is reached -1 is returned.
	// weight_buffer is a vector that is used for intern calculations as
	// preallocated memory.
	//////////////////////////////////////////////////////////////////////////
	void walkPath(tuple3f * pos, int * triangle, float *  t, bool * hitBorder,
		std::vector<float> & weight_buffer,int dir =-1);

	float maxt( tuple3f & pos, int triangle, tuple3f & dir, tuple3f & cutpos, wingedEdge & edge );


	//////////////////////////////////////////////////////////////////////////
	// Method to iterpolate the velocity field on curved manifolds. 
	// The Velocities are projected locally along the
	// curvature normal onto a plane, interpolated there and then projected 
	// back. The resulting velocity will be stored in result, weightbuffer
	// needs to be a vector, used as a buffer for intern calculations.
	//////////////////////////////////////////////////////////////////////////
	void getVelocityFlattened(tuple3f & pos, int triangleIndex, tuple3f & result, 
		std::vector<float> & weight_buffer,bool useHarmonicField = true);
	
	//////////////////////////////////////////////////////////////////////////
	//Makes sure that all dual vertices lie inside the corresponding triangles
	// This fascilitates the backtracing step
	void reprojectDualVerticesIntoTriangles();
	
};

