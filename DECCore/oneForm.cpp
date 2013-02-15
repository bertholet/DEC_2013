#include "StdAfx.h"
#include "oneForm.h"
#include "matrixf.h"

oneForm::oneForm(wingedMesh * msh)
{
	myMesh = msh;
	assign(msh->getEdges().size(),0);
}


oneForm::~oneForm(void)
{
}


//////////////////////////////////////////////////////////////////////////
// using the approach consistent with whitney forms
// one vector per face is computed (interpolated for the baricenter)
//////////////////////////////////////////////////////////////////////////
void oneForm::toVField( std::vector<tuple3f> & target )
{
	std::vector<tuple3f> & verts = myMesh->getVertices();
	std::vector<tuple3i> & fcs = myMesh->getFaces();
	std::vector<tuple3i> & f2es = myMesh->getf2e();
	std::vector<wingedEdge> & edges = myMesh->getEdges();


	tuple3i face,f2e;
	tuple3f bari_coord(0.3333333f,0.3333333f,0.333333f);
	tuple3f oneF, result;
	wingedEdge edge1, edge2;
	
	if(target.size()!= fcs.size()){
		target.resize(fcs.size());
	}

	for(unsigned int i = 0; i< fcs.size(); i++){
		face.set(fcs[i]);
		f2e.set(f2es[i]);
		oneF.set((*this)[f2e.a], (*this)[f2e.b],(*this)[f2e.c]);

		p_abT.set(verts[face.b]);
		p_abT -= verts[face.a];

		p_bcT.set(verts[face.c]);
		p_bcT -= verts[face.b];

		p_caT.set(verts[face.a]);
		p_caT -= verts[face.c];

		normal = - p_abT.cross(p_bcT);
		float vol2Triangle = normal.norm();
		normal.normalize();

	/*	matrixf rot = matrixFactory::rotate(PI/2,normal.x, normal.y, normal.z);
		p_abT = rot*p_abT;
		p_bcT = rot*p_bcT;
		p_caT = rot*p_caT;*/
		p_abT= p_abT.cross(normal);
		p_bcT= p_bcT.cross(normal);
		p_caT= p_caT.cross(normal);

		edge1 = (edges)[f2e.a];
		edge2 = (edges)[f2e.b];
		//c_ab	alpha_a				//c_bc		alpha_c
		//tuple3f result = p_caT * (getOneForm(fc2he[faceNr].a)*bara - getOneForm(fc2he[faceNr].b)*barc) +
		result = p_caT * (oneF.x* face.orientation(edge1)*bari_coord.x - 
			oneF.y * face.orientation(edge2)*bari_coord.z);
		//c_bc	alpha_b			//c_ca		alpha_a
		//result += p_abT * (getOneForm(fc2he[faceNr].b)*barb - getOneForm(fc2he[faceNr].c)*bara) +

		edge1 = (edges)[f2e.b];
		edge2 = (edges)[f2e.c];
		result += p_abT * (oneF.y * face.orientation(edge1)*bari_coord.y 
			- oneF.z * face.orientation(edge2)*bari_coord.x);
		//c_ca	alpha_c			//c_ab		alpha_b
		//	result+= p_bcT * (getOneForm(fc2he[faceNr].c)*barc - getOneForm(fc2he[faceNr].a)*barb);

		edge1 = (edges)[f2e.c];
		edge2 = (edges)[f2e.a];
		result+= p_bcT * (oneF.z * face.orientation(edge1)*bari_coord.z
			- oneF.x  * face.orientation(edge2)*bari_coord.y);

		result *= 1.f/vol2Triangle;
		target[i].set(result);
	}
}

void oneForm::dualToVField( std::vector<tuple3f> & target )
{
	std::vector<tuple3i> & fcs = myMesh->getFaces();
	std::vector<tuple3f> & verts = myMesh->getVertices();
	std::vector<tuple3i> & f2e = myMesh->getf2e();
	std::vector<wingedEdge> & edgs = myMesh->getEdges();
	
	target.resize(fcs.size(), tuple3f());

	matrix3f flux2vel;

	//triangle normal
	tuple3f n;
	//flux of actual face
	tuple3f flx;

	for(unsigned int i = 0; i < fcs.size(); i++){
		tuple3f & a = verts[fcs[i].a];
		tuple3f & b = verts[fcs[i].b];
		tuple3f & c = verts[fcs[i].c];

		n = (b-a).cross(c-a);
		n.normalize();



		flux2vel.setRow(0, (b-a).cross(n));
		flux2vel.setRow(1, (c-b).cross(n));
		flux2vel.setRow(2,n);

		flux2vel = flux2vel.inv();

		//flux should sum to 0. 
		flx.x = (*this)[f2e[i].a]* fcs[i].orientation(edgs[f2e[i].a]);
		flx.y = (*this)[f2e[i].b]* fcs[i].orientation(edgs[f2e[i].b]);
		flx.z = 0;


		target[i].set(flux2vel * flx);
	}
}


void oneForm::onesOnBoundary()
{
	int sz = myMesh->getEdges().size();
	resize(sz);
	for(int i = 0; i < sz; i++){
		(*this)[i] = (myMesh->getEdges()[i].isOnBorder()? 1.f:0.f);
	}
}

void oneForm::onesOnInnerEdges()
{
	int sz = myMesh->getEdges().size();
	resize(sz);
	for(int i = 0; i < sz; i++){
		(*this)[i] = (myMesh->getEdges()[i].isOnBorder()? 0.f:1.f);
	}
}