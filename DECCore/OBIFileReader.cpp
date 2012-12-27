#include "stdafx.h"
#include "OBIFileReader.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <assert.h>

#define VERTEX "v"
#define COMMENT "#"
#define VNORMAL "vn"
#define VTEX "vt"
#define FACE "f"

OBIFileReader::OBIFileReader()
{

}

OBIFileReader::~OBIFileReader(void)
{
}


void OBIFileReader::parse(const char * file){
	using namespace boost;
	ifstream myStream(file, ifstream::in);
	char_separator<char> sep(" /");
	
	if(!myStream){
		printf("File not found!");
		throw std::exception("File was not found.");
	}
	

	typedef tokenizer<char_separator<char>> tokenizer_; 
	std::string buff;
	tokenizer_ t(buff, sep);

	vector<std::string> tokens;

	float val1, val2, val3;
	//int vl1,vl2,vl3, vl4, vl5, vl6;
	tuple3f tuple;
	tuple3i tuplei;
	char what1, what2;
	int matches;


	while(myStream.good()){
		getline(myStream, buff);
		matches = sscanf_s(buff.c_str(), "%1c%1c", &what1, 1, &what2,1); 
		
		if(matches != 2){
			//skip everything
			cout << ">> Could not parse line: "<<buff <<std::endl;
		}
		else if(what1 == 'v' && what2 == ' '){
			sscanf_s(buff.c_str(), "v %f %f %f", &val1, &val2, &val3);
			tuple.set(val1, val2, val3);
			vertices.push_back(tuple);
		}
		else if(what1 == 'v' && what2 == 'n'){
			sscanf_s(buff.c_str(), "vn %f %f %f", &val1, &val2, &val3);
			tuple.set(val1, val2, val3);
			tuple.normalize();
			normals.push_back(tuple);
		}
		else if(what1 == 'v' && what2 == 't'){
			sscanf_s(buff.c_str(), "vt %f %f", &val1, &val2);
			tuple.set(val1, val2, 0.f);
			tex.push_back(tuple);
		}
		else if(what1 == 'f' && what2 == ' '){


			tokens.clear();
			t= tokenizer_(buff, sep);
			for(tokenizer_::iterator beg = t.begin(); beg!=t.end(); ++beg){
				tokens.push_back(beg.current_token());
			}
			

			if(tokens.size() == 4){ //only face coordinates
				tuplei.set(lexical_cast<int>(tokens[1]) - 1,
					lexical_cast<int>(tokens[2])-1,
					lexical_cast<int>(tokens[3])-1);
				faces.push_back(tuplei);
			}
			else if(tokens.size() == 7){//assume there are onlyvertices and normals
				tuplei.set(lexical_cast<int>(tokens[1])-1,
					lexical_cast<int>(tokens[3]) -1,
					lexical_cast<int>(tokens[5]) -1);
				faces.push_back(tuplei);

				tuplei.set(lexical_cast<int>(tokens[2])-1,
					lexical_cast<int>(tokens[4]) -1,
					lexical_cast<int>(tokens[6]) -1);
				faces_normals.push_back(tuplei);
			}
			else if(tokens.size() == 10){//assume there are onlyvertices and normals
				tuplei.set(lexical_cast<int>(tokens[1])-1,
					lexical_cast<int>(tokens[4]) -1,
					lexical_cast<int>(tokens[7]) -1);
				faces.push_back(tuplei);

				tuplei.set(lexical_cast<int>(tokens[2])-1,
					lexical_cast<int>(tokens[5]) -1,
					lexical_cast<int>(tokens[8]) -1);
				faces_textures.push_back(tuplei);

				tuplei.set(lexical_cast<int>(tokens[3])-1,
					lexical_cast<int>(tokens[6]) -1,
					lexical_cast<int>(tokens[9]) -1);
				faces_normals.push_back(tuplei);
			}
			else{
				cout <<"Undefined face specification!";
				throw std::runtime_error("Unimplemented face specification found while reading obj file");
			}

		}
		else{
			cout << ">> Didnt read line: "<<buff <<std::endl;
		}

				

	}
	myStream.close();
	cout << "read : \n" << vertices.size() << " vertices \n"
		<< normals.size() << " normals \n"
		<< tex.size() << " textures \n"
		<< faces.size() << " faces. :-*\n";

	checkAllVerticesUsed();


}

void OBIFileReader::checkAllVerticesUsed( void )
{
	bool * arr = new bool[vertices.size()];
	vector<int>  unusedVertices;

	for(unsigned int i = 0; i < vertices.size(); i++){
		arr[i] = false;
	}


	for(unsigned int i = 0; i < faces.size(); i++){
		assert(faces[i].a < (int) vertices.size() && faces[i].b< (int) vertices.size() && faces[i].c < (int) vertices.size());
		arr[faces[i].a] = true;
		arr[faces[i].b] = true;
		arr[faces[i].c] = true;
	}

	for(unsigned int i = 0; i < vertices.size(); i++){
		if(!arr[i]){
			unusedVertices.push_back(i);
		}
	}
	
	cout << "Analysed parsed Mesh... " << unusedVertices.size() << " Vertices unused\n";


	delete[] arr;
}

void OBIFileReader::initializeMesh( wfMesh & mesh )
{
	mesh.setVertices(vertices);
	mesh.setFaces(faces);
	

	if(normals.size() != 0){
		mesh.setNormals(faces_normals, normals);
	}
	else{
		mesh.initNormalsFromVertices();
	}

	mesh.setTexture(faces_textures, tex);
}



