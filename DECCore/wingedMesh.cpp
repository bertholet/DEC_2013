#include "StdAfx.h"
#include "wingedMesh.h"
#include <algorithm>
#include "meshMath.h"
#include <limits>

wingedMesh::wingedMesh(wfMesh * theMesh)
{
	myMesh = theMesh;
	initEdges();
	findBoundary();
	checkAreaRatios();
	valid = true;
}

wingedMesh::wingedMesh()
{
	myMesh = new wfMesh();
}


wingedMesh::~wingedMesh(void)
{
	delete myMesh;
}


void wingedMesh::initEdges(void)
{
	vector<tuple3i> & faces = myMesh->getFaces();
	vector<tuple3i>::iterator it;
	vector<wingedEdge>::iterator el;
	edges.clear();
	f2e.clear();

	edges.reserve(faces.size() + myMesh->getVertices().size() + 100);
	f2e.reserve(faces.size());

	wingedEdge halfedge;
	
	//the raw edges
	for(it = faces.begin(); it!= faces.end(); it++){
		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));
		edges.push_back(halfedge);

		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		edges.push_back(halfedge);

		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		edges.push_back(halfedge);

	}

	//sort and remove duplicates and assign indices to the edges
	sort(edges.begin(), edges.end());
	int index = 0;
	halfedge.set(-1,-1);
	for(unsigned int i = 0; i< edges.size(); i++){
		if(halfedge!= edges[i]){
			halfedge = edges[i];
			edges[index]=halfedge;
			edges[index].setIndex(index);
			index++;
		}
	}
	edges.resize(index);

	//the previous and next edges
	//assumes an oriented wf mesh
	wingedEdge * e1, *e2, *e3;
	for(it = faces.begin(); it!= faces.end(); it++){
		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));

		//e1 = & edges[edgeIndex(halfedge)];
		e1 = edgePointer(halfedge);

		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		//e2 = & edges[edgeIndex(halfedge)];
		e2 = edgePointer(halfedge);

		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		//e3 = & edges[edgeIndex(halfedge)];
		e3 = edgePointer(halfedge);

		if(e1!=NULL){
			e1->setNext(e3);
			e1->setPrev(e2);
		}
		if(e2!= NULL){
			e2->setNext(e1);
			e2->setPrev(e3);
		}
		if(e3!=NULL){
			e3->setNext(e2);
			e3->setPrev(e1);
		}
	}
	

	//face 2 edge set up
	tuple3i f2e_;
	for(it = faces.begin(); it!= faces.end(); it++){

		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));
		f2e_.a =edgeIndex(halfedge);

		//halfedge.set(it->b, it->c);
		//		sign = (it->b < it->c ? 1: -1);
		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		f2e_.b = edgeIndex(halfedge);

		//halfedge.set(it->c, it->a);
		//		sign = (it->c < it->a ? 1: -1);
		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		f2e_.c = edgeIndex(halfedge);
		f2e.push_back(f2e_);
	}


	//edges: previous face, next face, setup
	//both have been initialized with -1
	int orientation;
	int j;
	for(unsigned int i = 0; i < faces.size(); i++){
		tuple3i & fc = faces[i];
		tuple3i & edg = f2e[i];

		for(j=0; j < 3; j++){
			orientation = fc.orientation(edges[edg[j]]);
		
			if(orientation== 1){
				edges[edg[j]].setLeftFace(i);
			}
			else if(orientation == -1){
				edges[edg[j]].setRightFace(i);
			}
			else{
				assert(false);
			}
		}
	}


	//init v2e, note: they are automatically sorted.
	int nrVertices = myMesh->getVertices().size();
	v2e.clear();
	v2e.reserve(nrVertices);
	std::vector<int> aVector;
	for(int i = 0 ; i < nrVertices; i++){
		v2e.push_back(aVector);
	}

	for(unsigned int i = 0; i < edges.size(); i++){
		v2e[edges[i].start()].push_back(i);
		v2e[edges[i].end()].push_back(i);
	}

}


int wingedMesh::edgeIndex( wingedEdge & edge )
{
	int index = lower_bound(edges.begin(),edges.end(), edge) - edges.begin();
	return (index ==edges.size()||edges[index]!=edge ? -1:index);
}

wingedEdge * wingedMesh::edgePointer( wingedEdge & halfedge )
{
	int idx = edgeIndex(halfedge);
	if(idx <0){
		return NULL;
	}
	return &edges[idx];
}



void wingedMesh::update( void * src, meshMsg msg )
{
	if(src == myMesh && msg ==CONNECTIVITY_CHANGED){
		valid = false;
	}
}

std::vector<wingedEdge> & wingedMesh::getEdges()
{
	return edges;
}

std::vector<std::vector<int>> & wingedMesh::getv2e()
{
	return v2e;
}

std::vector<tuple3i> & wingedMesh::getFaces()
{
	return myMesh->getFaces();
}

std::vector<tuple3i> & wingedMesh::getf2e()
{
	return f2e;
}

std::vector<tuple3f>&  wingedMesh::getVertices()
{
	return myMesh->getVertices();
}

bool wingedMesh::isValid()
{
	return valid;
}

wingedEdge & wingedMesh::getAnEdge( int vertex )
{
	return edges[v2e[vertex][0]];
}

wfMesh* wingedMesh::getWfMesh()
{
	return this->myMesh;
}

