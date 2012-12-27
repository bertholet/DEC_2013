#pragma once
#include "DDGMatrices.h"
#include <vector>
#include "MODEL.h"

class application_smoothing
{
public:
	application_smoothing(wingedMesh & m);
	~application_smoothing(void);
	void explicitEuler(MODEL * model, float timeStep);
private :
	cpuCSRMatrix Laplace0_mixed;
	wingedMesh * myMesh;
	std::vector<tuple3f> buffer;
};

