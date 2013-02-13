#include "application_meshParam.h"
#include "mySolver.h"
#include "DDGMatrices.h"
#include "matrixCreator.h"
#include <math.h>
#include <algorithm>
#define PI 3.14159265359f

//////////////////////////////////////////////////////////////////////////
//create a matrix of 2*borderlength x 2*borderlength to solve for x,y
//positions simultaneously
//////////////////////////////////////////////////////////////////////////
class conformalBorderCreator:public matrixCreator{
private:
	wingedMesh * myMesh;
	std::vector<float> lambdas, angles;
	int borderSz;

public:
	conformalBorderCreator(wingedMesh & m){
		myMesh= &m;
		borderSz = m.getBoundarySizes()[0];


		application_meshParam::computeAnglesAndLambdas(m,angles,lambdas,0,true);
	}
	~conformalBorderCreator(){}

	virtual float val( int i , int j ) 
	{
		int sz = angles.size();
		
		float out = 0;
		float angle = angles[i%sz];
		float lambda = lambdas[i%sz];

		bool firstLine = i<sz;
		bool xcoordinates =j<sz;
		//ith line is the constraint around ith border vertex.
		bool j_is_i_min1 = ((i+sz-1)%sz==j%sz);
		bool j_is_i = (j%sz==i%sz);
		bool j_is_ip1 = (j%sz==(i+1)%sz);

		if(firstLine){
			if(xcoordinates){
				if(j_is_i_min1){
					out = -lambda*cos(angle);
				}
				else if(j_is_i){
					out =lambda*cos(angle) -1;
				}
				else if(j_is_ip1){
					out = 1;
				}
				else{
					assert(false);
				}
			}
			else{
				if(j_is_i_min1){
					out = - lambda * sin(angle);
				}
				else if (j_is_i)
				{
					out = lambda *sin(angle);
				}
				else if(j_is_ip1){
					out = 0;
				}
				else{
					assert(false);
				}
				
			}

		}
		else{
			if(xcoordinates){
				if(j_is_i_min1){
					out = lambda * sin(angle);
				}
				else if (j_is_i)
				{
					out = -lambda*sin(angle);
				}
				else if(j_is_ip1){
					out = 0;
				}
				else{
					assert(false);
				}
			}
			else{
				if(j_is_i_min1){
					out = -lambda * cos(angle);
				}
				else if (j_is_i)
				{
					out = lambda * cos(angle)-1;
				}
				else if(j_is_ip1){
					out = 1;
				}
				else{
					assert(false);
				}
			}

		}
		return out;
	}

	virtual void indices( int row, std::vector<int> & target ) 
	{
		int rw = row %borderSz;
		target.clear();

		target.push_back((rw+borderSz-1)%borderSz);
		target.push_back(rw);
		target.push_back((rw+1)%borderSz);

		target.push_back((rw+borderSz-1)%borderSz + borderSz);
		target.push_back(rw + borderSz);
		target.push_back((rw+1)%borderSz + borderSz);

		if(rw==0 || rw == borderSz -1){
			sort(target.begin(), target.end());
		}
	}

};

application_meshParam::application_meshParam(void)
{
}


application_meshParam::~application_meshParam(void)
{
}


int application_meshParam::circleBorder( wingedMesh &m, std::vector<tuple3f> & target )
{

	int sz = m.getBoundarySizes()[0];
	target.clear();

	for(int i = 0; i < sz;i++){
		target.push_back(tuple3f((sin(-i*2* PI/sz)+1.f)/2.f,(cos(-i*2* PI/sz)+1.f)/2.f,0.f));
	}
	return 0;
}

int application_meshParam::distWeightCircleBorder( wingedMesh &m, std::vector<tuple3f> & target )
{
	float length = m.calcBorderLength(0);
	float ln = 0;
	wingedEdge * first, *edge;


	first = edge = m.getBoundaryEdges()[0];
	do{
		ln+= m.length(*edge);
		target.push_back(tuple3f((sin(-ln/length * 2* PI)+1.f)/2.f,(cos(-ln/length * 2* PI)+1.f)/2.f,0.f));

		edge= edge->nextBorderEdge();
	}while(edge!=first);

	return 0;
}


int application_meshParam::conformalBorder( wingedMesh &m, std::vector<tuple3f> & target )
{
	target.clear();
	
	//setUp Matrix;
	cpuCSRMatrix mat;
	int boundarySz = m.getBoundarySizes()[0];
	mat.initMatrix(conformalBorderCreator(m), 2*boundarySz);
	
	mySolver solver;
	solver.setMatrix(mat);

	//setup x,b
	floatVector x,b;
	x.assign(2*boundarySz,0);
	b.assign(2*boundarySz,0);

	//one additional constraint
	b[boundarySz/2] = 1;
	b[boundarySz+boundarySz/2] = 1;
	mat.add(0,0,1);
	mat.add(boundarySz/2,boundarySz/2,1);
	mat.add(boundarySz,boundarySz,1);
	mat.add(boundarySz+boundarySz/2,boundarySz+boundarySz/2,1);

	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("conformalMat.m");
	b.saveVector("b", "conformalB.m");
	//////////////////////////////////////////////////////////////////////////

	solver.solve(x,b);
		
	for(int k=0; k < boundarySz; k++){
		target.push_back(tuple3f(x[k], x[k+boundarySz],0));
	}
	normalizeTexture(target);

	return 0;
}

