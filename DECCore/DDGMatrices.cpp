#include "stdafx.h"
#include "DDGMatrices.h"
#include <vector>
#include "matrixCreator.h"
#include "tuple3.h"
#include "meshMath.h"
#include <algorithm>
#include <limits>
#include "idCreator.h"
//#include "meshOperation.h"


class d_0Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	d_0Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		// i is the row, j the vertex
		wingedEdge & edge =mesh->getEdges()[i];
		return (float) edge.orientation(j);
	}

	// row: its the edge number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		wingedEdge & edge = mesh->getEdges()[row];
		if( edge.start() < edge.end()){
			target.push_back(edge.start());
			target.push_back(edge.end());
		}
		else{
			target.push_back(edge.end());
			target.push_back(edge.start());
		}
	}
};

class border1_Creator: public matrixCreator
{

	wingedMesh * mesh;
	bool halfedgeMode;
	int nrEdges;

public:
	border1_Creator(wingedMesh & aMesh){
		mesh = & aMesh;
		halfedgeMode = false;
		nrEdges= aMesh.getEdges().size();
	}

	float val(int i , int j){
		// i is the row, j the edge
		wingedEdge & edge =mesh->getEdges()[(halfedgeMode?j%nrEdges: j)];
		return (float) edge.orientation(i);
	}

	void setHalfedgeMode(bool what){
		halfedgeMode = what;
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target =  mesh->getv2e()[row];

		if(halfedgeMode){
			std::vector<wingedEdge> & edges = mesh->getEdges();
			int orientation;
			for(unsigned int i = 0; i < target.size(); i++){
				orientation = edges[target[i]].orientation(row);
				assert(orientation != 0);
				if(orientation == 1){
					target[i]+=edges.size();
				}
			}
			sort(target.begin(), target.end());
		}
	}
};

class d_1Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	d_1Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		// i is the row
		tuple3i & fc =(mesh->getFaces())[i];
		
		return (float) fc.orientation((mesh->getEdges())[j].v_a_b);
	}

	// row: its the face number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		tuple3i & f2e =(mesh->getf2e())[row];
		target.push_back(f2e.a);
		target.push_back(f2e.b);
		target.push_back(f2e.c);

		sort(target.begin(), target.end());
	}
};

class border2_Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	border2_Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		// i is the row, i.e. the edge
		// j is the face
		// returns the relative orientation
		tuple3i & fc =(mesh->getFaces())[j];

		return (float) fc.orientation((mesh->getEdges())[i].v_a_b);
	}

	// row: is the edge number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		tuple2i & nbrFcs = mesh->getEdges()[row].getAdjFaces();

		if(nbrFcs.a>=0){
			if(nbrFcs.b> nbrFcs.a){
				target.push_back(nbrFcs.a);
				target.push_back(nbrFcs.b);
			}
			else if(nbrFcs.b >= 0){
				target.push_back(nbrFcs.b);
				target.push_back(nbrFcs.a);
			}
			else{
				target.push_back(nbrFcs.a);
			}
		}
		else{
			assert(nbrFcs.b >= 0);
			target.push_back(nbrFcs.b);
		}

	}
};


class star0Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	star0Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}


		// i is the row
		float temp =meshMath::aVoronoi(i, *mesh);
		assert(temp < numeric_limits<float>::infinity());
		assert(temp > -numeric_limits<float>::infinity());

		return temp;
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};


class star1Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	star1Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}

		//tuple2i & edge = (*mesh->getHalfedges())[i];
		// i is the row
		return meshMath::dualEdge_edge_ratio(i,*mesh);
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};

class star1mixedCreator: public matrixCreator
{
private:
	wingedMesh * mesh;
	meshMath::dualEdgeType type;

public:

	star1mixedCreator(wingedMesh & aMesh){
		mesh = & aMesh;
		type =meshMath::NO_COMPLETION;
	}

	void set(meshMath::dualEdgeType what){
		type= what;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}

		//tuple2i & edge = (*mesh->getHalfedges())[i];
		// i is the row
		return meshMath::dualEdge_edge_ratio_mixed(i,*mesh, type);
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};

class star2Creator: public matrixCreator
{

	wingedMesh * mesh;

public:
	star2Creator(wingedMesh & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}

		// i is the row
		return 1.f / meshMath::area(i,*mesh);
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};

