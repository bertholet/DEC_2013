#include "glDebuggingStuff.h"
#include <QTOpenGL>
#include <GL/GLU.h>
#include <QDebug>
#include <iostream>
#include <string>


glDebuggingStuff::glDebuggingStuff(void)
{
}


glDebuggingStuff::~glDebuggingStuff(void)
{
}

void glDebuggingStuff::didIDoWrong()
{
	GLenum errCode;
	std::string errString;
	errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
		errString = std::string( (char*) gluErrorString(errCode));
		qWarning() << "*** GLError: " << errString.c_str();
	}

}
