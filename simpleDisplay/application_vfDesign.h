#pragma once
#include "MODEL.h"
#include "oneForm.h"

class application_vfDesign
{
private:
	bool adaptToBorder;
public:
	application_vfDesign(void);
	~application_vfDesign(void);

	void setAdaptToBorder(bool what);

	void computeField(MODEL & m, 
		std::vector<int> & source_verts,
		std::vector<int> & sink_verts,
		std::vector<int> & constrainedEdges,
		std::vector<float> & edgeConstraints,
		oneForm & target);
	
};