void application_meshParam::calcTexturePos( MODEL & model ,borderStyle borderFunc)
{

	wingedMesh & m = * model.getMesh();
	if(m.getBoundaryEdges().size()!=1){
		return;
	}

	cpuCSRMatrix mat;
	floatVector xy, b;
	mySolver solver;

	setUp(mat, model);
	setUpB(b, model, borderFunc);


	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("meshParamMatrix.m");
	b.saveVector("b","meshParamVector.m");
	floatVector::saveTuple3iVectorMatlabStyle(m.getFaces(),"tri","meshParamFaces.m");
	//////////////////////////////////////////////////////////////////////////

	solver.setMatrix(mat);
	solver.solve(xy,b);


}

void application_meshParam::calcTexturePosMultiborder( MODEL & model,borderStyle borderFunc /*= & circleBorder*/ )
{
	wingedMesh & m = * model.getMesh();
	if(m.getBoundaryEdges().size()<1){
		return;
	}

	floatVector xy, b;
	setUpB(b, model, borderFunc);
	cpuCSRMatrix mat;
	setUp(mat, model);
	//adapt inner border vertices
	adaptConstraintsInnerBorder(mat, m);

	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("meshParamMatrix.m");
	b.saveVector("b","meshParamVector.m");
	floatVector::saveTuple3iVectorMatlabStyle(m.getFaces(),"tri","meshParamFaces.m");
	//////////////////////////////////////////////////////////////////////////

	mySolver solver;
	solver.setMatrix(mat);
	solver.solve(xy,b);
}

void application_meshParam::adaptConstraintsInnerBorder( cpuCSRMatrix & mat, wingedMesh & m )
{
	std::vector<wingedEdge*> borders = m.getBoundaryEdges();
	std::vector<float> angles, lambdas;
	float lambda, angle;

	wingedEdge *first, *previous,*actual, *next;
	int index, vrtx, nrVt = m.getVertices().size();
	int vrtxm1,vrtxp1;
	int bordersz;

	//hack to be able to adapt all important entries.
	makeMatrixEditable(mat, m);

	for(int i = 1; i < borders.size(); i++){
		computeAnglesAndLambdas(m,angles,lambdas,i,false);
		bordersz= angles.size();

		//iterate over the border nr i-
		//init
		actual = first = borders[i];
		previous = actual->previousBorderEdge();
		assert(previous->nextBorderEdge() == actual);
		assert(previous->orientation(actual->getFirstBoundaryVertex())!= 0);
		index=0;
		do{
			//iteration
			next = actual->nextBorderEdge();

			//find the vertices concerned, the edge ratio, and the angle
			vrtx = actual->getFirstBoundaryVertex();
			vrtxm1 = previous->getFirstBoundaryVertex();
			vrtxp1 = next->getFirstBoundaryVertex();
			lambda = lambdas[index];
			angle = angles[index];

			//directly adapt matrix for reflex vertices: set to conformalityConstraint
			//fill in the line for the xcoordinate and for the y coordinate.
			if(angle<PI){//(bordersz+2)/bordersz * PI){
				mat.setLineToZero(vrtx);
				mat.set(vrtx, vrtxm1,-lambda*cos(angle));
				mat.set(vrtx, vrtx,lambda*cos(angle)-1);
				mat.set(vrtx, vrtxp1,1);
				mat.set(vrtx, vrtxm1+nrVt,-lambda*sin(angle));
				mat.set(vrtx, vrtx+nrVt,lambda*sin(angle));

				mat.setLineToZero(vrtx + nrVt);
				mat.set(vrtx+nrVt, vrtxm1,lambda*sin(angle));
				mat.set(vrtx+nrVt, vrtx,-lambda*sin(angle));
				mat.set(vrtx+nrVt, vrtxm1+nrVt,-lambda*cos(angle));
				mat.set(vrtx+nrVt, vrtx+nrVt, lambda*cos(angle)-1);
				mat.set(vrtx+nrVt,vrtxp1+nrVt,1);
			}

			previous = actual;
			actual = next;
			index++;
		}while(actual!= first);

	}
}

void application_meshParam::setUp( cpuCSRMatrix & mat, MODEL & model )
{
	//mat = model.getLaplace0_mixed();
	mat = model.getLaplace0_ignoreBoundary();
	mat.normalizeLines();
	//////////////////////////////////////////////////////////////////////////
	mat.saveMatrix("meshParamlaplace_new.m");
	//////////////////////////////////////////////////////////////////////////
	std::vector<wingedEdge*> & boundaries = model.getMesh()->getBoundaryEdges();

	
	//iterate over boundary, set to fixed values.
	wingedEdge * edge, * first;
	edge = first = boundaries[0];
	do{
		mat.setLineToID(edge->getFirstBoundaryVertex());
		edge = edge->nextBorderEdge();
	}while(edge != first);

	//setUp for x and y at once
	//though they could be solved for independently for now.
	cpuCSRMatrix temp = mat;
	mat.diagAppend(temp);
	
}

