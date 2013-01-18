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

	void calcTexturePos(MODEL & model,borderStyle s = & circleBorder);
	void calcTexturePosMultiborder(MODEL & model,borderStyle s = & circleBorder);

	int circleBorder(wingedMesh &m, std::vector<tuple3f> & target);
	int distWeightCircleBorder(wingedMesh &m, std::vector<tuple3f> & target);
	int conformalBorder(wingedMesh &m, std::vector<tuple3f> & target);

	void setUp( cpuCSRMatrix & mat, MODEL & m );

	void setUpB( floatVector& b, MODEL & m//);//, 
				,borderStyle s = & circleBorder);

	//compute the edge length ratios and the angles of a border component
	//they are stored in angles and lambdas.
	static void computeAnglesAndLambdas(wingedMesh & m, std::vector<float> & angles, std::vector<float> & lambdas, 
		int borderComponent, bool outerBorder);

private:
	void normalizeTexture( std::vector<tuple3f> & target );
	void adaptConstraintsInnerBorder( cpuCSRMatrix & mat, wingedMesh & m );
	void makeMatrixEditable( cpuCSRMatrix & mat, wingedMesh & m );
};

