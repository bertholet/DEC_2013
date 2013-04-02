#pragma once
#include "oneForm.h"
#include <vector>
#include "MODEL.h"
#include "wingedMesh.h"
#include "SolverIF.h"

#ifndef DEBUG_FS
#define DEBUG_FS
#endif


class application_fluidSimulation
{
private:
	wingedMesh * myMesh;
	//dual vertices
	std::vector<tuple3f> backtracedDualVertices, dualVertices,
		//normals
		curvatureNormals,
		//velocities
		velocities, velocities_harm, velocities_vort,
		backtracedVelocity, backtracedVelocity_noHarmonic;
	//triangles the backtraced vertices lie in
	std::vector<int> triangle_btVel;
	// the vorticity 0form
	floatVector vorticity, buffer;
	//the flux generated from the backtraced vorticities.
	oneForm flux, harmonicFlux, forceFlux;
	//the viscosity
	float viscosity, timestep;
	//speed up:
	std::vector<bool> isOnBorder;

	//matrix and solver for diffusion computation
	cpuCSRMatrix star0_min_vhl;
	SolverIF * solver_diffusion;

	//Laplacian matrix and Solver for vorticity to flux conversion 
	cpuCSRMatrix L;
	SolverIF * solver_vort2flux;

	//Matrix for force addition
	cpuCSRMatrix duald1_star1;

	//current model
	MODEL * myModel;
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
	//Do one iteration of the simulation:
	//i.e the steps 1 to 6.5 below.
	//////////////////////////////////////////////////////////////////////////
	void doOneStep();

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

	//////////////////////////////////////////////////////////////////////////
	// step 4 add Forces
	//
	//////////////////////////////////////////////////////////////////////////
	void addForces2Vorticity(float timestep);

	//////////////////////////////////////////////////////////////////////////
	// step 5
	//
	// diffuse vorticity
	//////////////////////////////////////////////////////////////////////////
	void addDiffusion2Vorticity();

	//////////////////////////////////////////////////////////////////////////
	// step 6
	//
	// convert the computed vorticities to flux
	//////////////////////////////////////////////////////////////////////////
	void vorticity2Flux();

	//////////////////////////////////////////////////////////////////////////
	// step 6.5
	// update the velocity field
	//////////////////////////////////////////////////////////////////////////
	void updateVelocities();

	void flux2Vorticity();

	void vel2Vorticity();

	std::vector<tuple3f> & getHarmonicVel();
	std::vector<tuple3f> & getVortVel();
	std::vector<tuple3f> & getVelocities();
	std::vector<tuple3f>& getDualVertices();
	std::vector<tuple3f> & getTracedDualVertices();
	std::vector<tuple3f> & getTracedVelocities();
	floatVector & getVorticities();

	void setViscosityAndTimestep( float visc, float timestp);
	void setForces(std::vector<int> &faces, std::vector<tuple3f> & dirs, float scale);
	void resetForces();
	void resetFlux();

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

	int closestVertex(tuple3i &tr, tuple3f & pos );


	//////////////////////////////////////////////////////////////////////////
	//Makes sure that all dual vertices lie inside the corresponding triangles
	// This fascilitates the backtracing step
	void reprojectDualVerticesIntoTriangles();
	void setUpMatrixL( MODEL & model );
};

