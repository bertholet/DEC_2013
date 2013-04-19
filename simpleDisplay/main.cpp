#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QGLFormat>


int main(int argc, char *argv[])
{
	
	
	QApplication a(argc, argv);
	
	//QApplication a(argc, argv);

	// Specify an OpenGL 3.3 format using the Core profile.
	// That is, no old-school fixed pipeline functionality
	QGLFormat glFormat;
	glFormat.setVersion( 4, 1 );
	glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
	glFormat.setSampleBuffers( true );

	MainWindow w(glFormat);
	w.show();
	return a.exec();
}
