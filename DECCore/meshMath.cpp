#include "StdAfx.h"
#include "meshMath.h"


meshMath::meshMath(void)
{
}


meshMath::~meshMath(void)
{
}

float meshMath::aVoronoi( int vertex, wingedMesh & mesh )
{
	float Avornoi = 0;
	//int prev, next, nbr;
	float tempcot1, tempcot2;
	//bool prevIsNeighbor,nextIsNeighbor;


	vector<tuple3f> & verts = mesh.getVertices();
	wingedEdge first, prev, actual, next;
	int actualVert, prevVert, nextVert;
	first = mesh.getAnEdge(vertex);

	actual = first;

	prev = actual.getPrev_bc(vertex);
	next = actual.getNext_bc(vertex);
	do 
	{
		prevVert = prev.otherVertex(vertex);
		actualVert = actual.otherVertex(vertex);
		nextVert = next.otherVertex(vertex);

		if(!actual.isFirstEdge(vertex)){
			tempcot1 = tuple3f::cotPoints(verts[actualVert], verts[prevVert], verts[vertex]);
			tempcot1 = (tempcot1 >0 ? tempcot1: -tempcot1);
		}else{
			tempcot1 = 0;
		}

		if(!actual.isLastEdge(vertex)){
			tempcot2 = tuple3f::cotPoints(verts[vertex], verts[nextVert], verts[actualVert]);
			tempcot2 = (tempcot2 >0 ? tempcot2: -tempcot2);
		}
		else{
			tempcot2 = 0;
		}

		Avornoi += (tempcot1 +
			tempcot2) *
			(verts[vertex]-verts[actualVert]).normSqr();


		prev = actual;
		actual = next;
		next = actual.getNext_bc(vertex);
	} while (first != actual);
	
	return Avornoi / 8;
}

float meshMath::dualEdge_edge_ratio( int edgeNr, wingedMesh & mesh )
{

	//bool prevIsNeighbor, nextIsNeighbor;
	wingedEdge & edge = mesh.getEdges()[edgeNr];
	int vertex1 = edge.start();
	int vertex2 = edge.end();
	int prev = edge.getPrev_bc(vertex1).otherVertex(vertex1);//meshOperation::getPrevious_bc(i, j, m, &prevIsNeighbor);	
	int next = edge.getNext_bc(vertex1).otherVertex(vertex1);

	vector<tuple3f> & verts = mesh.getVertices();

	float cot_alpha1 = (edge.isFirstEdge(vertex1) ? 
		0:
		tuple3f::cotPoints(verts[vertex2], verts[prev], verts[vertex1]));

	float cot_alpha2 = (edge.isLastEdge(vertex1)?
		0:
		tuple3f::cotPoints(verts[vertex1], verts[next], verts[vertex2]));
	

	/*float cot_alpha1 = (prevIsNeighbor? 
		tuple3f::cotPoints(verts[j], verts[prev], verts[i]):
	0);
	float cot_alpha2 = (nextIsNeighbor? 
		tuple3f::cotPoints(verts[i], verts[next], verts[j]):
	0);*/


//.... well.....
/*	cot_alpha1 = (cot_alpha1 >0? cot_alpha1: -cot_alpha1);
	cot_alpha2 = (cot_alpha2 >0? cot_alpha2: -cot_alpha2);*/

	return (cot_alpha1 + cot_alpha2)/2;
}


float meshMath::dualEdge_edge_ratio_mixed( int edgeNr, wingedMesh & mesh, dualEdgeType what )
{

	wingedEdge & edge = mesh.getEdges()[edgeNr];
	int vertex1 = edge.start();
	int vertex2 = edge.end();
	int prev;
	int next;
	switch(what){
	case END_VERTEX_BORDER_COMPLETION:
		prev= edge.getPrev_bc(vertex2).otherVertex(vertex2);
		next= edge.getNext_bc(vertex2).otherVertex(vertex2);
		break;
	default:
		prev= edge.getPrev_bc(vertex1).otherVertex(vertex1);
		next= edge.getNext_bc(vertex1).otherVertex(vertex1);
	}


	vector<tuple3f> & verts = mesh.getVertices();

	float cot_alpha1 = (edge.isFirstEdge(vertex1) && (what== NO_COMPLETION) ? //killed ! (what == NO_COMPLETION) 
				0:
				tuple3f::cotPoints(verts[vertex2], verts[prev], verts[vertex1]));

	float cot_alpha2 = (edge.isLastEdge(vertex1) && (what== NO_COMPLETION)?
				0:
				tuple3f::cotPoints(verts[vertex1], verts[next], verts[vertex2]));


	
/*	float	cot_alpha1 = tuple3f::cotPoints(verts[vertex2], verts[prev], verts[vertex1]);
	float	cot_alpha2 = tuple3f::cotPoints(verts[vertex1], verts[next], verts[vertex2]);*/
				
	cot_alpha1 = (cot_alpha1 >0? cot_alpha1: 0);
	cot_alpha2 = (cot_alpha2 >0? cot_alpha2: 0);/**/

	return (cot_alpha1 + cot_alpha2)/2;
}


