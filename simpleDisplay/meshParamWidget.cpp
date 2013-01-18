#include "meshParamWidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>

#include "application_meshParam.h"
#include "MODEL.h"



meshParamWidget::meshParamWidget(QWidget *parent)
: QWidget(parent)
{
	outerMode =0;
	weightMode = 0;
	QPushButton * butt = new QPushButton("One Border");
	connect(butt, SIGNAL(released()), this, SLOT(oneBorder()));
	
	QPushButton * butt_neuman = new QPushButton("Multiple Borders");
	connect(butt_neuman, SIGNAL(released()), this, SLOT(multipleBorders()));
	
	/*QPushButton * butt2 = new QPushButton("Conformal Borders");
	connect(butt2, SIGNAL(released()), this, SLOT(conformalBorder()));*/
	QLabel * label1 = new QLabel("Outer Border: ");
	//QLabel * label2 = new QLabel("Weights: ");

	QComboBox * comboBox = new QComboBox();
	comboBox->setEditable(false);
	comboBox->addItem("Circle");
	comboBox->addItem("Circle Weighted");
	comboBox->addItem("Conformal");
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderMode(int)));


	/*QComboBox * comboBox2 = new QComboBox();
	comboBox2->setEditable(false);
	comboBox2->addItem("Cotan Voronoi");
	comboBox2->addItem("Cotan Mixed");
	comboBox2->addItem("Mean Value");
	comboBox2->addItem("Uniform");
	connect(comboBox2, SIGNAL(currentIndexChanged(int)), this, SLOT(setWeightMode(int)));*/

	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(butt);
	layout->addWidget(butt_neuman);
	layout->addWidget(label1);
	layout->addWidget(comboBox);
	/*layout->addWidget(label2);
	layout->addWidget(comboBox2);
	layout->addWidget(butt2);*/
	this->setLayout(layout);
}

meshParamWidget::~meshParamWidget(void)
{
}

void meshParamWidget::oneBorder( void )
{
	application_meshParam meshParam;

	int nrBorders = MODEL::getModel()->getMesh()->getBoundaryEdges().size();

	if(nrBorders == 1){
		switch(outerMode){
		case 0:
			meshParam.calcTexturePos(*MODEL::getModel(), & application_meshParam::circleBorder);
			break;
		case 1:
			meshParam.calcTexturePos(*MODEL::getModel(), 
				& application_meshParam::distWeightCircleBorder);
			break;
		case 2:
			meshParam.calcTexturePos(*MODEL::getModel(), 
				& application_meshParam::conformalBorder);
			break;
		}
	}
}

void meshParamWidget::multipleBorders( void )
{
	application_meshParam meshParam;
	int nrBorders = MODEL::getModel()->getMesh()->getBoundaryEdges().size();
	if(nrBorders >= 1){
		switch(outerMode){
		case 0:
			meshParam.calcTexturePosMultiborder(*MODEL::getModel(), & application_meshParam::circleBorder);
			break;
		case 1:
			meshParam.calcTexturePosMultiborder(*MODEL::getModel(), 
				& application_meshParam::distWeightCircleBorder);
			break;
		case 2:
			meshParam.calcTexturePosMultiborder(*MODEL::getModel(), 
				& application_meshParam::conformalBorder);
			break;
		}
	}
}



void meshParamWidget::setBorderMode( int mode )
{
	//throw std::exception("The method or operation is not implemented.");
	outerMode = mode;
}


