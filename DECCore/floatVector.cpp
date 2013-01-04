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
