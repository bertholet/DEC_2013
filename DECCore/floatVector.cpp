#include "StdAfx.h"
#include "floatVector.h"


floatVector::floatVector(void)
{
}

floatVector::floatVector( std::vector<tuple3f> & vals, int what )
{
	for(unsigned int i =0; i < vals.size(); i++){
		this->push_back(vals[i][what]);
	}
}


floatVector::~floatVector(void)
{
}

void floatVector::set( std::vector<tuple3f> & vals, int what )
{
	if(vals.size() != this->size()){
		this->resize(vals.size(), 0);
	}

	for(unsigned int i = 0; i < size(); i++){
		(*this)[i] = vals[i][what];
	}
}
