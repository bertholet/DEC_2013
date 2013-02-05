#include "StdAfx.h"
#include "floatVector.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>


floatVector::floatVector(void)
{
}

floatVector::floatVector( std::vector<tuple3f> & vals, int what )
{
	for(unsigned int i =0; i < vals.size(); i++){
		this->push_back(vals[i][what]);
	}
}

floatVector::floatVector( int sz ) :std::vector<float>(sz)
{

}


floatVector::~floatVector(void)
{
}

void floatVector::set( std::vector<tuple3f> & vals, int what )
{
	if(vals.size() != this->size()){
		this->resize(vals.size(), 0);
	}

	for(unsigned int i = 0; i < size(); i++){
		(*this)[i] = vals[i][what];
	}
}

void floatVector::saveVector(std::string  name, 
	std::string  file )
{
	std::ofstream myFile;
	myFile.open(file.c_str());
	myFile << name << " = [";

	for(unsigned int i = 0; i < size(); i++){
		myFile << (*this)[i];
		if(i != size() -1){
			myFile << ", ";
		}
	}

	myFile << "];";
	myFile.close();
}

void floatVector::saveTuple3iVectorMatlabStyle( std::vector<tuple3i> & vals,std::string name, std::string file )
{
	std::ofstream myFile;
	myFile.open(file.c_str());
	myFile << name << " = [";

	for(unsigned int i = 0; i <vals.size(); i++){
		myFile << vals[i][0] +1<<"," << vals[i][1] +1<<"," <<vals[i][2]+1;
		if(i != vals.size() -1){
			myFile << "; ";
		}
	}

	myFile << "];";
	myFile.close();
}

void floatVector::loadVector( std::string file )
{
	clear();
	std::ifstream myStream(file, std::ifstream::in);

	if(!myStream){
		printf("File not found!");
		throw std::exception("File was not found.");
	}

	//	typedef tokenizer<char_separator<char>> tokenizer_; 
	std::string buff;
	float val1;

	while(myStream.good()){
		getline(myStream, buff);
		sscanf_s(buff.c_str(), "%f", &val1);
		push_back(val1);
	}
}
