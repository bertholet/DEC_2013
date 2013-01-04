#include "StdAfx.h"
#include "wingedEdge.h"


wingedEdge::wingedEdge(void)
{
	preva = NULL;
	nexta = NULL;
	prevb = NULL;
	nextb = NULL;
	this->v_a_b.set(-1,-1);

}

wingedEdge::wingedEdge( int start, int end )
{
	preva = NULL;
	nexta = NULL;
	prevb = NULL;
	nextb = NULL;
	this->v_a_b.set(start, end);
}


wingedEdge::~wingedEdge(void)
{
}

bool wingedEdge::operator<( const wingedEdge & other ) const
{
	return v_a_b < other.v_a_b;
}

bool wingedEdge::operator==( const wingedEdge & other ) const
{
	return v_a_b == other.v_a_b;
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

wingedEdge & wingedEdge::getNext( int vertex )
{
	if(v_a_b.a == vertex){
		return *nexta;
	}
	else if( v_a_b.b == vertex ){
		return *nextb;
	}
	else{
		assert(false);
		throw std::runtime_error("mesh has a border");
	}
}

wingedEdge & wingedEdge::getPrev( int vertex )
{
	if(v_a_b.a == vertex){
		return *preva;
	}
	else if( v_a_b.b == vertex ){
		return *prevb;
	}
	else{
		assert(false);

	}
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


