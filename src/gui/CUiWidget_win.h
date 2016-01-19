/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIWIDGET_WIN_H
#define PRODRAW_CUIWIDGET_WIN_H

#include <QtGui>
#include <widgets/CUiWidget>

class CUiWidget_win;
class CActionManager;

class CUiWidget_win: public CUiWidget
{
	Q_OBJECT

public:
	CUiWidget_win(QWidget *parent = NULL);
	virtual ~CUiWidget_win();

protected:
	void paintEvent(QPaintEvent *event);
	void setupUi(QLayout *globalLayout, CActionManager *actionManager);

}; // End of class CUiWidget_Win

#endif // PRODRAW_CUIWIDGET_WIN_H