class Op5sgnij_plus_0p5sgnjk_creator: public matrixCreator
{
wingedMesh * myMesh;
std::vector<wingedEdge> & edges;
int nrColumns;
public:
	Op5sgnij_plus_0p5sgnjk_creator(wingedMesh & aMesh):edges(aMesh.getEdges())
	{
		myMesh = & aMesh;
		nrColumns = myMesh->getEdges().size();
	}

	virtual float val( int i , int j ) 
	{
		//i is the vertex, j the edge;
		wingedEdge & edge =edges[j];
		if(!edge.isOnBorder()){
			return 0 ;
		}

		if(edge.start() != i && edge.end() != i ){
			return 0;
		}
		if(edge.getAdjFaces().a <0){
			return -0.5;
		}
		else if(edge.getAdjFaces().b<0){
			return 0.5;
		}
		return 0;

	}


	virtual void indices( int row, std::vector<int> & target ) 
	{
		//row is a vertex i.e dual face
		target.clear();
		//target.push_back(min(row, nrColumns));

		//pushback the potentially 2 adjacent boundary edges 
		if(myMesh->isOnBorder(row)){
			std::vector<int> & nbr_edgs = myMesh->getv2e()[row];
			std::vector<wingedEdge> & edges = myMesh->getEdges();
			for(unsigned int i = 0; i< nbr_edgs.size(); i++){
				if(edges[nbr_edgs[i]].isOnBorder()){
					target.push_back(nbr_edgs[i]);
				}
			}
		}
		sort(target.begin(), target.end());
	}


};

class primal2dual_approx_creator:public matrixCreator
{
private:
	wingedMesh * myMesh;
	std::vector<wingedEdge> & edges;
	std::vector<tuple3f> & verts;
	std::vector<tuple3i> & fcs;
public:
	primal2dual_approx_creator(wingedMesh &aMesh):
	edges(aMesh.getEdges()), verts(aMesh.getVertices()), fcs(aMesh.getFaces())
	{
		myMesh = &  aMesh;
	}
	virtual float val( int i , int j ) 
	{
		if(i==j){
			return 0;
		}
		wingedEdge & edge = edges[i];
		wingedEdge & other = edges[j];

		int commonVertex = edge.and(other); 
		if(commonVertex < 0){
			return 0;
		}

		int commonFace = edge.commonFace(other);
		if(commonFace < 0){
			return 0;
		}

		return tuple3f::cotPoints(verts[other.otherVertex(commonVertex)],
			verts[commonVertex],
			verts[edge.otherVertex(commonVertex)]) * fcs[commonFace].orientation(edge);

		//i is the index of the edge for which the dual is computed
		//returned is the weight for the contribution of j
		//this is the cotan of the angle oposite to 'other' times the orientation
		//of the edge in the triangle 

	}


	virtual void indices( int row, std::vector<int> & target ) 
	{
		target.clear();
	//	target.push_back(row);
		wingedEdge & edge  = edges[row];
		
		if(!edge.isOnBorder()){
			return;
		}



		if(edge.getAdjFaces().a <0){
			target.push_back(edge.getNext(edge.start())->getIndex());
			target.push_back(edge.getPrev(edge.end())->getIndex());
		}
		else if(edge.getAdjFaces().b <0){
			target.push_back(edge.getNext(edge.end())->getIndex());
			target.push_back(edge.getPrev(edge.start())->getIndex());
		}
		sort(target.begin(), target.end());
	}
};

class diagCreator: public matrixCreator
{

	std::vector<float> * vals;

public:
	diagCreator(std::vector<float> & values){
		vals = & values;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}

		// i is the row
		return (*vals)[i];
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};