void application_meshParam::setUpB( floatVector& b, MODEL & model, borderStyle calcBorder)
{
	std::vector<wingedEdge*> & boundaries = model.getMesh()->getBoundaryEdges();
	std::vector<tuple3f> borderValues;
	int nrVertices = model.getMesh()->getVertices().size();
	b.assign(2*nrVertices, 0);

	//use the given borderStyle to compute the outer positions of the mesh
	(this->*calcBorder)(*model.getMesh(), borderValues);

	//iterate over the outer boundary, fill in the position constraint for x and y.
	wingedEdge * edge, * first;
	int idx = 0;
	edge = first = boundaries[0];
	do{
		b[edge->getFirstBoundaryVertex()] = borderValues[idx].x; 
		b[edge->getFirstBoundaryVertex() + nrVertices] = borderValues[idx].y; 
		edge = edge->nextBorderEdge();
		idx++;
	}while(edge != first);
}

void application_meshParam::normalizeTexture( std::vector<tuple3f> & target )
{
	float maxX , minX, maxY,minY;
	maxX = minX = target[0].x;
	maxY = minY = target[0].y;
	for(int i = 0; i < target.size(); i++){
		if(target[i].x > maxX)
			maxX = target[i].x;
		else if(target[i].x < minX){
			minX = target[i].x;
		}

		if(target[i].y > maxY)
			maxY = target[i].y;
		else if(target[i].y < minY){
			minY = target[i].y;
		}
	}

	float scale = (maxX-minX>maxY-minY? maxX-minX:maxY-minY);
	scale = 1.f/scale;
	for(int i = 0; i< target.size(); i++){
		target[i].x = (target[i].x - minX) * scale;
		target[i].y = (target[i].y - minX) * scale;
	}
}

void application_meshParam::computeAnglesAndLambdas( wingedMesh & m, std::vector<float> & angles, 
	std::vector<float> & lambdas, int borderComponent, bool outerBorder )
{
	int borderSz = m.getBoundarySizes()[borderComponent];
	angles.clear(); lambdas.clear();
	angles.reserve(borderSz);
	lambdas.reserve(borderSz);


	//iterate once over the boundary
	float totalAngle=0;
	float targetAngle = (outerBorder?(borderSz-2)*PI: (borderSz+2)*PI);
	wingedEdge *first, *edge, *nextEdge;
	//such that the first angle lambda pair corresponds to boundaryEdge[bC].firstVertex
	first= edge = m.getBoundaryEdges()[borderComponent]->previousBorderEdge(); 

	do 
	{
		nextEdge = edge->nextBorderEdge();

		lambdas.push_back(m.length(*nextEdge)/m.length(*edge));
		angles.push_back(m.angle(*nextEdge,*edge));
		totalAngle += angles.back();
		edge = nextEdge;
	} while (first!=edge);

	assert(angles.size() == borderSz);
	//rescale angles
	float scale = targetAngle/totalAngle;
	for(int i = 0; i < angles.size(); i++){
		angles[i] *= scale;
	}
}


class editableEntries:public matrixCreator{
	wingedMesh * myMesh;
	std::vector<bool> isBorderVertice;
		int nrVertices;
public:
	editableEntries(wingedMesh & m){
		myMesh = &m;
		nrVertices = m.getVertices().size();
		isBorderVertice.resize(nrVertices, false);

		wingedEdge * first, *edge;
		for(int i = 1; i < m.getBoundaryEdges().size(); i++){
			first = edge = m.getBoundaryEdges()[i];

			do{
				isBorderVertice[edge->getFirstBoundaryVertex()] = true;
				edge = edge->nextBorderEdge();
			}while(edge!= first);
		}

	}
	virtual float val( int i , int j ) 
	{
		return 0;
	}

	virtual void indices( int row, std::vector<int> & target ) 
	{
		target.clear();
		int vertex = row%nrVertices;

		if(! isBorderVertice[vertex]){
			target.push_back(row);
			return;
		}

		wingedEdge edg = myMesh->getAnEdge(vertex);
		wingedEdge first = edg;

		target.push_back(vertex);
		target.push_back(vertex+nrVertices);

		do{
			target.push_back(edg.otherVertex(vertex));
			target.push_back(edg.otherVertex(vertex)+nrVertices);
			edg = edg.getNext_bc(vertex);
		}while(edg!=first);

		sort(target.begin(), target.end());
	}

};

void application_meshParam::makeMatrixEditable( cpuCSRMatrix & mat, wingedMesh & m )
{
	cpuCSRMatrix temp;
	temp.initMatrix(editableEntries(m), 2*m.getVertices().size());
	mat = mat + temp;
}







