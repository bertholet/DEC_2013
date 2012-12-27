#pragma once
#include "wfmesh.h"
#include "DECCORE_EXPORT.h"

class DECCORE_EXPORT ball :
	public wfMesh
{
public:
	ball(float r, int verticesPerHalfCycle);
	ball(float r, int nrPhi, int nrPsi);
	ball(float r, int nrPhi, int nrPsi,bool FUN);
	ball( float r, int nrPhi, int nrPhi2,int nrPsi );
	~ball(void);
};
