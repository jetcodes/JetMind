/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiWidget.h"

CUiBorder::CUiBorder(CUiWidget *parent, int index, const Qt::CursorShape &cursor, QString &image, QString &repeat)
	: QWidget(parent),
	  m_parent(parent),
	  m_index(index),
	  m_dragging(false)
{
	QString		tpl = "background: url(%1);background-repeat: %2;";
	
	setCursor( cursor );
	setStyleSheet( tpl.arg(image).arg(repeat) );
}

CUiBorder::~CUiBorder()
{
}

void CUiBorder::paintEvent(QPaintEvent *event)
{	
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiBorder::mousePressEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		event->ignore();
	}
	else
	{
		m_dragging = true;
		QWidget::mousePressEvent(event);
	}
}

void CUiBorder::mouseMoveEvent(QMouseEvent *event)
{	
	if (m_dragging && m_parent)
	{
		m_parent->doResize(this, event->globalPos());
		event->accept();
	}
	else
	{
		QWidget::mouseMoveEvent(event);
	}
}

void CUiBorder::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
	{
		event->ignore();
	}
	else
	{
		m_dragging = false;
		QWidget::mouseReleaseEvent(event);	
	}
}

CUiWidget::CUiWidget(QWidget *parent)
    : QFrame(parent),
	  m_border(4),
	  m_titleBar(NULL),
      m_centralWidget(NULL),
	  m_currState(Qt::WindowNoState),
	  m_prevState(Qt::WindowNoState),
	  m_mainLayout(NULL),
	  m_navLayout(NULL),
	  m_topLayout(NULL),
	  m_centerLayout(NULL),
	  m_bottomLayout(NULL)
{
	QWidget		*w = NULL;

    setMouseTracking(true);

    initBorders();

	m_centralWidget = new QWidget(this);

	m_mainLayout = new QVBoxLayout();
	m_navLayout = new QVBoxLayout();
	m_topLayout = new QHBoxLayout();
	m_bottomLayout = new QHBoxLayout();
	m_centerLayout = new QHBoxLayout();
	m_mainLayout->setMargin(0);
	m_mainLayout->setSpacing(0);
	m_navLayout->setMargin(0);
	m_navLayout->setSpacing(0);
	m_topLayout->setMargin(0);
	m_topLayout->setSpacing(0);
	m_bottomLayout->setMargin(0);
	m_bottomLayout->setSpacing(0);
	m_centerLayout->setMargin(0);
	m_centerLayout->setSpacing(0);

	m_topLayout->addWidget(m_borders[LeftTop]);
	m_topLayout->addWidget(m_borders[Top], 1);
	m_topLayout->addWidget(m_borders[RightTop]);

	w = new QWidget(this);
	w->setLayout(m_navLayout);
	m_navLayout->addWidget(m_centralWidget, 1);

	m_centerLayout->addWidget(m_borders[Left]);
	m_centerLayout->addWidget(w, 1);
	m_centerLayout->addWidget(m_borders[Right]);

	m_bottomLayout->addWidget(m_borders[LeftBottom]);
	m_bottomLayout->addWidget(m_borders[Bottom], 1);
	m_bottomLayout->addWidget(m_borders[RightBottom]);

	w = new QWidget(this);
	w->setLayout(m_topLayout);
	m_mainLayout->addWidget(w);

	w = new QWidget(this);
	w->setLayout(m_centerLayout);
	m_mainLayout->addWidget(w, 1);

	w = new QWidget(this);
	w->setLayout(m_bottomLayout);
	m_mainLayout->addWidget(w);

    QWidget::setLayout(m_mainLayout);

	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

CUiWidget::~CUiWidget()
{

}

void CUiWidget::setTitleBar(QWidget *titleBar)
{
    if (!titleBar && m_titleBar )
        m_titleBar->setVisible(false);

    if (titleBar != m_titleBar && m_navLayout)
    {
        m_navLayout->removeWidget(m_titleBar);
        m_titleBar->deleteLater();
        m_titleBar = titleBar;
        m_navLayout->insertWidget(0, m_titleBar);
    }
}

QWidget* CUiWidget::titleBar() const
{
    return m_titleBar;
}

void CUiWidget::initBorders()
{
	int				style;
	CUiBorder			*border = NULL;
	Qt::CursorShape cursors[8] = {\
		Qt::SizeVerCursor,\
		Qt::SizeFDiagCursor,\
		Qt::SizeHorCursor,\
		Qt::SizeBDiagCursor,\
		Qt::SizeVerCursor,\
		Qt::SizeFDiagCursor,\
		Qt::SizeHorCursor,\
		Qt::SizeBDiagCursor, \
	};

	QString			images[8] = {\
		":/res/skin/app-border-top.png",\
		":/res/skin/app-border-lefttop.png",\
		":/res/skin/app-border-left.png",\
		":/res/skin/app-border-leftbottom.png",\
		":/res/skin/app-border-bottom.png",\
		":/res/skin/app-border-rightbottom.png",\
		":/res/skin/app-border-right.png",\
		":/res/skin/app-border-righttop.png"\
	};

	QString			repeats[8] = {\
		"repeat-x",\
		"no-repeat",\
		"repeat-y",\
		"no-repeat",\
		"repeat-x",\
		"no-repeat",\
		"repeat-y",\
		"no-repeat"\
	};

	for (int i=CUiWidget::Top; i<=CUiWidget::RightTop; ++i)
	{
		border = new CUiBorder(this, i, cursors[i], images[i], repeats[i]);
		border->setMinimumWidth(m_border);
		border->setMinimumHeight(m_border);
		m_borders.append(border);
	}
}

void CUiWidget::doResize(CUiBorder *border, const QPoint &p)
{
	int		index = 0;
	int		w, h, dy, dx;
	QRect	r = geometry();
	int		top = r.top(), left = r.left();
	int		bottom = r.bottom(), right = r.right();
	QPoint	c;
	QSize	s;

	if (!border || !border->isDragging())
		return;

	index = border->index();
	switch (index)
	{
	case Top:
		w = r.width();
		h = bottom - p.y();
		dy = h - r.height();
		c = QPoint(r.x(), r.y() - dy);
		break;

	case LeftTop:
		w = right - p.x();
		h = bottom - p.y();
		dx = w - r.width();
		dy = h - r.height();
		c = QPoint(r.x() - dx, r.y() - dy);
		break;

	case Left:
		w = right - p.x();
		h = r.height();
		dx = w - r.width();
		c = QPoint(r.x() - dx, r.y());
		break;

	case LeftBottom:
		w = right - p.x();
		h = p.y() - top;
		dx = w - r.width();
		c = QPoint(r.x() - dx, r.y());
		break;

	case Bottom:
		w = r.width();
		h = p.y() - top;
		c = r.topLeft();
		break;

	case RightBottom:
		w = p.x() - left;
		h = p.y() - top;
		c = r.topLeft();
		break;

	case Right:
		w = p.x() - left;
		h = r.height();
		c = r.topLeft();
		break;

	case RightTop:
		w = p.x() - left;
		h = bottom - p.y();
		dy = h - r.height();
		c = QPoint(r.x(), r.y() - dy);
		break;

	default:
		break;
	};
	
	if (w <= 0 || h <= 0) 
		return;
	
	s = minimumSizeHint();
	if (s.width() > w)
	{
		w = s.width();
		c.setX( r.x() );
	}

	if (s.height() > h)
	{
		h = s.height();
		c.setY( r.y() );
	}

	setGeometry(c.x(), c.y(), w, h );

	emit geometryChanged( geometry() );
}

void CUiWidget::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	QRect				r;
    /*
	QPainterPath		path;
	QRegion				region;
	int					roundness = 10;
    */

	r = rect();
    /*
    path.addRoundRect(1, 1, r.width() - 2, r.height() - 2, roundness, roundness);
    p.setClipPath(path);
    region = p.clipRegion();
    setMask(region);
    */

	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiWidget::onMinimize()
{
	showMinimized();
	m_currState = Qt::WindowMinimized;
}

void CUiWidget::onRestore()
{
    for (int i = 0; i < m_borders.length(); ++i)
        m_borders[i]->setVisible(true);

	// geometry MUST be changed step by step, if we change the geometry
	// using setGeometry method, it will cause the window to maintain
	// invalid client rect size.
	resize(m_oldGeometry.width(), m_oldGeometry.height());
	move(m_oldGeometry.x(), m_oldGeometry.y());

	m_currState = Qt::WindowNoState;
	
	emit geometryChanged( geometry() );
}

void CUiWidget::onMaximize()
{
	QDesktopWidget		*w = NULL;
	QRect				r;
	static int			mark = 0;

	m_oldGeometry = geometry();
	w = QApplication::desktop();
	r = w->availableGeometry();

	for (int i = 0; i < m_borders.length(); ++i)
		m_borders[i]->setVisible(false);

	// geometry MUST be changed step by step, if we change the geometry
	// using setGeometry method, it will cause the window to maintain
	// invalid client rect size.
	resize(r.width(), r.height());
	move(0, 0);

	m_currState = Qt::WindowMaximized;

	emit geometryChanged( geometry() );
}

void CUiWidget::onMove(const QPoint &p)
{
	move( p );

	emit geometryChanged( geometry() );
}

void CUiWidget::onClose()
{
	close();
}

void CUiWidget::onFullScreen()
{
	toggleFullScreen(!isFullScreen());
	m_currState = isFullScreen() ? Qt::WindowFullScreen : m_prevState;

	emit fullscreen(isFullScreen());
}

void CUiWidget::toggleFullScreen(bool value)
{
	if (value)
	{
		// record the prev state before going into 
		// fullscreen
		m_prevState = m_currState;
		
		if (beforeEnterFullScreen())
		{
			for (int i = 0; i < m_borders.length(); ++i)
				m_borders[i]->setVisible(false);
			
			showFullScreen();
		}
	}
	else
	{
		if (m_prevState != Qt::WindowMaximized)
		{
			for (int i = 0; i < m_borders.length(); ++i)
				m_borders[i]->setVisible(true);
		}
		else
		{
			// Still maximized
		}
		showNormal();
		afterExitFullScreen();
	}
}

Qt::WindowState  CUiWidget::widgetState()
{
	return m_currState;
}

bool CUiWidget::beforeEnterFullScreen()
{
    m_titleBar->hide();
    return true;
}

bool CUiWidget::afterExitFullScreen()
{
    m_titleBar->show();
    return true;
}
