#pragma once
class Resetable{
public:
	virtual void reset() = 0;
};

class Resizable{
public:
	virtual void setLength(float what) = 0;
};

class Activable{
public:
	virtual void activateInput() = 0;
	virtual void desactivateInput() = 0;
};