#include "colorMap.h"


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
