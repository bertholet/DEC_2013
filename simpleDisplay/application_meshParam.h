#pragma once
#include <vector>
#include "wingedMesh.h"

class application_meshParam
{
public:
	application_meshParam(void);
	~application_meshParam(void);

	static void circleBorder(wingedMesh &m, std::vector<tuple3f> & target);
	/*void distWeightCircBorder( vector<tuple3f> & outerPos , vector<int> & border, mesh &);
	void angleApproxBorder( vector<tuple3f> & outerPos , vector<int> & border, mesh & m);*/

};