void meshMath::dualEdge_edge_ratios_mixed( wingedMesh & mesh, std::vector<float> & target )
{
	std::vector<wingedEdge> & edges = mesh.getEdges();
	std::vector<tuple3f> & verts = mesh.getVertices();
	float cot_alpha1, cot_alpha2;
	int prev, next, vertex1, vertex2;

	if(target.size() != edges.size()){
		target.resize(edges.size(), 0);
	}

	for(unsigned int i = 0; i < edges.size(); i++){
		vertex1 = edges[i].start();
		vertex2 = edges[i].end();
		prev = edges[i].getPrev_bc(vertex1).otherVertex(vertex1);//meshOperation::getPrevious_bc(i, j, m, &prevIsNeighbor);	
		next = edges[i].getNext_bc(vertex1).otherVertex(vertex1);

		cot_alpha1 = (edges[i].isFirstEdge(vertex1) ? 
				0:
				tuple3f::cotPoints(verts[vertex2], verts[prev], verts[vertex1]));

		cot_alpha2 = (edges[i].isLastEdge(vertex1)?
				0:
				tuple3f::cotPoints(verts[vertex1], verts[next], verts[vertex2]));

		cot_alpha1 = (cot_alpha1 >0? cot_alpha1: 0);
		cot_alpha2 = (cot_alpha2 >0? cot_alpha2: 0);

		target[i]= (cot_alpha1 + cot_alpha2)/2;
	}


}



float meshMath::area( int faceNr, wingedMesh & mesh )
{
	std::vector<tuple3i> & fcs = mesh.getFaces();
	std::vector<tuple3f> & verts = mesh.getVertices();
	float res = tuple3f::crossNorm(
		verts[fcs[faceNr].b] - 
		verts[fcs[faceNr].a],
		verts[fcs[faceNr].c] - 
		verts[fcs[faceNr].a]) / 2.f;

	if(res > 4){
		//hmm....
		res = res;
	}
	return res;
}

void meshMath::calcAllMixedAreas_ignoreBorder( wingedMesh &m, std::vector<float> &AMixed )
{
	vector<tuple3i>& faces = m.getFaces();
	vector<tuple3f>& verts = m.getVertices();
	float areaT, aMix, cot1, cot2;
	//	AMixed.reserve(m.getVertices().size());
	AMixed.resize(m.getVertices().size(), 0);
	int nrVertices = m.getVertices().size(), vertex_nbr1, vertex_nbr2;

	wingedEdge * edge, *first, *next;

	for(int vert = 0; vert < nrVertices; vert++){
		aMix = 0;

		//iterate over the neightborhood...
		edge = first = & m.getAnEdge(vert);
		next= & edge->getNext_bc(vert);
		do{
			//the vertices to be considered.
			vertex_nbr1 = edge->otherVertex(vert);
			vertex_nbr2 = next->otherVertex(vert);
			//triangleArea
			areaT = tuple3f::cross((verts[vertex_nbr1] - verts[vert]),(verts[vertex_nbr2] - verts[vert])).norm()/2;


			if(tuple3f::a_bDotc_b(verts[vertex_nbr1], verts[vert], verts[vertex_nbr2]) <0 )
			{
				aMix += areaT/2;
			}
			else if(tuple3f::a_bDotc_b(verts[vertex_nbr1], verts[vertex_nbr2], verts[vert]) <0 ||
				tuple3f::a_bDotc_b(verts[vertex_nbr2], verts[vertex_nbr1], verts[vert])<0)
			{
				aMix += areaT/4;
			}
			else{
				cot1 = tuple3f::cotPoints(verts[vertex_nbr1], verts[vertex_nbr2], verts[vert]);
				cot1 = (cot1 >0 ? cot1: -cot1);
				cot2 = tuple3f::cotPoints(verts[vert], verts[vertex_nbr1], verts[vertex_nbr2]);
				cot2 = (cot2 >0 ? cot2: -cot2);
				aMix += cot1 * (verts[vert]-verts[vertex_nbr1]).normSqr() +
					cot2 * (verts[vert]-verts[vertex_nbr2]).normSqr();
			}

			edge= next;
			next= & next->getNext_bc(vert);
		}while(edge!= first);

		AMixed[vert] = aMix/8;
	}

}

