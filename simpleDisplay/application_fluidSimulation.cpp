#include "application_fluidSimulation.h"
#include <algorithm>
#include "DDGMatrices.h"
#include "meshMath.h"

application_fluidSimulation::application_fluidSimulation(MODEL &model)
{
	setUpSimulation(model);
}


application_fluidSimulation::~application_fluidSimulation(void)
{
}



void application_fluidSimulation::setUpSimulation( MODEL &model )
{
	myMesh = model.getMesh();
	meshMath::circumcenters(*myMesh, dualVertices);

	//the algorithm as it is implemented here only works if the dual
	//vertices lie inside their corresponding triangles
	//therefore they are reprojected
	reprojectDualVerticesIntoTriangles();
}


oneForm application_fluidSimulation::computeHarmonicFlow( std::vector<tuple3f> & borderConstraints, MODEL & model )
{
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
	// vector, which play the role of b in Ax = b, are adapted.
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

	harmonicFlux.dualToVField(harmonic_velocities);
	velocities = harmonic_velocities;

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

void application_fluidSimulation::pathTraceDualVertices( float t )
{
	backtracedDualVertices = dualVertices;
	int triangle;
	int nrIterations = t/0.05 + 1; // at least one iteration
	int end = myMesh->getFaces().size(); 
	float changed_t;
	bool hitBorder;
	std::vector<float> intern_memory; //intern memory

//#pragma omp parallel for private(triangle, changed_t, intern_memory, hitBorder) num_threads(8)
	for(int i = 0; i < end; i++){
		
		triangle = i;
		hitBorder = false;
		for(int j = 0; j < nrIterations;j++){
			changed_t = t/nrIterations;
			//triangle = -1 => outside of mesh
			while(changed_t > 0.0000001 && !hitBorder/*triangle!=-1*/){
				walkPath(&(backtracedDualVertices[i]), &triangle,&changed_t, &hitBorder,intern_memory);
			}
		}
//		triangle_btVel[i]=triangle; //what was this one good for?
	}

}

float application_fluidSimulation::maxt( tuple3f & pos, int triangle, tuple3f & dir, tuple3f & cutpos, wingedEdge & edge )
{
	assert(dir.x == dir.x && dir.y == dir.y && dir.z == dir.z);
	vector<tuple3f> & verts = myMesh->getVertices();
	vector<wingedEdge> & edges = myMesh->getEdges();

	tuple3i& tr = myMesh->getFaces()[triangle];
	tuple3i & f2e = myMesh->getf2e()[triangle];
	tuple3f & a = verts[tr.a];
	tuple3f & b = verts[tr.b];
	tuple3f & c = verts[tr.c];
	int start, end;

	tuple3f n = (b-a).cross(c-a);
	n.normalize();

	float max_t = numeric_limits<float>::infinity();
	
	tuple3f n1 = (a-b).cross(n);
	float t= n1.dot(a-pos)/n1.dot(dir);
	if(t==t && t>=-0.00001 && dir.dot(n1)<0){
		max_t = t;
		
		//edge.set(tr.a,tr.b);
		start = tr.a;
		end = tr.b;
	}

	n1 = (b-c).cross(n);
	t = n1.dot(b-pos)/n1.dot(dir);
	if(t==t && t>= -0.00001 && t<max_t && dir.dot(n1)<0){
		max_t = t;
		//edge.set(tr.b, tr.c);
		start = tr.b;
		end = tr.c;
	}

	n1 = (c-a).cross(n);
	t = n1.dot(c-pos)/n1.dot(dir);
	if(t==t && t>= -0.00001 && t<max_t && dir.dot(n1)<0){
		max_t = t;
		//edge.set(tr.c, tr.a);
		start = tr.c;
		end = tr.a;
	}


	//identify the edge hit
	edge = edges[f2e.a];
	if(!edge.contains(start, end)){
		edge = edges[f2e.b];
		if(!edge.contains(start, end)){
			edge = edges[f2e.c];
		}
	}

	cutpos.set(pos+dir*max_t);

	return max_t;
}

void application_fluidSimulation::getVelocityFlattened( tuple3f & pos, int actualTriangle, tuple3f & result, 
	std::vector<float> & weights,bool useHarmonicField)
{
	result.set(0,0,0);
	if(actualTriangle <0){
		return;
		//return tuple3f();
	}

	result.set(velocities[actualTriangle]);
	return;

/*	if(!doInterpolation){
		result.set(velocities[actualTriangle]);
		return;
	}

	assert(actualTriangle >=0);
	std::vector<tuple3f> & verts = myMesh->getVertices();
	tuple3i & tr = myMesh->getFaces()[actualTriangle];
	//determine actual dual Face 22 ms per go up to here.
	int dualFace;
	float d1 = (verts[tr.a]-pos).norm();
	float d2 = (verts[tr.b]-pos).norm();
	float d3 = (verts[tr.c]-pos).norm();
	float mn = min(min(d1,d2),d3);
	if(mn == d1){
		dualFace = tr.a;
	}
	else if(mn == d2){
		dualFace = tr.b;
	}
	else if ( mn == d3){
		dualFace = tr.c;
	}
	else{
		// NAN....
		assert(false);
	}


	//determine weights;
	fluidTools::bariCoords(pos,dualFace,dualVertices, weights, *myMesh);

	// dualVertices of dualFace;
	std::vector<int> & dualVertIDs = myMesh->getBasicMesh().getNeighborFaces()[dualFace];

	if(myMesh->getBorder().size() != 0){
		//on bordered meshs the stuff with the curv normal does not work.
		//assume that meshes are flat if they have a border
		assert(weights.size() == dualVertIDs.size());
		for(int i = 0; i < weights.size(); i++){
			assert(weights[i]== weights[i]);
			result += (velocities[dualVertIDs[i]])*weights[i];
			if(!useHarmonicField){
				result -= harmonicVelocities[dualVertIDs[i]]*weights[i];
			}
		}
	}
	else{
		tuple3f curvNormal = (*myMesh->getCurvNormals())[dualFace];
		curvNormal.normalize();
		tuple3f triangleNormal = (verts[tr.b]-verts[tr.a]).cross(verts[tr.c]-verts[tr.a]);
		tuple3f vel;

		for(int i = 0; i < weights.size(); i++){
			assert(weights[i]== weights[i]);
			vel.set(velocities[dualVertIDs[i]]);
			result += (vel - curvNormal * (curvNormal.dot(vel) ))*weights[i];
		}

		result = result - curvNormal * (result.dot(triangleNormal) / curvNormal.dot(triangleNormal));
		assert(result.x == result.x && result.y == result.y && result.z == result.z);
	}
	assert(result.x *0 == 0 && result.y*0 ==0 && result.z*0 == 0);
	//return result;*/
}

void application_fluidSimulation::walkPath( tuple3f * pos, int * triangle, float * t, bool * hitBorder, 
	std::vector<float> & weight_buffer, int direction )
{
	tuple3f dir;
	//velocity is stored in dir:
	getVelocityFlattened(*pos,*triangle, dir, weight_buffer);
	dir*=direction;
	tuple3f cut_pos;
	wingedEdge cut_edge;
	//stores the met edge and the cut position in cut_edge and cur_pos
	float max_t = maxt(*pos, *triangle, dir, cut_pos, cut_edge);


	if(*t <= max_t ){
		*pos = *pos + dir * (*t);
		*t = 0;
	}
	else{
		//leaving the triangle.
		pos->set(cut_pos);
		*t = *t - max_t;
		int temp = cut_edge.otherFace(*triangle);//meshOperation::getNegFace(cut_edge, myMesh->getBasicMesh());
		assert(temp!= *triangle);

		//if temp < 0 isOutside = true;
		if(temp < 0 ){
			*hitBorder = true;
		}
		else{
			*triangle = temp; 
			*hitBorder = false;
		}
	}
}

std::vector<tuple3f> & application_fluidSimulation::getHarmonicVel()
{
	return harmonic_velocities;
}

std::vector<tuple3f>& application_fluidSimulation::getDualVertices()
{
	return dualVertices;
}

std::vector<tuple3f> & application_fluidSimulation::getTracedDualVertices()
{
	return backtracedDualVertices;
}

void application_fluidSimulation::reprojectDualVerticesIntoTriangles()
{
	tuple3f a,b,c,n;
//	std::vector<tuple3f> & fcNormals = myMesh->getWfMesh()->getFaceNormals();
	std::vector<tuple3f> & verts = myMesh->getVertices();
	std::vector<tuple3i> & fcs = myMesh->getFaces();
	float temp, eps = 0.01;

	bool allOk = true;
	for(int i = 0; i < dualVertices.size(); i++){
		a.set(verts[fcs[i].a]);
		b.set(verts[fcs[i].b]);
		c.set(verts[fcs[i].c]);
		n.set((b-a).cross(c-a));
		n.normalize();

		if( (temp = n.cross(b-a).dot(dualVertices[i] -a)) < 0){
			if(temp > -eps){
				dualVertices[i] = (a+b)*0.5f;
				continue;
			}
			allOk = false;
			//			break;
		}
		if((temp = n.cross(c-b).dot(dualVertices[i] -b)) < 0){
			if(temp > -eps){
				dualVertices[i] = (c+b)*0.5f;
				continue;
			}
			allOk = false;
			//			break;
		}
		if((temp = n.cross(a-c).dot(dualVertices[i] -c)) < 0){
			if(temp > -eps){
				dualVertices[i] = (a+c)*0.5f;
				continue;
			}
			allOk = false;
			//			break;
		}

	}
}
