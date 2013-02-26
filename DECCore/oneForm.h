#pragma once
#include "DECCORE_EXPORT.h"
#include "floatVector.h"
#include "wingedMesh.h"
#include "tuple3.h"

class DECCORE_EXPORT oneForm: public floatVector
{
public:
	oneForm(wingedMesh * mesh);
	~oneForm(void);

	void onesOnBoundary();
	void onesOnInnerEdges();
	//conversion to vectorfield, interpreting the one form as flow values
	void toVField(std::vector<tuple3f> & target);
	//conversion to vectorfield, interpreting the one form as flux values
	void dualToVField(std::vector<tuple3f> & target);
	void setZero();
private:
	wingedMesh *myMesh;

	tuple3f  p_abT, p_bcT, p_caT, normal;
};

