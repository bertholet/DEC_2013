#include "stdafx.h"
#include "ATestClassForObserver.h"
#include "wfmesh.h"

#include <iostream>


ATestClassForObserver::ATestClassForObserver(void)
{
}


ATestClassForObserver::~ATestClassForObserver(void)
{
}

void ATestClassForObserver::update( void * src, meshMsg msg )
{
	cout << "got a message from mesh: the mesh has ";
	cout << ((wfMesh *) src)->getVertices().size() << "vertices, friend \n" ;

}