/*class dualVal1Creator: public matrixCreator
{

	meshMetaInfo * mesh;

public:
	dualVal1Creator(meshMetaInfo & aMesh){
		mesh = & aMesh;
	}

	// i is the edge for which the dual value shall be calculated.§
	float val(int i , int j){
		if(i== j){
			return 0;
		}

		// i is the row
		tuple2i & edg = (* mesh->getHalfedges())[i];
		int nbr_fc1, nbr_fc2;
		meshOperation::getNbrFaces(edg,&nbr_fc1,&nbr_fc2, mesh->getBasicMesh().getNeighborFaces());
		
		//if i not on the border.
		if(nbr_fc2 >=0 ){
			return 0;
		}


		tuple3i he = (* mesh->getFace2Halfedges())[nbr_fc1];
		tuple3i fc = mesh->getBasicMesh().getFaces()[nbr_fc1];		


		std::vector<tuple3f> & verts = mesh->getBasicMesh().getVertices();

		float a_ij = 0;
		int sign=1;
		if(j== he.a){
			a_ij = tuple3f::cotPoints(verts[fc.b],verts[fc.c],verts[fc.a]);
			sign = (i == he.b? -1: 1);
			sign *= fc.orientation((* mesh->getHalfedges())[he.a]);
			sign *= fc.orientation(edg);
			assert(sign!=0);
			a_ij*=sign;
		}
		else if(j== he.b){
			a_ij = tuple3f::cotPoints(verts[fc.c],verts[fc.a],verts[fc.b]);
			sign = (i == he.c? -1: 1);
			sign *= fc.orientation((* mesh->getHalfedges())[he.b]);
			sign *= fc.orientation(edg);
			assert(sign!=0);
			a_ij*=sign;
		}
		else if(j==he.c){
			a_ij = tuple3f::cotPoints(verts[fc.a],verts[fc.b],verts[fc.c]);
			sign = (i == he.a? -1: 1);
			sign *= fc.orientation((* mesh->getHalfedges())[he.c]); //reason for this: formula for e_ab, e_bc, e_ca i.e. all edges should be 
			//positive oriented
			sign *= fc.orientation(edg); //same.
			assert(sign!=0);
			a_ij*=sign;
		}
		else{
			assert(false);
		}

		return a_ij*-1;
	}

	// row: is the edge number; target will be the two other edges in the triangle; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		tuple2i & edg = (* mesh->getHalfedges())[row];
		int nbr_fc1, nbr_fc2;
		meshOperation::getNbrFaces(edg,&nbr_fc1,&nbr_fc2, mesh->getBasicMesh().getNeighborFaces());

		//if is border edge (i.e. we want to calculate dual value for this edge!)
		if(nbr_fc2 < 0){
			tuple3i he = (* mesh->getFace2Halfedges())[nbr_fc1];
			target.push_back(he.a);
			target.push_back(he.b);
			target.push_back(he.c);


			sort(target.begin(),target.end());
		}
		
	}
};

//////////////////////////////////////////////////////////////////////////
// primal part of dual d1 with bordered mehs-
// d1_dual_primal times the dual values  on the primal edges
// gives the part of d1...
class d1dual_primal: public matrixCreator
{

	meshMetaInfo * mesh;

public:
	d1dual_primal(meshMetaInfo & aMesh){
		mesh = & aMesh;
	}
	
	float val(int i , int j){
	//j is an edge, i is a vertex.
		tuple2i & edge = (* mesh->getHalfedges())[j];
		if(edge.a != i && edge.b != i ){
			return 0;
		}

		int nbr_fc1, nbr_fc2;
		meshOperation::getNbrFaces(edge,&nbr_fc1,&nbr_fc2, mesh->getBasicMesh().getNeighborFaces());
		
		if(nbr_fc2 >=0){
			assert(nbr_fc1>=0);
			return 0;
		}
		//j is a border vertex adjascent to vertex i.
		//border vertices
		if(nbr_fc2<0){
			tuple3i fc = mesh->getBasicMesh().getFaces()[nbr_fc1];	
			assert(fc.contains(i));
			int or = fc.orientation(edge);
			assert(or!=0);
			// plus 0.5 in fc pos oriented edge value of incoming and of outgoing border edge.
			return 0.5 * or;
		}

		assert(false);
		return 0;
	}

	void indices(int row, std::vector<int> & target){
	//row is a vertex i.e dual face

		target.clear();
		if(meshOperation::isOnBorder(row,mesh->getBasicMesh())){
			meshOperation::getNeighborEdges(row,mesh->getBasicMesh().getNeighborFaces(),* mesh->getFace2Halfedges(),
			* mesh->getHalfedges(),target);
			sort(target.begin(),target.end());
		}
	}
};

class onesBorderEdgesCreator: public matrixCreator
{

	meshMetaInfo * mesh;

public:
	onesBorderEdgesCreator(meshMetaInfo & aMesh){
		mesh = & aMesh;
	}

	float val(int i , int j){
		if(i!= j){
			return 0;
		}

		tuple2i & edge = (* mesh->getHalfedges())[i];

		int nbr_fc1, nbr_fc2;
		meshOperation::getNbrFaces(edge,&nbr_fc1,&nbr_fc2, mesh->getBasicMesh().getNeighborFaces());

		if(nbr_fc2 >=0){
			return 0;
		}
		return 1;
	}

	// row: its the vertex number; 
	void indices(int row, std::vector<int> & target){
		target.clear();
		target.push_back(row);
	}
};*/

