#include "StdAfx.h"
#include "wfmesh.h"
#include "OBIFileReader.h"
#include <iostream>
#include <algorithm>


wfMesh::wfMesh(void)
{
}

wfMesh::wfMesh( const char* file )
{
	OBIFileReader reader;
	reader.parse(file);
	reader.initializeMesh(*this);
}

/*wfMesh::wfMesh( const char* file, tuple3f col )
{
	init(file,col,1.f);
}

wfMesh::wfMesh( const char* file, tuple3f col, float scale )
{
	init(file,col,scale);
}*/

wfMesh::~wfMesh(void)
{
	updateObserver(MESH_DELETED);
}

/*void wfMesh::init( const char* file, tuple3f & col, float scale )
{
	/*OBIFileReader f;
	f.parse(file);
	vertices = f.getVertices();
	faces = f.getFaces();


	if(f.getNormals().size() != 0){
		normals = f.getNormals();
		face_normals_perVertex = f.getFaceNormals();
	}
	else{
		initNormalsFromVertices();
	}

	tex = f.getTex();
	face_tex = f.getFaceTextures();*/


	/*cout << " Max Area Ratio " << Operator::maxAreaRatio(*this) << "\n";
	cout << " Checking Orientation... " ;
	cout << " Consistent = " << (meshOperation::consistentlyOriented(*this)? "yes" : "no") << "\n";*/
/*}*/


void wfMesh::initNormalsFromVertices()
{
	//cout << ">> Normals are generated, no normals read!\n";
	//vector<int> nrFaces(vertices.size());
	normals_per_vertex.clear();
	normals_per_vertex.reserve(vertices.size());
	normals_per_vertex.assign(vertices.size(), tuple3f());

	tuple3f temp_normal;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		temp_normal = tuple3f::cross(vertices[faces[i].b]-vertices[faces[i].a], vertices[faces[i].c] - vertices[faces[i].b]);
		temp_normal.normalize();
		normals_per_vertex[faces[i].a] += temp_normal;
		normals_per_vertex[faces[i].b] += temp_normal;
		normals_per_vertex[faces[i].c] += temp_normal;
	}

	for(unsigned int i = 0; i < normals_per_vertex.size(); i++){
		normals_per_vertex[i].normalize();
	}


	//face_normals_perVertex = faces;
	//cout << ">> Normals generated!\n";
}

void wfMesh::reset( vector<tuple3f> & _vertices, vector<tuple3i> &_faces )
{
	vertices = _vertices;
	faces = _faces;
	this->initNormalsFromVertices();
}


vector<tuple3f> & wfMesh::getNormals()
{
	return this->normals_per_vertex;
}




void wfMesh::addOrthogonalNoise( float max )
{
	for(unsigned int i = 0; i < vertices.size(); i++){
		vertices[i] += normals_per_vertex[i] *( ((float)rand())/RAND_MAX * max );
	}
}

void wfMesh::addUniformNoise( float max )
{
	tuple3f randm;
	for(unsigned int i = 0; i < vertices.size(); i++){
		randm = tuple3f(((float)rand())/RAND_MAX *max,((float)rand())/RAND_MAX * max,((float)rand())/RAND_MAX * max);
		vertices[i]+= randm;
	}
}

void wfMesh::scaleVertices( float scale )
{
	for(unsigned int i = 0; i < vertices.size(); i++){
		vertices[i] *= scale;
	}
}

void wfMesh::translateVertices( float dx,float dy,float dz )
{
	for(unsigned int i = 0; i < vertices.size(); i++){
		vertices[i].x += dx;
		vertices[i].y += dy;
		vertices[i].z += dz;
	}
}

void wfMesh::translateVertices( std::vector<tuple3f> & translation, float scale )
{
	assert(translation.size() == vertices.size());
	for(unsigned int i = 0; i< vertices.size(); i++){
		vertices[i]+=translation[i]*scale;
	}
}


void wfMesh::normalize( void )
{
	if(vertices.size() == 0)
		return;

	float maxx= vertices[0].x, minx = vertices[0].x,
		 maxy= vertices[0].y, miny = vertices[0].y,
		  maxz= vertices[0].z, minz = vertices[0].z;

	for(vector<tuple3f>::iterator it = vertices.begin(); it != vertices.end(); it++){
		if(it->x > maxx){
			maxx = it->x;
		}
		if(it->y > maxy){
			maxy = it->y;
		}
		if(it->z > maxz){
			maxz = it->z;
		}
		if(it->x < minx){
			minx = it->x;
		}
		if(it->y < miny){
			miny = it->y;
		}
		if(it->z < minz){
			minz = it->z;
		}
	}

	float max_min = (maxx-minx > maxy - miny? maxx-minx:maxy-miny);
	max_min = (max_min > maxz-minz? max_min : maxz-minz);


	float scale = 2.f/max_min;
	translateVertices(-(maxx+minx)/2,-(miny +maxy)/2,-(minz+maxz)/2);
	scaleVertices(scale);


}




void wfMesh::setVertices( vector<tuple3f> & verts )
{
	this->vertices = verts;
}

void wfMesh::setFaces( vector<tuple3i> & facs )
{
	this->faces = facs;
}

