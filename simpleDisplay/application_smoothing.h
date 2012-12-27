#pragma once
#include "DDGMatrices.h"
#include <vector>
#include "MODEL.h"

class application_smoothing
{
public:
	application_smoothing();
	~application_smoothing(void);
	void explicitEuler(MODEL * model, float timeStep);
private :
	std::vector<tuple3f> buffer;
};

