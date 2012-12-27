// ConsoleApplication1.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <iostream>
#include "tuple3.h"
#include "wfmesh.h"
#include "ATestClassForObserver.h"
#include "OBIFileReader.h"

int _tmain(int argc, _TCHAR* argv[])
{


	tuple3f myTuple(1.53245f,2.0f,3.0f);
	//tuple3i myTuple= tuple3i(1,2,3);
	std::cout<<myTuple.x<<'\n';
	wfMesh myMesh = wfMesh();
	ATestClassForObserver obs;
	myMesh.attach(&obs);
	
	OBIFileReader myreader;
	myreader.parse("../objfiles/teapotTex.obj");
	myreader.initializeMesh(myMesh);
	myMesh.updateObserver(CONNECTIVITY_CHANGED);

	std::cout << "Hello, my Mesh has " << myMesh.getVertices().size() << " vertices\n";
	std::cout << "Now, enter a number and press Enter\n";
	int a;
	std::cin >> a;
	return 0;
}

