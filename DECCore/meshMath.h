#pragma once
#include "wingedMesh.h"
#include <vector>
#include "DECCORE_EXPORT.h"
#include "cpuCSRMatrix.h"

class DECCORE_EXPORT meshMath
{
public:
	enum dualEdgeType {START_VERTEX_BORDER_COMPLETION, END_VERTEX_BORDER_COMPLETION, NO_COMPLETION};

	meshMath(void);
	~meshMath(void);

	static float area( int face, wingedMesh & mesh );
	static float volume(wingedMesh &m);

	static float aVoronoi(int vertex, wingedMesh & mesh);
	static float dualEdge_edge_ratio( int edge, wingedMesh & mesh );
	//dual edge edge ratio if the dual mesh is defined using mixed areas.
	static float dualEdge_edge_ratio_mixed( int edge, wingedMesh & mesh, dualEdgeType what = NO_COMPLETION );
	static void dualEdge_edge_ratios_mixed(wingedMesh & mesh, std::vector<float> & target);
	

	static void calcAllMixedAreas( wingedMesh &m, std::vector<float> &AMixed );
	static void calcAllMixedAreas_ignoreBorder( wingedMesh &m, std::vector<float> &AMixed );

	static void curvatureNormals(wingedMesh & m, std::vector<tuple3f> & target,
		cpuCSRMatrix & d0_coderiv1mixed, std::vector<float> & buffer);

	static void circumcenters(wingedMesh & mesh, std::vector<tuple3f> & target );
	static void centroids( wingedMesh & mesh, std::vector<tuple3f> & target );
};