DDGMatrices::DDGMatrices(void)
{
}

DDGMatrices::~DDGMatrices(void)
{
}


cpuCSRMatrix DDGMatrices::border1( wingedMesh & aMesh )
{

	cpuCSRMatrix border1_;
	int nrVertices = aMesh.getVertices().size();
	border1_.initMatrix(border1_Creator(aMesh), nrVertices);

//	cpuCSRMatrix border_1 = cpuCSRMatrix::transpose(d0(aMesh));
	return border1_;
}

cpuCSRMatrix DDGMatrices::border1_halfedges( wingedMesh & aMesh )
{
	cpuCSRMatrix border1_;
	int nrVertices = aMesh.getVertices().size();
	border1_Creator c(aMesh);
	c.setHalfedgeMode(true);
	border1_.initMatrix(c, nrVertices);

	//	cpuCSRMatrix border_1 = cpuCSRMatrix::transpose(d0(aMesh));
	return border1_;
}

cpuCSRMatrix DDGMatrices::border2( wingedMesh & aMesh )
{
	cpuCSRMatrix border_2;
	border_2.initMatrix(border2_Creator(aMesh), aMesh.getEdges().size());
	//cpuCSRMatrix border_2= d1(aMesh);
	
	//return cpuCSRMatrix::transpose(border_2);
	return border_2;
}


cpuCSRMatrix DDGMatrices::d0( wingedMesh & aMesh )
{
	cpuCSRMatrix d_0;
	int nrEdges = aMesh.getEdges().size();
	d_0.initMatrix(d_0Creator(aMesh), nrEdges);

	return d_0;
}


cpuCSRMatrix DDGMatrices::d1(wingedMesh & aMesh )
{
	cpuCSRMatrix d_1;
	int nrFaces = aMesh.getFaces().size();
	d_1.initMatrix(d_1Creator(aMesh), nrFaces);
	return d_1;
}

// this is d1 transformed
cpuCSRMatrix DDGMatrices::dual_d0( wingedMesh & aMesh )
{
	//cpuCSRMatrix d1_=d1(aMesh);
	//return (cpuCSRMatrix::transpose(d1_));

	return border1(aMesh);
}

// this is minus d0 transformed
cpuCSRMatrix DDGMatrices::dual_d1( wingedMesh & aMesh )
{
	/*cpuCSRMatrix d_0= d0(aMesh);
	d_0*=(-1);//^1
	return (cpuCSRMatrix::transpose(d_0));*/
	cpuCSRMatrix border_1;
	border_1.initMatrix(border1_Creator(aMesh),aMesh.getVertices().size());
	border_1*=-1;
	return border_1;
}


/*cpuCSRMatrix DDGMatrices::dual_d1_borderdiff( meshMetaInfo & aMesh )
{
	cpuCSRMatrix dual1 = dualVals1(aMesh);
	cpuCSRMatrix d1diff;
	//dual edges to dual faces i.e. vertices
	d1diff.initMatrix(d1dual_primal(aMesh),aMesh.getBasicMesh().getVertices().size());
	d1diff.forceNrColumns(aMesh.getHalfedges()->size());

	cpuCSRMatrix star1inv = star1(aMesh);
	star1inv.elementWiseInv(0.000);
	return d1diff*dual1*star1inv;
}


cpuCSRMatrix DDGMatrices::dual_d1star1_borderdiff( meshMetaInfo & aMesh )
{
	cpuCSRMatrix dual1 = dualVals1(aMesh);
	cpuCSRMatrix d1diff;
	//dual edges to dual faces i.e. vertices
	d1diff.initMatrix(d1dual_primal(aMesh),aMesh.getBasicMesh().getVertices().size());
	d1diff.forceNrColumns(aMesh.getHalfedges()->size());
	return d1diff*dual1;
}
*/


