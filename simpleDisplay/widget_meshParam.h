#pragma once
#include <QWidget>

class meshParamWidget : public QWidget
{
	Q_OBJECT
public:
	meshParamWidget(QWidget *parent = NULL);
	~meshParamWidget(void);

	int outerMode;
	int weightMode;

public slots:
	void oneBorder(void);
	void multipleBorders( void );
	void setBorderMode( int mode );
};
