#pragma once
#include <QGLWidget>

/************************************************************************/
/* Just a wrapper for VAOs                                    */
/************************************************************************/


class myVAO
{
private:
	typedef void (WINAPI *GL_GEN_VARRAYS)(GLsizei , GLuint *);
	typedef void (WINAPI *GL_BIND_VARRAY)(GLuint);
	typedef void (WINAPI *GL_DELETE_VARRAY)(GLsizei , GLuint *);
	GL_GEN_VARRAYS glGenVertexArrays;
	GL_BIND_VARRAY glBindVertexArray;
	GL_DELETE_VARRAY glDeleteVertexArrays;

	uint vao;
	bool created;
public:
	myVAO(void);
	~myVAO(void);

	void create();
	bool isCreated();
	void bind();
	void unbind();
};