cpuCSRMatrix DDGMatrices::star0( wingedMesh & aMesh )
{
	cpuCSRMatrix star0;
	int nrVerts = aMesh.getVertices().size();
	star0.initMatrix(star0Creator(aMesh), nrVerts);
	return star0;
}

cpuCSRMatrix DDGMatrices::star1( wingedMesh & aMesh )
{
	cpuCSRMatrix star1;
	int nrEdges = aMesh.getEdges().size();
	star1.initMatrix(star1Creator(aMesh), nrEdges);
	return star1;
}

cpuCSRMatrix DDGMatrices::star2( wingedMesh & aMesh )
{
	cpuCSRMatrix star2;
	int nrFaces = aMesh.getFaces().size();
	star2.initMatrix(star2Creator(aMesh), nrFaces);
	return star2;
}

cpuCSRMatrix DDGMatrices::star0_mixed( wingedMesh & aMesh, std::vector<float> & buffer, bool ignoreBorder )
{
	if(!ignoreBorder){
		meshMath::calcAllMixedAreas(aMesh, buffer);
	}
	else{
		meshMath::calcAllMixedAreas_ignoreBorder(aMesh, buffer);
	}
	cpuCSRMatrix id;
	id.initMatrix(diagCreator(buffer), aMesh.getVertices().size());
	return id;
}

cpuCSRMatrix DDGMatrices::star1_mixed( wingedMesh & aMesh, std::vector<float> & buffer)
{
	cpuCSRMatrix star1;
	int nrEdges = aMesh.getEdges().size();
	star1.initMatrix(star1mixedCreator(aMesh), nrEdges);
	return star1;
}

cpuCSRMatrix DDGMatrices::star1_mixed_halfedges( wingedMesh & aMesh)
{
	star1mixedCreator c(aMesh);
	c.set(meshMath::START_VERTEX_BORDER_COMPLETION);
	cpuCSRMatrix top, bot;
	top.initMatrix(c, aMesh.getEdges().size());
	c.set(meshMath::END_VERTEX_BORDER_COMPLETION);
	bot.initMatrix(c, aMesh.getEdges().size());
	top.append(bot);
	return top;
}

cpuCSRMatrix DDGMatrices::coderiv1_mixed( wingedMesh & aMesh, std::vector<float> & buffer )
{
	

	cpuCSRMatrix star_1= star1_mixed(aMesh, buffer);

	//meshMath::dualEdge_edge_ratios_mixed(aMesh, buffer);
	//cout << "calculating dual weights: " << GetTickCount() - tm <<"\n";
	//tm=GetTickCount();


	cpuCSRMatrix border1_ = border1(aMesh);

	cpuCSRMatrix star_0_inv = star0_mixed(aMesh, buffer);
	star_0_inv.elementWiseInv();

	return star_0_inv * border1_ * star_1;
}


cpuCSRMatrix DDGMatrices::coderiv1_mixed_ignoreBoundary( wingedMesh & aMesh )
{
	cpuCSRMatrix star1_he= star1_mixed_halfedges(aMesh);
	cpuCSRMatrix border1_he = border1_halfedges(aMesh);

	cpuCSRMatrix star_0_inv = star0_mixed(aMesh, std::vector<float>());
	star_0_inv.elementWiseInv();
	return star_0_inv * border1_he * star1_he;
}


cpuCSRMatrix DDGMatrices::coderiv1_mixed( wingedMesh & aMesh, std::vector<float> & buffer, cpuCSRMatrix & border1_ )
{

	DWORD tm = GetTickCount();

	cpuCSRMatrix star_1= star1_mixed(aMesh, buffer);
//	cout << "star_1: " << GetTickCount() - tm <<"\n";
//	tm=GetTickCount();

	//meshMath::dualEdge_edge_ratios_mixed(aMesh, buffer);
	//cout << "calculating dual weights: " << GetTickCount() - tm <<"\n";
	//tm=GetTickCount();

	cpuCSRMatrix star_0_inv = star0_mixed(aMesh, buffer);
	star_0_inv.elementWiseInv();

//	cout << "star0_inv: " << GetTickCount() - tm <<"\n";
//	tm=GetTickCount();

	star_1 =star_0_inv * border1_ * star_1;
//	cout << "star_0_inv * border1_ * star_1: " << GetTickCount() - tm <<"\n";
	cout << "*time in coderiv1mixed " << GetTickCount() - tm <<"\n";

	return star_1;
}

