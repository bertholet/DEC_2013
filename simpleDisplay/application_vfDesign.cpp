#include "application_vfDesign.h"


application_vfDesign::application_vfDesign(void)
{
}


application_vfDesign::~application_vfDesign(void)
{
}


void application_vfDesign::computeField( MODEL & m, 
	std::vector<int> & source_verts, 
	std::vector<int> & sink_verts, 
	std::vector<int> & constrainedEdges, 
	std::vector<float> & edgeConstraints, 
	oneForm & target )
{
  
	cpuCSRMatrix mat = m.getBorder2()* m.getStar2() * m.getD1();
	cpuCSRMatrix duald1_border = m.getBorder1();
	mat = mat +(m.getStar1()*cpuCSRMatrix::transpose(duald1_border) *m.getStar0_inv() * (duald1_border) *m.getStar1());

	//mat_border =	DDGMatrices::dual_d0(*msh) * DDGMatrices::star2(*msh) * DDGMatrices::d1(*msh);
}
