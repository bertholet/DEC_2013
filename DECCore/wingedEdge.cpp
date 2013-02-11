#include "StdAfx.h"
#include "wingedEdge.h"
#include <assert.h>

wingedEdge::wingedEdge(void)
{
	preva = NULL;
	nexta = NULL;
	prevb = NULL;
	nextb = NULL;
	myIndex = -1;
	this->v_a_b.set(-1,-1);

}

wingedEdge::wingedEdge( int start, int end )
{
	preva = NULL;
	nexta = NULL;
	prevb = NULL;
	nextb = NULL;
	myIndex = -1;
	this->v_a_b.set(start, end);
}


wingedEdge::~wingedEdge(void)
{
}

void wingedEdge::set( int start, int stop )
{
	this->v_a_b.set(start, stop);
}

int wingedEdge::start()
{
	return v_a_b.a;
}

int wingedEdge::end()
{
	return v_a_b.b;
}

void wingedEdge::setLeftFace( int fc )
{
	fc_p_n.b = fc;
}

void wingedEdge::setRightFace( int fc )
{
	fc_p_n.a = fc;
}


void wingedEdge::setNext( wingedEdge * e )
{
	if(v_a_b.a == e->v_a_b.a || v_a_b.a == e->v_a_b.b){
		nexta = e;
	}
	else if (v_a_b.b == e->v_a_b.a || v_a_b.b == e->v_a_b.b)
	{
		nextb = e;
	}
	else{
		assert(false);
	}
}

void wingedEdge::setPrev( wingedEdge * e )
{
	if(v_a_b.a == e->v_a_b.a || v_a_b.a == e->v_a_b.b){
		preva = e;
	}
	else if (v_a_b.b == e->v_a_b.a || v_a_b.b == e->v_a_b.b)
	{
		prevb = e;
	}
	else{
		assert(false);
	}
}

bool wingedEdge::operator!=( const wingedEdge & other ) const
{
	return !(this->operator==(other) );
}

wingedEdge * wingedEdge::getNext( int vertex )
{
	if(v_a_b.a == vertex){
		return nexta;
	}
	else if( v_a_b.b == vertex ){
		return nextb;
	}
	else{
		assert(false);
		throw std::runtime_error("mesh has a border");
	}
}

wingedEdge * wingedEdge::getPrev( int vertex )
{
	if(v_a_b.a == vertex){
		return preva;
	}
	else if( v_a_b.b == vertex ){
		return prevb;
	}
	else{
		assert(false);
		throw std::runtime_error("mesh has a border");
	}
}

wingedEdge & wingedEdge::getNext_bc( int vertex )
{
	wingedEdge * result= getNext(vertex);

	if(result == NULL){
		result = getPrev(vertex);
		while(result->getPrev(vertex)!=NULL){
			result = result->getPrev(vertex);
		}
	}
	return *result;
}

wingedEdge & wingedEdge::getPrev_bc( int vertex )
{
	wingedEdge * result= getPrev(vertex);
	
	if(result == NULL){
		result = getNext(vertex);
		while(result->getNext(vertex)!=NULL){
			result = result->getNext(vertex);
		}
	}
	return *result;
}

int wingedEdge::orientation( int j )
{
	return v_a_b.orientation(j);
}

tuple2i & wingedEdge::getTuple()
{
	return v_a_b;
}

int wingedEdge::otherVertex( int vertex )
{
	if(vertex == v_a_b.a){
		return v_a_b.b;
	}else if (vertex== v_a_b.b)
	{
		return v_a_b.a;
	}
	assert(false);
	return -1;
}

bool wingedEdge::isOnBorder()
{
	//assert((preva == NULL) || (prevb == NULL));// == (fc_p_n.a ==-1 ||fc_p_n.b ==-1));
	//return preva == NULL || prevb == NULL;
	return fc_p_n.a==-1 || fc_p_n.b == -1;
}


bool wingedEdge::isFirstEdge( int vertex )
{
	if(v_a_b.a == vertex){
		return preva == NULL;
	}
	else if( v_a_b.b == vertex ){
		return prevb == NULL;
	}
	else{
		return false;

	}
}

bool wingedEdge::isLastEdge( int vertex )
{
	if(v_a_b.a == vertex){
		return nexta == NULL;
	}
	else if( v_a_b.b == vertex ){
		return nextb == NULL;
	}
	else{
		return false;
	}
}

bool wingedEdge::operator<( const wingedEdge & other ) const
{
	return v_a_b < other.v_a_b;
}

bool wingedEdge::operator==( const wingedEdge & other ) const
{
	return v_a_b == other.v_a_b;
}

void wingedEdge::setIndex( int ind )
{
	myIndex = ind;
}

int wingedEdge::getIndex()
{
	return myIndex;
}

wingedEdge * wingedEdge::previousBorderEdge()
{
	//To get the orientation induced by the manifold it is
	//enough to know that next and previous face are ordered 
	//according to the orientation of the manifold.

	//next face oriented opposite to edge.
	//so, looking for next border edge at start vertex
	if(fc_p_n.a == -1){
		assert(preva == NULL);
		return & getPrev_bc(start());
	}
	//next fce oriented according to edge
	else if(fc_p_n.b == -1){
		assert(prevb== NULL);
		return & getPrev_bc(end());
	}
	else{
		return NULL;
	}
}


wingedEdge * wingedEdge::nextBorderEdge()
{
	//To get the orientation induced by the manifold it is
	//enough to know that next and previous face are ordered 
	//according to the orientation of the manifold.

	//prev face oriented opposite to edge.
	//so, looking for next border edge at start vertex
	if(fc_p_n.a == -1){
		assert(nextb == NULL);
		return & getNext_bc(end());
	}
	//next fce oriented according to edge
	else if(fc_p_n.b == -1){
		assert(nexta== NULL);
		return & getNext_bc(start());
	}
	else{
		return NULL;
	}
}


int wingedEdge::getFirstBoundaryVertex()
{
	if(fc_p_n.a == -1){
		return start();
	}
	else if(fc_p_n.b == -1){
		return end();
	}
	else{
		//invalid use of this method.
		assert(false);
		return -1;
	}
}

int wingedEdge::and( wingedEdge & other )
{
	if(start() == other.start() || start() == other.end()){
		return start();
	}
	else if(end() == other.start() || end() == other.end()){
		return end();
	}
	return -1;
}

tuple2i & wingedEdge::getAdjFaces()
{
	return fc_p_n;
}

int wingedEdge::commonFace( wingedEdge & other )
{
	if(fc_p_n.a>=0 && (fc_p_n.a == other.fc_p_n.a || fc_p_n.a == other.fc_p_n.b)){
		return fc_p_n.a;
	}
	else if(fc_p_n.b>=0 &&(fc_p_n.b == other.fc_p_n.a || fc_p_n.b == other.fc_p_n.b)){
		return fc_p_n.b;
	}
	return -1;
}