void DDGMatrices::coderiv1(
	cpuCSRMatrix & star_1, 
	cpuCSRMatrix & border_1,
	cpuCSRMatrix & star_0,
	cpuCSRMatrix & target)
{

	DWORD tm = GetTickCount();

	
	target = star_0;
	target.elementWiseInv();

	//	cout << "star0_inv: " << GetTickCount() - tm <<"\n";
	//	tm=GetTickCount();

	target = target * border_1;
	target = target * star_1;
	//	cout << "star_0_inv * border1_ * star_1: " << GetTickCount() - tm <<"\n";
	cout << "*time in coderiv1mixed " << GetTickCount() - tm <<"\n";

}




cpuCSRMatrix DDGMatrices::coderiv1( wingedMesh & aMesh )
{
	cpuCSRMatrix star_1= star1(aMesh);
	cpuCSRMatrix border1_ = border1(aMesh);
	cpuCSRMatrix star_0_inv = star0(aMesh);
	star_0_inv.elementWiseInv();

	return star_0_inv * border1_ * star_1;
	//return (star_0_inv % d_0)* star_1;


}

cpuCSRMatrix DDGMatrices::coderiv2( wingedMesh & aMesh )
{
	cpuCSRMatrix star_2= star2(aMesh);
	cpuCSRMatrix border2_ = border2(aMesh);//dual_d0(aMesh);

	cpuCSRMatrix star_1_inv = star1(aMesh);
	star_1_inv.elementWiseInv();

	return star_1_inv * border2_ * star_2;
	//return (star_1_inv % d_1)* star_2;
}


cpuCSRMatrix DDGMatrices::id0( wingedMesh & aMesh )
{
	cpuCSRMatrix id;
	id.initMatrix(idCreator(), aMesh.getVertices().size());
	return id;
}

cpuCSRMatrix DDGMatrices::id1( wingedMesh & aMesh )
{
	cpuCSRMatrix id;
	id.initMatrix(idCreator(), aMesh.getEdges().size());
	return id;
}

cpuCSRMatrix DDGMatrices::id2( wingedMesh & aMesh )
{
	cpuCSRMatrix id;
	id.initMatrix(idCreator(), aMesh.getFaces().size());
	return id;
}



/*double cotan_weights_divAmix( int i, int j, wingedMesh & m )
{
	

	if(i==j){
		return -1;
	}

	std::vector<int>::iterator idx;
	int prev, next;
	int edgeIndex =	m.edgeIndex(wingedEdge(i<j?i:j,i<j?j:i));
	if(edgeIndex <0){
		return 0;
	}

	//jth vertex is a neighbor of i
	std::vector<tuple3f> & verts =m.getVertices();
	float cot_alpha1, cot_alpha2;//, tempcot1, tempcot2;
	int nbr;
	wingedEdge *prevEdge, *nextEdge, *edge = & m.getEdges()[edgeIndex], *firstEdge;
	assert( *edge == wingedEdge(i<j?i:j,i<j?j:i));

	prevEdge = & edge->getPrev_bc(i);
	nextEdge = & edge->getNext_bc(i);

	prev =prevEdge->otherVertex(i);	
	next = nextEdge->otherVertex(i);

	cot_alpha1 = tuple3f::cotPoints(verts[j], verts[prev], verts[i]);
	cot_alpha2 = tuple3f::cotPoints(verts[i], verts[next], verts[j]);
	
	return (cot_alpha1 + cot_alpha2)/2 / meshMath::aVoronoi(i,m);//
	
}


cpuCSRMatrix DDGMatrices::laplaceOld( wingedMesh & m )
{
	cpuCSRMatrix mat;
	bool a_ii_added;
	int count = 0, nrVertices = m.getVertices().size(), offset;
	std::vector<int>::iterator j;

	float factor;
	wingedEdge *edge, *firstEdge;

	mat.iapush_back(0);

	std::vector<int> nbrs_i;
	for(int i = 0; i < nrVertices;i++){
		
		firstEdge = edge = & m.getAnEdge(i%nrVertices);
		nbrs_i.clear();
		do{
			nbrs_i.push_back(edge->otherVertex(i%nrVertices));
			edge = & edge->getNext_bc(i%nrVertices);
		}while(edge!=firstEdge);
		sort(nbrs_i.begin(), nbrs_i.end());

		offset = (i<nrVertices?0:nrVertices);

		a_ii_added = false;
		//calculate normation factor
		factor = 0;
		for(j = nbrs_i.begin(); j!=nbrs_i.end(); j++){
			factor += cotan_weights_divAmix(i%nrVertices,*j,m);

		}


		for(j = nbrs_i.begin(); j!=nbrs_i.end(); j++){
			if(i< *j + offset &&! a_ii_added){
				mat.japush_back(i);
				a_ii_added = true;
				mat.apush_back(cotan_weights_divAmix(i%nrVertices,i%nrVertices,m));
			}
			mat.japush_back((*j)+offset );
			//note the following makes sense because the sum of vals = 0 means there is only a diagonal element..
			mat.apush_back((factor <0.0001? 0: cotan_weights_divAmix(i%nrVertices,*j,m)/factor));

		}

		if(!a_ii_added){
			mat.japush_back(i);
			a_ii_added = true;
			mat.apush_back(cotan_weights_divAmix(i%nrVertices,i%nrVertices,m));
		}
		mat.iapush_back(mat.getja().size());
	}
	return mat;
}*/

