#pragma once
#include "MODEL.h"
#include "oneForm.h"

class application_vfDesign
{
public:
	application_vfDesign(void);
	~application_vfDesign(void);

	void computeField(MODEL & m, 
		std::vector<int> & source_verts,
		std::vector<int> & sink_verts,
		std::vector<int> & constrainedEdges,
		std::vector<float> & edgeConstraints,
		oneForm & target);
};

