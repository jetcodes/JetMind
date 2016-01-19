/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <widgets/CUiWidget>

#include "CTitleBar.h"
#include "CUiWidget_win.h"

CUiWidget_win::CUiWidget_win(QWidget *parent)
	: CUiWidget(parent)
{
}

CUiWidget_win::~CUiWidget_win()
{
}

void CUiWidget_win::setupUi(QLayout *globalLayout, CActionManager *actionManager)
{
	QWidget		*bar = NULL;

	bar = new CTitleBar(this);
	connect(this, SIGNAL(docTitleChanged(QString)), bar, SLOT(onDocTitleChanged(QString)));

	setTitleBar(bar);

	m_centralWidget->setLayout(globalLayout);
}

void CUiWidget_win::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	QRect				r;
    //QPainterPath		path;
    //QRegion				region;
    //int					xroundness = 1, yroundness = 2;

	r = rect();
	opt.init(this);

    /*
	// isMaximized doesn't work here, because we change
	// the maximize logic, we can use the new 'widgetState' method
	if (isFullScreen() || widgetState() == Qt::WindowMaximized )
	{
		setMask(region);
	}
	else
	{
        path.addRoundRect(0, 0, r.width(), r.height(), xroundness, yroundness);
        p.setClipPath(path);
        region = p.clipRegion();
        setMask(region);
	}
    */
    QPalette    defaultPalette = QApplication::palette(this);
    /*
    QColor      c;
    QPixmap     pixmap(":/res/icons/woodbackground.png");

    for (int i = 0; i < QPalette::NColorGroups; ++i)
    {
        c = defaultPalette.brush(QPalette::ColorGroup(i), QPalette::Window).color();
        defaultPalette.setBrush(QPalette::ColorGroup(i), QPalette::Window, pixmap);
    }
    */
    setPalette(defaultPalette);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
