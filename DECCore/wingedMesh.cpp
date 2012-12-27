#include "StdAfx.h"
#include "wingedMesh.h"
#include <algorithm>


wingedMesh::wingedMesh(wfMesh * theMesh)
{
	myMesh = theMesh;
	initEdges();
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

	wingedEdge halfedge;
	
	//the raw edges
	for(it = faces.begin(); it!= faces.end(); it++){

		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		if(el==edges.end() || el->v_a_b.a != halfedge.v_a_b.a || el->v_a_b.b != halfedge.v_a_b.b){
			edges.insert(el,halfedge);
		}

		//halfedge.set((*it).b, (*it).c);
		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		if(el==edges.end() || el->v_a_b.a != halfedge.v_a_b.a || el->v_a_b.b != halfedge.v_a_b.b){
			edges.insert(el,halfedge);
		}

		//halfedge.set((*it).c, (*it).a);
		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		if(el==edges.end() || el->v_a_b.a != halfedge.v_a_b.a || el->v_a_b.b != halfedge.v_a_b.b){
			edges.insert(el,halfedge);
		}
	}

	//the previous and next edges
	//assumes an oriented wf mesh
	wingedEdge * e1, *e2, *e3;
	for(it = faces.begin(); it!= faces.end(); it++){
		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));
		e1 = & edges[edgeIndex(halfedge)];
		
		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		e2 = & edges[edgeIndex(halfedge)];


		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		e3 = & edges[edgeIndex(halfedge)];

		e1->setNext(e3);
		e1->setPrev(e2);

		e2->setNext(e1);
		e2->setPrev(e3);

		e3->setNext(e2);
		e3->setPrev(e1);
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


	//fill in f2e
	for(it = faces.begin(); it!= faces.end(); it++){

		halfedge.set((it->a < it->b ? (*it).a: (*it).b),
			(it->a < it->b ? (*it).b: (*it).a));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		f2e_.a =  (el - edges.begin());

		//halfedge.set(it->b, it->c);
		//		sign = (it->b < it->c ? 1: -1);
		halfedge.set((it->b < it->c ? (*it).b: (*it).c),
			(it->b < it->c ? (*it).c: (*it).b));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		f2e_.b = (el - edges.begin());

		//halfedge.set(it->c, it->a);
		//		sign = (it->c < it->a ? 1: -1);
		halfedge.set((it->a < it->c ? (*it).a: (*it).c),
			(it->a < it->c ? (*it).c: (*it).a));
		el = lower_bound(edges.begin(),edges.end(), halfedge);
		f2e_.c = (el - edges.begin());
		f2e.push_back(f2e_);
	}


	//init v2e
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
	return lower_bound(edges.begin(),edges.end(), edge) - edges.begin();
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



