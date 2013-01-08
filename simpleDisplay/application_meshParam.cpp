#include "application_meshParam.h"


application_meshParam::application_meshParam(void)
{
}


application_meshParam::~application_meshParam(void)
{
}

void application_meshParam::circleBorder( wingedMesh &m, std::vector<tuple3f> & target )
{
	if(m.getBoundaryEdges().size() ==0){
		return;
	}
	wingedEdge *outer = m.getBoundaryEdges()[0], * temp;
	target.clear();

	for(int i = 0; i < sz;i++){
		target.push_back(tuple3f((sin(-i*2* PI/sz)+1.f)/2.f,(cos(-i*2* PI/sz)+1.f)/2.f,0.f));
	}
}
