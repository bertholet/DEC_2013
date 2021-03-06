#pragma once
#include <vector>
#include "DECCORE_EXPORT.h"
#include "tuple3.h"

class DECCORE_EXPORT floatVector:public std::vector<float>
{
public:
	floatVector(void);
	floatVector(int sz);
	//make a vector of floats of the what = 0,1,2 component of vals
	floatVector(std::vector<tuple3f> & vals, int what);
	~floatVector(void);

	void set(std::vector<tuple3f> & vals, int what);	
	void add(floatVector & other, float scale );


	void saveVector(std::string  name, 
		std::string  file );
	//not compatible to savevector
	void loadVector(std::string  file );

	static void saveTuple3iVectorMatlabStyle(std::vector<tuple3i> & vals,std::string  name, 
		std::string  file );

};

