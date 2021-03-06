#include "StdAfx.h"
#include "cpuCSRMatrix.h"
#include <assert.h>
#include "idCreator.h"
#include <boost/lexical_cast.hpp>

cpuCSRMatrix::cpuCSRMatrix(void)
{
	n= 0;
	m= 0;
}

cpuCSRMatrix::cpuCSRMatrix( int * ia_, int *ja_,
							 float * a_, int sz_ia, int sz_ja)
{
	n= 0;
	m= 0;
	for(int i = 0; i < sz_ia; i++){
		this->iapush_back(ia_[i]);
	}
	for(int j = 0; j< sz_ja; j++){
		this->japush_back(ja_[j]);
		this->apush_back(a_[j]);
	}
}

cpuCSRMatrix::~cpuCSRMatrix(void)
{
}

void cpuCSRMatrix::initMatrix( matrixCreator & creator, int n )
{
	std::vector<int> js;
	int j;
/*	int stp = n/100 +1;
	int max = (n - n%stp)/stp;
	int nrStp = 0;
	//statusbar
	if(bar != NULL){
		bar->setBar(0,max);
	}*/

	this->clear();

	for(int i = 0; i < n ; i++){
		creator.indices(i,js);
		iapush_back(a.size());

		for(unsigned int k = 0; k < js.size(); k++){
			j = js[k];
			apush_back(creator.val(i,j));
			japush_back(j);
		}

		/*if(bar != NULL && i % stp == 0){
			bar->updateBar(nrStp++);
		}*/
	}
	iapush_back(a.size());
}

void cpuCSRMatrix::saveMatrix( std::string file )
{
	std::ofstream myFile;
	int internI;
	bool element_n_m_missing = (ia[ia.size()-1] - ia[ia.size()-2] == 0 ||
		 ja[ja.size()-1]!=m);
	//bool lastRowDegenerated = (ia[ia.size()-1] - ia[ia.size()-2] == 0);

	myFile.open(file.c_str());

	myFile << "i= [";
	//internI is the row number
	internI = 0;
	for(int i = 0; i < (int) a.size(); i++){
		//i goes over all indices. i.e determine for each a[i] its "i" matrix
		//coordinate. ia[internI+1] is the start index of 0 based row internI
		while(i >= ia[internI+1]){
			internI++;
			myFile << "...\n";
		}
		myFile << boost::lexical_cast<std::string>(internI+1); //store 1-based indices for matlab
		if(i!= a.size() -1){
			myFile <<", ";
		}
	}
	if(element_n_m_missing){
		myFile << ",...\n " << boost::lexical_cast<std::string>(n);
	}

	myFile << "];\n\n j=[";

	internI = 0;
	for(int i = 0; i < (int) a.size(); i++){
		while(i >= ia[internI+1]){
			internI++;
			myFile << "...\n";
		}
		myFile << boost::lexical_cast<std::string>(ja[i]+1);
		if(i!= a.size() -1){
			myFile <<", ";
		}
	}
	if(element_n_m_missing){
		myFile << ",...\n " << boost::lexical_cast<std::string>(m);
	}

	myFile << "];\n\n a=[";
	internI = 0;
	for(int i = 0; i < (int) a.size(); i++){
		while(i >= ia[internI+1]){
			internI++;
			myFile << "...\n";
		}
		myFile << boost::lexical_cast<std::string>(a[i]);
		if(i!= a.size() -1){
			myFile <<", ";
		}
	}
	if(element_n_m_missing){
		myFile << ",...\n " << boost::lexical_cast<std::string>(0);
	}
	myFile << "];";

	myFile.close();
}

/*void pardisoMatrix::saveVector(std::vector<double> & vctor, std::string  name, 
							   std::string  file )
{
	std::ofstream myFile;
	int internI;
	myFile.open(file.c_str());
	myFile << name << " = [";

	for(int i = 0; i < vctor.size(); i++){
		myFile << vctor[i];
		if(i != vctor.size() -1){
			myFile << ", ";
		}
	}

	myFile << "];";
	myFile.close();
}


void pardisoMatrix::saveVector(std::vector<int> & vctor, std::string  name, 
							   std::string  file )
{
	std::ofstream myFile;
	int internI;
	myFile.open(file.c_str());
	myFile << name << " = [";

	for(int i = 0; i < vctor.size(); i++){
		myFile << vctor[i];
		if(i != vctor.size() -1){
			myFile << ", ";
		}
	}

	myFile << "];";
	myFile.close();
}*/

