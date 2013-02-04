#pragma once
class Resetable{
public:
	virtual void reset() = 0;
};

class Resizable{
public:
	virtual void setLength(float what) = 0;
};