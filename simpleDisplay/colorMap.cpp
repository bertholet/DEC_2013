#include "colorMap.h"
#include "application_meshParam.h"

colorMap::colorMap(void)
{
}


colorMap::~colorMap(void)
{
}


//////////////////////////////////////////////////////////////////////////
// Border Map: highlight borders
//////////////////////////////////////////////////////////////////////////
borderMap::borderMap( wingedMesh & m )
{
	std::vector<wingedEdge*>::iterator  it;
	tuple3f colors[] = {tuple3f(0.9,0.1,0.1),tuple3f(0.1,0.9,0.1),tuple3f(0.9,0.9,0.1),tuple3f(0.1,0.9,0.9),tuple3f(0.9,0.1,0.9),
	tuple3f(0.9,0.9,0.9),tuple3f(0.1,0.1,0.1)};

	cols.resize(m.getVertices().size(),tuple3f(0.1,0.1,0.9));
	wingedEdge * temp;
	int i = 0;
	for(it = m.getBoundaryEdges().begin(); it != m.getBoundaryEdges().end(); it++){
		cols[(*it)->getFirstBoundaryVertex()] = colors[i % 7];
		temp = *it;
		do{
			temp= temp->nextBorderEdge();
			cols[temp->getFirstBoundaryVertex()]=colors[i % 7];
		}while(temp!= *it);

		i++;
	}
}

borderMap::~borderMap()
{

}

std::vector<tuple3f> & borderMap::getColors()
{
	return cols;
}


//////////////////////////////////////////////////////////////////////////
//constColor
//////////////////////////////////////////////////////////////////////////
std::vector<tuple3f> & constColor::getColors()
{
	return cols;
}

constColor::constColor( wfMesh & m, tuple3f color )
{
	cols.resize(m.getVertices().size(), color);
}


//////////////////////////////////////////////////////////////////////////
//Anglemap
//////////////////////////////////////////////////////////////////////////
angleMap::angleMap( wingedMesh & m )
{
	cols.resize(m.getVertices().size(), tuple3f(1,1,1));
	std::vector<float> angles, lambdas;
	wingedEdge *edge, *first;
	int index;
	for(int i = 0; i < m.getBoundaryEdges().size(); i++){

		application_meshParam::computeAnglesAndLambdas(m,
			angles, lambdas, i, i==0);

		index=0;
		edge= m.getBoundaryEdges()[i];
		first= edge;
		do{
			cols[edge->getFirstBoundaryVertex()] = (angles[index] < 3.1415? tuple3f(0,0,1):tuple3f(1,0,0));
			edge= edge->nextBorderEdge();
			index++;
		}while(edge!=first);
	}
	
}

angleMap::~angleMap()
{

}

std::vector<tuple3f> & angleMap::getColors()
{
	return cols;
}

//////////////////////////////////////////////////////////////////////////
// Stroke Map
//////////////////////////////////////////////////////////////////////////
markupMap::markupMap():basecolor(0.8,0.8,0.8)
{
	cols.push_back(basecolor);
}

markupMap::~markupMap()
{

}

std::vector<tuple3f> & markupMap::getColors()
{
	return cols;
}

/*void markupMap::process( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	cols[intersec_vertex % cols.size()].set(0,1,0);
}
void markupMap::processOnPress( int intersec_vertex, int intersec_face, tuple3f & intersec_pos )
{
	process(intersec_vertex,intersec_face,intersec_pos);
}*/


void markupMap::reset()
{
	cols.assign(cols.size(), basecolor);
}

void markupMap::associateTo( wfMesh & m )
{
	if(cols.size()!= m.getVertices().size()){
		cols.assign(m.getVertices().size(),basecolor);
	}
}


void markupMap::mark( int vertex, tuple3f &col )
{
	if(cols.size()> vertex){
		cols[vertex] = col;
	}
}

//////////////////////////////////////////////////////////////////////////

void vorticityMap::update( std::vector<float> & values, MODEL & model )
{
	cols.resize(values.size());
	float sth = (values[0]), sth2, sth3;
	cpuCSRMatrix & st0 = model.getStar0_mixed();
	float scl = scale * st0.geta()[0];

	for(int i = 0; i < cols.size(); i++){
		sth = abs(values[i]*scale/st0.geta()[i]);
		sth = log(100*sth + 1) /(0.7*log(11.0)); // sth = 0 for sth = 0 else > 0, >0.7 for sth > 0.1
		sth2 = (sth > 0.7? sth-0.7:0);
		sth2 = log(10*sth2 + 1)/2; //>1 if sth2 >=1 i.e. sth>1.7 i.e sth > 1
		sth3 = (sth2>0.7?sth2-0.7:0);
		sth3 = log(sth3+1)/3;
		if(values[i] < 0){
			sth = 0.3 + sth;
			cols[i].set(0.3,0.3 + sth2,sth-sth3);
		}
		else{
			sth = 0.3+ sth;
			cols[i].set(sth-sth3,0.3+sth2,0.3);
		}
	}
}

std::vector<tuple3f> & vorticityMap::getColors()
{
	return cols;
}

void vorticityMap::set( float scl )
{
	scale = scl;
}

vorticityMap::vorticityMap()
{
	scale = 1;	
}

vorticityMap::~vorticityMap()
{

}