int cpuCSRMatrix::dim()
{
	return ia.size()-1;
}



void cpuCSRMatrix::getDiagonalIndices( std::vector<int> & target_ind )
{
	target_ind.clear();
	target_ind.reserve(ia.size()-1);
	bool hasDiagEl;

	for(int i = 0; i < (int) ia.size()-1; i++){
		hasDiagEl = false;
		for(int j = 0; j < ia[i+1]-ia[i]; j++){
			if(ja[ia[i] + j] == i) { 
				hasDiagEl = true;
				target_ind.push_back(ia[i] + j);
			}
		}
		assert(hasDiagEl);
		assert(ja[target_ind.back()] == i); 
	}
}

void cpuCSRMatrix::addOnDiagonal( float val )
{
	bool hasDiagEl;

	for(int i = 0; i < (int) ia.size()-1; i++){
		hasDiagEl = false;
		for(int j = 0; j < ia[i+1]-ia[i]; j++){
			if(ja[ia[i] + j] == i) { 
				hasDiagEl = true;
				a[ia[i] + j] += val;
			}
		}
		assert(hasDiagEl);
	}
}

void cpuCSRMatrix::add( int i, int j, float val )
{
	int bs = ia[i];
	bool added = false;
	for(int k = bs; k < ia[i+1]; k++){
		if(ja[k] == j){
			a[k]+= val;
			added = true;
			break;
		}
	}

	if(!added){
		assert(false);
		throw std::runtime_error("Error in pardisoMatrix::add : (i,j) not a Matrix Entry");
	}
}


void cpuCSRMatrix::set( int i , int j, float val )
{
	int bs = ia[i];
	bool added = false;
	for(int k = bs; k < ia[i+1]; k++){
		if(ja[k] == j){
			a[k]= val;
			added = true;
			break;
		}
	}

	if(!added){
		assert(false);
		throw std::runtime_error("Error in pardisoMatrix::add : (i,j) not a Matrix Entry");
	}
}

float cpuCSRMatrix::get( int i , int j )
{
	int bs = ia[i];
	for(int k = bs; k < ia[i+1]; k++){
		if(ja[k] == j){
			return a[k];
		}
	}
	return 0;
}





void cpuCSRMatrix::addLine(std::vector<int> & js, std::vector<float> & vals){
	assert(js.size()  == vals.size());
	if(ia.size() == 0){
		assert(a.size() == 0 && ja.size() == 0);
		iapush_back(0);
	}

	for(unsigned int i = 0; i < js.size();i++){
		a.push_back(vals[i]);
		japush_back(js[i]);
	}
	iapush_back(a.size());

}

cpuCSRMatrix cpuCSRMatrix::operator*( cpuCSRMatrix & B )
{
	assert(B.getn() == this->getm());
	cpuCSRMatrix AB;
	AB.ia.reserve(this->dim());
	AB.ja.reserve(3*this->dim());
	AB.a.reserve(3*this->dim());
	AB.iapush_back(0);

	int Aia_start, Aia_stop, next_j, k;
	float val;
	std::vector<int> b_idx, b_stop;

	for(int i = 0; i < this->n; i++){
		//new this.ia[i]
		Aia_start = this->ia[i];
		Aia_stop = this->ia[i+1];

		//reset b_idx and b_stop
		b_idx.clear();
		b_stop.clear();
		for(int l = Aia_start; l < Aia_stop; l++){
			k=this->ja[l]; //loop k s.t. A(i,k) != 0;
			b_idx.push_back(B.ia[k]); //index of first val in row k for matrix B
										//j = B.ja[b_idx] first j such that B(k,j)!=0;
			b_stop.push_back(B.ia[k+1]); //at b_stop the next row starts.
		}

		//b_idx contains now the indices of the values in B.a of the rows k  with A(i,k) != 0
		//has size Aia_stop-Aia_start.

		//loop the j such that A*B(i,j)!=0
		while(true){
			//find next j value : the first column with unconsidered values
			//i.e. will multiply row i of A with column next_j
			next_j = B.getm()+2; //invalid index for break condition
			for(int l = 0; l < Aia_stop-Aia_start; l++){
				//value not considered and in a column earlier than next_j
				if( b_idx[l] < b_stop[l] && next_j >B.ja[b_idx[l]]){
					next_j = B.ja[b_idx[l]];
				}
			}

			//break condition
			if(next_j > B.getm()) 
				break;

			//calculate A*B(i,next_j)
			val = 0;
			for(int l=Aia_start,l2 = 0; l < Aia_stop; l++,l2++){
				//"B(k,next_j)!=0" i.e. multiplicating row i with column j
				if(b_idx[l2] < b_stop[l2]&&B.ja[b_idx[l2]] == next_j){
					val+=this->a[l]*B.a[b_idx[l2]];
					b_idx[l2]++;//this row is done. Advance to next non zero row
				}
			}

			if(val!= 0)
			{
				//store values
				AB.japush_back(next_j);
				AB.apush_back(val);

			}
		}

		//adapt AB.ia
		AB.iapush_back(AB.a.size());

		
	}
		
	AB.forceNrColumns(B.getm());

	return AB;
}