void meshMath::calcAllMixedAreas( wingedMesh &m, std::vector<float> &AMixed )
{
	vector<tuple3i>& faces = m.getFaces();
	vector<tuple3f>& vertices = m.getVertices();
	float areaT,cot1, cot2, cot3, norm1sqr, norm2sqr, norm3sqr;
//	AMixed.reserve(m.getVertices().size());
	AMixed.resize(m.getVertices().size(), 0);
	int nrFaces = m.getFaces().size();

	for(int i = 0; i < nrFaces; i++){
		cot1 = tuple3f::cotPoints(vertices[faces[i].c],vertices[faces[i].a],vertices[faces[i].b]);//cot(c,a,b)
		cot2 = tuple3f::cotPoints(vertices[faces[i].a],vertices[faces[i].b],vertices[faces[i].c]);//cot(a,b,c)
		cot3 = tuple3f::cotPoints(vertices[faces[i].b],vertices[faces[i].c],vertices[faces[i].a]);//cot(b,c,a)
		areaT = tuple3f::cross((vertices[faces[i].a] - vertices[faces[i].b]),(vertices[faces[i].c] - vertices[faces[i].b])).norm()/2;
		if(tuple3f::a_bDotc_b(vertices[faces[i].c],vertices[faces[i].a],vertices[faces[i].b])< 0) //triangle obtuse at a
		{
			AMixed[faces[i].a]+=areaT/2;  
			AMixed[faces[i].b]+=areaT/4; 
			AMixed[faces[i].c] +=areaT/4;
		}
		else if(tuple3f::a_bDotc_b(vertices[faces[i].a],vertices[faces[i].b],vertices[faces[i].c])< 0)
		{
			AMixed[faces[i].a]+=areaT/4;  
			AMixed[faces[i].b]+=areaT/2; 
			AMixed[faces[i].c] +=areaT/4;
		}
		else if(tuple3f::a_bDotc_b(vertices[faces[i].b],vertices[faces[i].c],vertices[faces[i].a]) < 0)
		{
			AMixed[faces[i].a]+=areaT/4;  
			AMixed[faces[i].b]+=areaT/4; 
			AMixed[faces[i].c] +=areaT/2;
		}
		else //triangle is non obtuse: vornoi area*/
		{
			norm1sqr = (vertices[faces[i].c] - vertices[faces[i].b]).normSqr();
			norm2sqr = (vertices[faces[i].c] - vertices[faces[i].a]).normSqr();
			norm3sqr = (vertices[faces[i].a] - vertices[faces[i].b]).normSqr();
			AMixed[faces[i].c] += 0.125f * cot1 *norm1sqr;
			AMixed[faces[i].c] += 0.125f *cot2 * norm2sqr;
			AMixed[faces[i].a] += 0.125f *cot2 * norm2sqr;
			AMixed[faces[i].a] += 0.125f *cot3 * norm3sqr;
			AMixed[faces[i].b] += 0.125f *cot3 * norm3sqr;
			AMixed[faces[i].b] += 0.125f *cot1 * norm1sqr;
		}
	}
}

void meshMath::curvatureNormals( wingedMesh & m, std::vector<tuple3f> & K, 
	cpuCSRMatrix & d0_coderiv1mixed, std::vector<float> & buffer )
{
	d0_coderiv1mixed.mult(m.getVertices(), K);
}

float meshMath::volume( wingedMesh &m )
{
	int sz = m.getFaces().size();
	std::vector<tuple3f> & vertices = m.getVertices();
	std::vector<tuple3i> & faces = m.getFaces();
	float volume=0;
	float temp;
	for(int i = 0; i < sz; i++){
		temp= ((vertices[faces[i].a] + vertices[faces[i].b] + vertices[faces[i].c]) * (1.f/3)).dot(
			tuple3f::cross((vertices[faces[i].b] - vertices[faces[i].a]),vertices[faces[i].c] - vertices[faces[i].a]));

		//temp = (temp>0?temp:-temp);
		volume+=temp;
	}
	volume = (volume < 0? -volume:volume);
	return volume / 6;
}

