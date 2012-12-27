#pragma once

#include "Observer.h"
#include "wfmesh.h"

class ATestClassForObserver:public Observer<meshMsg>
{
public:
	ATestClassForObserver(void);
	~ATestClassForObserver(void);

	void update(void * src, meshMsg msg);
};

