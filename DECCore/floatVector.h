#pragma once
#include <vector>
#include "DECCORE_EXPORT.h"
#include "tuple3.h"

class DECCORE_EXPORT floatVector:public std::vector<float>
{
public:
	floatVector(void);
	//make a vector of floats of the what = 0,1,2 component of vals
	floatVector(std::vector<tuple3f> & vals, int what);
	~floatVector(void);

	void set(std::vector<tuple3f> & vals, int what);
};