void meshMath::circumcenters(wingedMesh & mesh, std::vector<tuple3f> & target )
{
	target.clear();
	std::vector<tuple3i> & fcs =mesh.getFaces();
	std::vector<tuple3f> & verts =mesh.getVertices();
	target.reserve(fcs.size());
	
	std::vector<tuple3i>::iterator fc;
	/*tuple3f normal_ac;
	tuple3f normal_ab;
	tuple3f normal;*/
	tuple3f circumcenter;
	float l_ab_sqr, l_bc_sqr, l_ca_sqr, bari_a,bari_b, bari_c, tot;
	for(fc = fcs.begin(); fc != fcs.end(); fc++){
		/*normal = (verts[fc->c] -verts[fc->a]).cross(verts[fc->b]-verts[fc->a]);
		normal_ac = (verts[fc->c] -verts[fc->a]).cross(normal);
		normal_ac.normalize();*/
		l_ab_sqr = (verts[fc->b] -verts[fc->a]).normSqr();
		l_bc_sqr = (verts[fc->c] -verts[fc->b]).normSqr();
		l_ca_sqr = (verts[fc->a] -verts[fc->c]).normSqr();
		bari_a = l_bc_sqr*(-l_bc_sqr + l_ca_sqr + l_ab_sqr);
		bari_b = l_ca_sqr*(l_bc_sqr - l_ca_sqr + l_ab_sqr);
		bari_c = l_ab_sqr*(l_bc_sqr + l_ca_sqr - l_ab_sqr);

		tot = bari_a + bari_b + bari_c;
		circumcenter = verts[fc->a] * (bari_a /tot) + verts[fc->b] * (bari_b /tot)
			+ verts[fc->c] * (bari_c /tot);
		target.push_back(circumcenter);
	}
}

void meshMath::centroids( wingedMesh & mesh, std::vector<tuple3f> & target )
{
	target.clear();
	std::vector<tuple3i> & fcs =mesh.getFaces();
	std::vector<tuple3f> & verts =mesh.getVertices();
	target.reserve(fcs.size());
	
	std::vector<tuple3i>::iterator fc;
	for(fc = fcs.begin(); fc != fcs.end(); fc++){
		target.push_back(verts[fc->a]*0.333333333f
			+verts[fc->b]*0.333333333f
			+verts[fc->c]*0.333333333f);
	}
}

void meshMath::centroid( wingedMesh & mesh, int faceNr, tuple3f & target )
{
	std::vector<tuple3f> & verts =mesh.getVertices();
	tuple3i * fc = & mesh.getFaces()[faceNr];
	target.set(verts[fc->a]*0.333333333f 
		+verts[fc->b]*0.333333333f
		+verts[fc->c]*0.333333333f);
}

void meshMath::bariCoords( tuple3f & pos, 
	int dualFace, 
	std::vector<tuple3f> & dualVertices, 
	std::vector<float> & target, 
	std::vector<int> & target_contributers,
	wingedMesh & myMesh )
{
	std::vector<tuple3i> & faces = myMesh.getFaces();
	std::vector<tuple3f> & verts = myMesh.getVertices();
	tuple3f n1, n2, vert;
	wingedEdge * edge, *first, *next;
	int fc; 
	float sum=0;


	target.clear();
	target_contributers.clear();

	//iterate over the faces adjacent to the vertex 'dualFace'
	first = edge = & myMesh.getAnEdge(dualFace);
	next = & edge->getNext_bc(dualFace);
	do 
	{
		fc = edge->commonFace(*next);//edge->getLeftFace(dualFace);
		if(fc >=0){
			target_contributers.push_back(fc);
			
			//the vectors normal to the dual edges are
			vert = verts[dualFace];
			n1 = verts[edge->otherVertex(dualFace)] - vert;
			n1.normalize();
			n2 = verts[next->otherVertex(dualFace)] - vert;
			n2.normalize();

			target.push_back(abs(
				tuple3f::crossNorm(n1, n2) / 
					(nonzeroDot(n1, pos - dualVertices[fc])* 
					nonzeroDot(n2, pos -dualVertices[fc]))
			));
			sum += target.back();
			
			//target.push_back(abs(bariWeight(pos, i,dualFace_id,v2f,dualVert_pos,mesh)));
		}
		edge = next;
		next = & next->getNext_bc(dualFace);
	} while (first!= edge);

	for(unsigned int i = 0; i < target.size(); i++){
		target[i]/=sum;
	}

}

float meshMath::nonzeroDot( tuple3f & a, tuple3f & b )
{
	float dot = a.dot(b);
	if(dot>=0 ){
		dot+= 10E-10f;
	}
	else{
		dot-= 10E-10f;
	}
	return dot;
}