void wfMesh::setNormals( vector<tuple3i>& v_per_f_normals, vector<tuple3f> & normls )
{
	/*this->face_normals_perVertex = v_per_f_normals;
	this->normals = normls;*/
	if(normls.size() == vertices.size()){
		//assume this is already normals specified by vertices.
		this->normals_per_vertex = normls;
		return;
	}

	normals_per_vertex.clear();
	normals_per_vertex.reserve(vertices.size());
	normals_per_vertex.assign(vertices.size(), tuple3f());

	for(unsigned int i = 0; i < faces.size(); i++){
		normals_per_vertex[faces[i].a] += normls[v_per_f_normals[i].a];
		normals_per_vertex[faces[i].b] += normls[v_per_f_normals[i].b];
		normals_per_vertex[faces[i].c] += normls[v_per_f_normals[i].c];
	}

	for(unsigned int i = 0; i < normals_per_vertex.size(); i++){
		normals_per_vertex[i].normalize();
	}
}

void wfMesh::setTextures_perVertex( double * x, double * y )
{
	tex.clear();
	tex.reserve(vertices.size());

	for(unsigned int i = 0; i < vertices.size(); i++){
		tex.push_back(tuple3f((float)x[i], (float)y[i],0.f));
	}

	this->face_tex = faces;
}


void wfMesh::setTextures_perVertex( double * xy )
{
	tex.clear();
	tex.reserve(vertices.size());

	for(unsigned int i = 0; i < vertices.size(); i++){
		tex.push_back(tuple3f((float)xy[i], (float)xy[i + vertices.size()],0.f));
	}

	this->face_tex = faces;
}

void wfMesh::setTextures_perVertex( vector<tuple3f> & textures )
{
	tex.clear();
	tex.reserve(vertices.size());

	for(unsigned int i = 0; i < vertices.size(); i++){
		tex.push_back(textures[i]);
	}

	this->face_tex = faces;
}

/************************************************************************/
/* Attach an Observer to this mesh
/* Observers will be notified about changes in the mesh structure
/* or positions
/************************************************************************/
void wfMesh::attach( meshObserver * o )
{
	this->observer.push_back(o);
}
/************************************************************************/
/* Detatch an observer from this mesh                       */
/************************************************************************/
void wfMesh::detatch(meshObserver * obs )
{
	vector<meshObserver * >::iterator it;
	it = find(observer.begin(), observer.end(), obs);
	if(it != observer.end()){
		this->observer.erase(it);
	}
	else{
		assert(false);
	}
}

void wfMesh::updateObserver( meshMsg msg )
{
	for(unsigned int i = 0; i < observer.size(); i++){
		observer[i]->update(this, msg);
	}
}

tuple3i * wfMesh::intersect( tuple3f & start,tuple3f &to, int * closestVertex,
						  int * face, tuple3f & intersectPosition)
{

	//matrixf world2obj = 
	//tuple3f startObj, endObj;
	
	int res = -1;
	float t,d; // t= dist.
	float bestdist = 1000000;
	tuple3f normal,v, insect, sidenormal, bestIntersect;

	for(unsigned int i = 0; i < faces.size(); i++){
		normal = vertices[faces[i].b]- vertices[faces[i].a];
		normal = normal.cross(vertices[faces[i].c] -vertices[faces[i].a]);
		normal.normalize();

		v = to - start; 
		v.normalize();
		d= vertices[faces[i].a].dot(normal);
		//t = -(normal.dot(start) + d)/v.dot(normal);
		t= (vertices[faces[i].a] - start).dot(normal) / v.dot(normal);

		if(t > bestdist){
			continue;
		}

		insect = start + v*t;

		sidenormal = -(vertices[faces[i].a] -start).cross(vertices[faces[i].b] -start);
		if(sidenormal.dot(insect) -sidenormal.dot(start) < 0){
			continue;
		}

		sidenormal = -(vertices[faces[i].b] -start).cross(vertices[faces[i].c] -start);
		if(sidenormal.dot(insect) -sidenormal.dot(start) < 0){
			continue;
		}

		sidenormal = -(vertices[faces[i].c] -start).cross(vertices[faces[i].a] -start);
		if(sidenormal.dot(insect) -sidenormal.dot(start) < 0){
			continue;
		}
		res = i;
		bestdist = t;
		bestIntersect = insect;
	}

	if(res>-1){
		tuple3i * result = & (faces[res]);
		*face = res;
		intersectPosition.set(bestIntersect);

		if((vertices[result->a] - bestIntersect).norm() < (vertices[result->b] - bestIntersect).norm()){
			if((vertices[result->a] - bestIntersect).norm()< (vertices[result->c] - bestIntersect).norm()){
				*closestVertex = result->a;
			}
			else{
				*closestVertex = result->c;
			}
		}
		else{
			if((vertices[result->b] - bestIntersect).norm()< (vertices[result->c] - bestIntersect).norm()){
				*closestVertex = result->b;
			}
			else{
				*closestVertex = result->c;
			}
		}

		return result;
	}
	return NULL;
}

void wfMesh::setTexture( vector<tuple3i> & tex_per_f_per_v, vector<tuple3f> & texture )
{
	this->face_tex = tex_per_f_per_v;
	this->tex = texture;
}

vector<tuple3f>& wfMesh::getVertices( void )
{
	return vertices;
}

vector<tuple3i>& wfMesh::getFaces()
{
	return faces;
}

/*vector<tuple3i>& wfMesh::getFaces2Normals()
{
	return face_normals_perVertex;
}*/

vector<tuple3f> & wfMesh::getTexCoords()
{
	return tex;
}
























