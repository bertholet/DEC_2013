#pragma once
#include <vector>
#include "MODEL.h"

class cpuCSRMatrix;

class application_meshParam
{
typedef int (application_meshParam::*borderStyle)(wingedMesh &, std::vector<tuple3f> &);

public:
	application_meshParam(void);
	~application_meshParam(void);

	//compute a conformal mapping either on a mesh with one or with
	//multiple boundaries
	void calcTexturePos(MODEL & model,borderStyle s = & circleBorder);
	void calcTexturePosMultiborder(MODEL & model,borderStyle s = & circleBorder);

	//alternative boundary position computations
	int circleBorder(wingedMesh &m, std::vector<tuple3f> & target);
	int distWeightCircleBorder(wingedMesh &m, std::vector<tuple3f> & target);
	int conformalBorder(wingedMesh &m, std::vector<tuple3f> & target);

	//compute the edge length ratios and the angles of a border component
	//they are stored in angles and lambdas.
	static void computeAnglesAndLambdas(wingedMesh & m, std::vector<float> & angles, std::vector<float> & lambdas, 
		int borderComponent, bool outerBorder);


private:

	//method that takes care of the matrix set up, i.e. the
	//A in Ax =b
	void setUp( cpuCSRMatrix & mat, MODEL & m );
	//method that takes care to get the constraints right,
	//i.e. the b in Ax = b
	void setUpB( floatVector& b, MODEL & m//);//, 
		,borderStyle s = & circleBorder);
	//method to adapt the constraints on inner boundaries and 
	//reflex vertices
	void adaptConstraintsInnerBorder( cpuCSRMatrix & mat, wingedMesh & m );

	//helper methods
	void normalizeTexture( std::vector<tuple3f> & target );
	void makeMatrixEditable( cpuCSRMatrix & mat, wingedMesh & m );
};