cpuCSRMatrix cpuCSRMatrix::operator*( float other )
{
	cpuCSRMatrix A = *this;
	A*= other;
	return A;
}


cpuCSRMatrix cpuCSRMatrix::operator%( cpuCSRMatrix & B )
{
	assert(B.getm() == this->getm());
	bool emptyLineFound = false;
	cpuCSRMatrix AB;

	AB.ia.reserve(this->dim());
	AB.ja.reserve(3*this->dim());
	AB.a.reserve(3*this->dim());
	AB.iapush_back(0);

	int Aia_start, Aia_stop, next_j;
	float val;
//	std::vector<int> b_idx, b_stop;
	int Bia_start, Bia_stop;

	for(int i = 0; i < dim(); i++){
		//new this.ia[i]
		Aia_start = this->ia[i];
		Aia_stop = this->ia[i+1];

		//loop the j such that A*B(i,j)!=0
		for(int j = 0; j < B.dim(); j++){
			//find next j value
			next_j = j; 
			Bia_start = B.ia[j] ;
			Bia_stop = B.ia[j+1] ; // first index of next row

			if(Bia_start == Bia_stop|| Aia_start == Aia_stop ||
				this->ja[Aia_start] > B.ja[Bia_stop-1] || 
				this->ja[Aia_stop-1] < B.ja[Bia_start]){
					continue;
			}

			//calculate (A*B^T)(i,next_j)
			val = 0;
			for(int l=Aia_start, l2 = Bia_start; l < Aia_stop && l2 < Bia_stop;){
				//"B(k,next_j)!=0"
				while(B.ja[l2] < this->ja[l] && l2 < Bia_stop){
					l2++;
				}
				while(this->ja[l] <B.ja[l2] && l < Aia_stop){
					l++;
				}

				if(B.ja[l2] == this->ja[l] && l<Aia_stop && l2 < Bia_stop){
					val+=this->a[l]*B.a[l2];
					l2++;
					l++;
				}
			}

			if(val!=0){
				//store values
				AB.japush_back(next_j);
				AB.a.push_back(val);
			}
		}


		//adapt AB.ia
		AB.iapush_back(AB.a.size());

/*		if(AB.ia.back() == AB.a.size()){
			emptyLineFound = true;
		}
		else if( !emptyLineFound){
			AB.iapush_back(AB.a.size());
		}
		else{
			//cout << "Matrix degenerated!in pardisoMatrix%";
			assert(false);
			throw std::runtime_error("Matrix degenerated in pardisoMatrix::%");
		}*/


	}

			/*if(AB.getm()!= B.getn()){
			AB.forceNrColumns(B.getn());
		}
		//the last lines are empty...
		if(AB.getn() != this->getn()){
			for(int i = AB.ia.size(); i< this->getn(); i++){
				AB.ia.push_back(AB.a.size());
			}
		}*/

	//empty last column...
	if(AB.getm()!= B.getn()){
		AB.forceNrColumns(B.getn());
	}

	return AB;
}

