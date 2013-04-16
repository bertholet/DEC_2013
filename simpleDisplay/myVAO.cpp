#include "myVAO.h"
#include "glDebuggingStuff.h"


myVAO::myVAO(void)
{
	glGenVertexArrays = NULL;
	glBindVertexArray = NULL;
	glDeleteVertexArrays = NULL;
	created = false;
}


myVAO::~myVAO(void)
{
	if(created){
		glDeleteVertexArrays(1, &vao);
		//glDebuggingStuff::didIDoWrong();
	}
}

void myVAO::create()
{
	if(!created){
		glGenVertexArrays = (GL_GEN_VARRAYS) wglGetProcAddress("glGenVertexArrays");
		glBindVertexArray = (GL_BIND_VARRAY) wglGetProcAddress("glBindVertexArray");
		glDeleteVertexArrays = (GL_DELETE_VARRAY) wglGetProcAddress("glDeleteVertexArrays");
		
		glGenVertexArrays(1, &vao);
		glDebuggingStuff::didIDoWrong();
		created = true;
	}

}

void myVAO::bind()
{
	if(!created){
		create();
	}
	glBindVertexArray(vao);
	glDebuggingStuff::didIDoWrong();
}

void myVAO::unbind()
{
	if(created){
		glBindVertexArray(0);
	}
	glDebuggingStuff::didIDoWrong();
}

bool myVAO::isCreated()
{
	return created;
}