cpuCSRMatrix DDGMatrices::laplaceIgnoreBoundary( wingedMesh & aMesh )
{
	return coderiv1_mixed_ignoreBoundary(aMesh) * d0(aMesh);
}

cpuCSRMatrix DDGMatrices::d1dual_star1_borderDiff( wingedMesh & aMEsh )
{
	cpuCSRMatrix mat_0p5sgn;
	mat_0p5sgn.initMatrix(Op5sgnij_plus_0p5sgnjk_creator(aMEsh), aMEsh.getVertices().size());
	mat_0p5sgn.forceNrColumns(aMEsh.getEdges().size());



	cpuCSRMatrix primal2dual_approx;
	primal2dual_approx.initMatrix(primal2dual_approx_creator(aMEsh), aMEsh.getEdges().size());
	primal2dual_approx.forceNrColumns(aMEsh.getEdges().size());


	return mat_0p5sgn*primal2dual_approx;
}


cpuCSRMatrix DDGMatrices::d1dual_star1_borderDiff_transp( wingedMesh & aMEsh )
{
	cpuCSRMatrix mat_0p5sgn = d1dual_star1_borderDiff(aMEsh);
	mat_0p5sgn = cpuCSRMatrix::transpose(mat_0p5sgn);
	return mat_0p5sgn;
}







/*
cpuCSRMatrix DDGMatrices::dualVals1( meshMetaInfo & aMesh )
{
	cpuCSRMatrix dual1;
	dual1.initMatrix(dualVal1Creator(aMesh),aMesh.getHalfedges()->size());
	dual1.forceNrColumns(dual1.getn());
	assert(dual1.getn() == aMesh.getHalfedges()->size() &&
		dual1.getm() == aMesh.getHalfedges()->size() );
	return dual1;
}

//ones on border for border Vertices!
cpuCSRMatrix DDGMatrices::onesBorder( std::vector<std::vector<int>> & border, int n, int m /*= n*/ /*)
{
	cpuCSRMatrix ones;
	ones.initMatrix(sparseDiagCreator(&border),n);
	ones.forceNrColumns(m);
	return ones;
}

//edge x edge matrix with ones exactly for border edges.
cpuCSRMatrix DDGMatrices::onesBorderEdges( meshMetaInfo & aMesh  )
{
	cpuCSRMatrix ones;
	ones.initMatrix(onesBorderEdgesCreator(aMesh),aMesh.getHalfedges()->size());
	ones.forceNrColumns(aMesh.getHalfedges()->size());
	return ones;
}

cpuCSRMatrix DDGMatrices::dual_d1_dualprimal( meshMetaInfo & aMesh )
{
	cpuCSRMatrix dualprimal;
	//dual edges to dual faces i.e. vertices
	dualprimal.initMatrix(d1dual_primal(aMesh),aMesh.getBasicMesh().getVertices().size());
	dualprimal.forceNrColumns(aMesh.getHalfedges()->size());
	return dualprimal;
}



*/