cpuCSRMatrix cpuCSRMatrix::operator+( cpuCSRMatrix & B )
{
	assert(getn() == B.getn() && getm() == B.getm());
	cpuCSRMatrix AnB;
	AnB.ia.reserve(this->dim());
	AnB.ja.reserve(3*this->dim());
	AnB.a.reserve(3*this->dim());
	AnB.iapush_back(0);
	
	int Aia_start, Aia_stop, Bia_start, Bia_stop;
	int j1, j2;
	float val;
	
	for(int i = 0; i < dim(); i++){
		Aia_start = this->ia[i];
		Aia_stop = this->ia[i+1];
		Bia_start = B.ia[i];
		Bia_stop = B.ia[i+1];
		for(j1 = Aia_start, j2 = Bia_start; j1 <Aia_stop || j2 < Bia_stop;){
			if(j2 >= Bia_stop || this->ja[j1]< B.ja[j2] && j1 < Aia_stop){
				val = this->a[j1];
//				if(val!=0){
					AnB.japush_back(this->ja[j1]);
					AnB.apush_back(val);
//				}
				j1++;
			}
			else if (j1 >= Aia_stop || this->ja[j1]> B.ja[j2] && j2 < Bia_stop){
				val = B.a[j2];
//				if(val!= 0){
					AnB.japush_back(B.ja[j2]);
					AnB.apush_back(val);
//				}
				j2++;
			}
			else{
				val = B.a[j2] + this->a[j1];
//				if(val != 0){
					AnB.japush_back(B.ja[j2]);
					AnB.apush_back(val);
//				}
				j1++;
				j2++;
			}

		} 	

		//adapt AB.ia
		AnB.iapush_back(AnB.a.size());
	}
	AnB.forceNrColumns(getm());
	return AnB;
}


cpuCSRMatrix cpuCSRMatrix::operator-( cpuCSRMatrix & B )
{
	assert(getn() == B.getn() && getm() == B.getm());
	cpuCSRMatrix AnB;
	AnB.ia.reserve(this->dim());
	AnB.ja.reserve(3*this->dim());
	AnB.a.reserve(3*this->dim());
	AnB.iapush_back(0);

	int Aia_start, Aia_stop, Bia_start, Bia_stop;
	int j1, j2;
	float val;

	for(int i = 0; i < dim(); i++){
		Aia_start = this->ia[i];
		Aia_stop = this->ia[i+1];
		Bia_start = B.ia[i];
		Bia_stop = B.ia[i+1];
		for(j1 = Aia_start, j2 = Bia_start; j1 <Aia_stop || j2 < Bia_stop;){
			if(j2 >= Bia_stop|| (j1 < Aia_stop && this->ja[j1]< B.ja[j2]) ){

				val = this->a[j1];
				if(val!=0){
					AnB.japush_back(this->ja[j1]);
					AnB.apush_back(val);
				}
				j1++;
			}
			else if (j1 >= Aia_stop || (j2 < Bia_stop && this->ja[j1]> B.ja[j2]) ){
				val = - B.a[j2];
				if(val!= 0){
					AnB.japush_back(B.ja[j2]);
					AnB.apush_back(val);
				}
				j2++;
			}
			else{
				val = -B.a[j2] + this->a[j1];
				if(val!= 0){
					AnB.japush_back(B.ja[j2]);
					AnB.apush_back(val);
				}
				j1++;
				j2++;
			}

		} 	

		//adapt AB.ia
		AnB.iapush_back(AnB.a.size());
	}
	AnB.forceNrColumns(getm());
	return AnB;
}


void cpuCSRMatrix::operator*=( float  other )
{
	for(unsigned int i = 0; i < a.size(); i++){
		a[i]*=other;
	}
}



void cpuCSRMatrix::elementWiseInv()
{
	for(unsigned int i = 0; i < a.size(); i++){
			a[i] = 1.f/ a[i];
	}
}

void cpuCSRMatrix::clear()
{
	a.clear();
	ia.clear();
	ja.clear();
	n = 0;
	m= 0;
}

void cpuCSRMatrix::mult( std::vector<float> & x, std::vector<float> & target, bool adaptTarget )
{
	assert(x.size() == getm() && (adaptTarget|| target.size() == getn()));
	assert(&target != &x);

	if(adaptTarget){
		target.clear();
		target.reserve(getn());
	}
	for(int i = 0; i < getn(); i++){
		if(adaptTarget){
			target.push_back(0);
		}
		target[i] = 0;
		for(int j = ia[i]; j < ia[i+1]; j++){
			target[i] += x[ja[j]]*a[j];
		}
	}
}