void wingedMesh::checkAreaRatios()
{
	float maxVoronoi=0, minVoronoi = numeric_limits<float>::infinity();
	float maxArea = 0, minArea = numeric_limits<float>::infinity();
	float maxMixedArea = 0, minMixedArea = numeric_limits<float>::infinity();
	float temp;

	for(unsigned int i = 0; i < myMesh->getVertices().size(); i++){
		temp = meshMath::aVoronoi(i,*this);
		if(temp > maxVoronoi){
			maxVoronoi = temp;
		}
		if(temp < minVoronoi){
			minVoronoi = temp;
		}
	}

	for(unsigned int i = 0; i < myMesh->getFaces().size(); i++){
		temp = meshMath::area(i,*this);
		if(temp > maxArea){
			maxArea = temp;
		}
		if(temp < minArea){
			minArea = temp;
		}
	}


	std::vector<float> mixedAreas;
	meshMath::calcAllMixedAreas(*this,mixedAreas);

	for(unsigned int i = 0; i < myMesh->getVertices().size(); i++){
		temp = mixedAreas[i];
		if(temp > maxMixedArea){
			maxMixedArea = temp;
		}
		if(temp < minMixedArea){
			minMixedArea = temp;
		}
	}


	cout << " Voronoi Areas: \n" << "Max: \t" << maxVoronoi << "\tMin: \t" << minVoronoi << "\tRatio:\t"<< maxVoronoi/minVoronoi <<"\n";
	cout << " Mixed Voronoi Areas: \n" << "Max: \t" << maxMixedArea << "\tMin: \t" << minMixedArea << "\tRatio:\t"<< maxMixedArea/minMixedArea <<"\n";
	cout << "Areas: \n" << "Max: \t" << maxArea << "\tMin: \t" << minArea << "\tRatio:\t"<< maxArea/minArea <<"\n";
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Boundary Methods
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void wingedMesh::findBoundary()
{
	bool * marked = new bool[edges.size()];
	for(unsigned int i = 0; i < edges.size(); i++){
		marked[i] = false;
	}

	boundaryEdges.clear();
	boundarySizes.clear();

	wingedEdge temp;
	int borderSize;
	for(unsigned int i = 0; i < edges.size(); i++){
		if(marked[i]==false){
			if(edges[i].isOnBorder()){
				borderSize = 0;
				boundaryEdges.push_back(& (edges[i]));
				temp = edges[i];

				while(!marked[temp.getIndex()]){
					borderSize++;
					marked[temp.getIndex()] = true;
					temp = *temp.nextBorderEdge();
					assert(temp.isOnBorder());
				}

				boundarySizes.push_back(borderSize);

			}
			marked[i] = true;
		}
	}

	putOuterBoundaryToPositionZero();


	cout << "Border Components:\t" << boundaryEdges.size() << " \n";

	delete[] marked;
}

void wingedMesh::putOuterBoundaryToPositionZero()
{
	if(boundaryEdges.size()>0){
		int max = boundarySizes[0];
		int indexMax = 0;
		for(int i = 0; i < boundarySizes.size(); i++){
			if(boundarySizes[i]> max ){
				max = boundarySizes[i];
				indexMax = i;
			}
		}
		wingedEdge * anEdge = boundaryEdges[0];
		int sz = boundarySizes[0];
		boundaryEdges[0]= boundaryEdges[indexMax];
		boundarySizes[0]= boundarySizes[indexMax];
		boundaryEdges[indexMax] = anEdge;
		boundarySizes[indexMax] = sz;
	}
}

std::vector<wingedEdge*> & wingedMesh::getBoundaryEdges()
{
	return boundaryEdges;
}

std::vector<int> & wingedMesh::getBoundarySizes()
{
	return boundarySizes;
}

float wingedMesh::length( wingedEdge & edge )
{
	return (myMesh->getVertices()[edge.end()] - myMesh->getVertices()[edge.start()]).norm();
}

float wingedMesh::calcBorderLength( int borderNr )
{
	if(borderNr >= boundaryEdges.size()){
		return 0;
	}
	wingedEdge * start, * edge;
	float ln=0;
	edge = start = boundaryEdges[borderNr];
	do{
		ln+= length(*edge);
		edge = edge->nextBorderEdge();
	}while(start!=edge);

	return ln;
}

float wingedMesh::angle( wingedEdge & first, wingedEdge & last )
{
	std::vector<tuple3f> & verts = myMesh->getVertices();
	int vertex= first.and(last);
	/*int actual = first.otherVertex(vert);
	int nextVertex;
	int last = last.otherVertex(vert);*/

	tuple3f &center = verts[vertex];

	float angle = 0;
	//iterate over 1-Neighborhood.
	wingedEdge * edg= &first, *nextEdg; 
	do 
	{
		nextEdg = edg->getNext(vertex);
		assert(nextEdg != NULL);
		angle += flatAngle(*edg, *nextEdg);
		edg = nextEdg;
	} while (*nextEdg != last);

	return angle;
}

float wingedMesh::flatAngle( wingedEdge & edg, wingedEdge & nextEdg )
{
	int vertex = edg.and(nextEdg);
	int last = edg.otherVertex(vertex);
	int next = nextEdg.otherVertex(vertex);

	std::vector<tuple3f> & verts = myMesh->getVertices();
	return tuple3f::angle(verts[last], verts[vertex], verts[next]);
}

bool wingedMesh::isOnBorder( int vertex )
{
	std::vector<int> & edgs = v2e[vertex];

	for(int i = 0; i < edgs.size(); i++){
		if(edges[edgs[i]].isOnBorder()){
			return true;
		}
	}
	return false;
}