void cpuCSRMatrix::mult( std::vector<tuple3f> & x, std::vector<tuple3f> & target, bool adaptTarget /*= false*/ )
{
	assert(x.size() == getm() && (adaptTarget|| target.size() == getn()));
	assert(&target != &x);

	if(adaptTarget){
		target.clear();
		target.reserve(getn());
	}
	for(int i = 0; i < getn(); i++){
		if(adaptTarget){
			target.push_back(tuple3f());
		}
		target[i].set(0,0,0);
		for(int j = ia[i]; j < ia[i+1]; j++){
			target[i].x += (float) x[ja[j]].x*a[j];
			target[i].y += (float) x[ja[j]].y*a[j];
			target[i].z += (float) x[ja[j]].z*a[j];
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// set line line to 0.... 1 ... 0 where line is the line in 0 based notation
//////////////////////////////////////////////////////////////////////////
void cpuCSRMatrix::setLineToID( int line )
{
	bool diagElementExisted = false;
	for(int j = ia[line]; j < ia[line+1]; j++){
		if(ja[j] == line){
			a[j] = 1;
			diagElementExisted = true;
		}
		else{
			a[j] = 0;
		}
	}

	assert(diagElementExisted);
	if(!diagElementExisted){
		throw std::runtime_error( "Unimplemented Case in setLineToID(), curse the lazy programmer who preferred to write this error msg than to implement a better method");
	}
	//was lazy and did not yet implement this method if the diag element does not exist.

}

void cpuCSRMatrix::setLineToZero( int line )
{
	for(int j = ia[line]; j < ia[line+1]; j++){
		a[j] = 0;
	}
}

void cpuCSRMatrix::scaleLine( int line, float scale )
{
	for(int j = ia[line]; j < ia[line+1]; j++){
		a[j] *= scale;
	}
}

cpuCSRMatrix cpuCSRMatrix::transpose( cpuCSRMatrix & mat )
{
	cpuCSRMatrix id;
	id.initMatrix(idCreator(),mat.m);
	return id % mat;
}


//////////////////////////////////////////////////////////////////////////
// force the number columns to have nrColums size.
// This might be needed if you want to assure the dimension of the matrix
// are n x nrColumns when the last columns are empty. ( this sparse format
// would ignore empty trailing columns)
void cpuCSRMatrix::forceNrColumns( int nrColumns )
{
	assert(m <=nrColumns); // else the matrix will behave strangely..
	m= nrColumns;
}

//returns the zero based indices of values in this column and the values.
void cpuCSRMatrix::getLine( int line, std::vector<int> & target_ind, std::vector<float> & target_vals )
{
	assert(line < getn());
	target_vals.clear();
	target_ind.clear();
	for(int i = ia[line]; i < ia[line+1]; i++){
		target_ind.push_back(ja[i]);
		target_vals.push_back(a[i]);
	}
}

void cpuCSRMatrix::diagAppend( cpuCSRMatrix & mat )
{
	assert(&mat != this);
	int newm = m + mat.m;
	int newn = n + mat.n;
	int oldm = m;
	int oldn = n;
	int oldszA = a.size();

	//the values...
	for(unsigned int i = 0; i < mat.a.size(); i++){
		this->apush_back(mat.a[i]);
	}
	//the indices
	for(unsigned int i = 1; i < mat.ia.size(); i++){
		this->iapush_back(mat.ia[i] + oldszA);
	}
	//the j indices
	for(unsigned int i = 0; i < mat.ja.size(); i++){
		this->japush_back(mat.ja[i] + oldm);
	}
	assert(this->n = newn);
	assert(this->m = newm);

}


void cpuCSRMatrix::append( cpuCSRMatrix & mat )
{
	assert(&mat != this);
	int newm = m;
	int newn = n + mat.n;
	//int oldm = m;
	//int oldn = n;
	int oldszA = a.size();

	//the values...
	for(unsigned int i = 0; i < mat.a.size(); i++){
		this->apush_back(mat.a[i]);
	}
	//the indices
	for(unsigned int i = 1; i < mat.ia.size(); i++){
		this->iapush_back(mat.ia[i] + oldszA);
	}
	//the j indices
	for(unsigned int i = 0; i < mat.ja.size(); i++){
		this->japush_back(mat.ja[i]);
	}
	assert(this->n = newn);
	assert(this->m = newm);
}



void cpuCSRMatrix::normalizeLines()
{
	float val;
	for(int i = 0; i < getn(); i++){
		val = get(i,i);
		if((1.f/val)*0 == 0){
			scaleLine(i,1.f/val);
		}
	}
}










