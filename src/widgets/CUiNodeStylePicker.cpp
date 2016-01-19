/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CDiagramItem>
#include <core/CDiagramNode>

#include "CUiNodeStylePicker.h"


CUiNodeStylePicker::CUiNodeStylePicker(QWidget *parent)
	: QFrame(parent, Qt::Popup),
	  m_gridSize(28),
	  m_space(2),
	  m_columns(8),
	  m_rows(6),
	  m_currStyle(-1),
	  m_penWidth(2),
	  m_selfClose(true)
{
	setupStyles();
	setMouseTracking(true);

	setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);
}

CUiNodeStylePicker::~CUiNodeStylePicker()
{

}

void CUiNodeStylePicker::setupStyles()
{
	int						alpha = 192;
	QList<QColor>			colors;
	QList<Qt::PenStyle>		penStyles;
	QColor					c, basicColor(255,255,255,alpha);

	m_styles.clear();

	colors << QColor("#ff00ff") << QColor("#4E80BC") << QColor("#cc00cc") << QColor("#9ABA58") \
		<< QColor("#7F63A1") << QColor("#4AABC5") << QColor("#F69545") << QColor("#c0c0c0");
	penStyles << Qt::DashLine << Qt::DotLine << Qt::SolidLine;

	foreach (Qt::PenStyle ps, penStyles)
	{
		foreach (c, colors)
		{
			CDiagramNodeStyle	s(QPen(c, m_penWidth, ps), QBrush(Qt::white));
			m_styles.push_back(s);
		}
	}

	// colors
	// first line
	foreach (c, colors)
	{
		CDiagramNodeStyle	 s(QPen(c, m_penWidth, Qt::SolidLine), QBrush(c));
		m_styles.push_back(s);
	}

	// second line
	colors.clear();
	colors  << QColor("#FF99CC") << QColor("#993399") << QColor("#FF6666") << QColor("#339999")
		<< QColor("#CCFF00") << QColor("#66CC99") << QColor("#FFCC99") << QColor("#CC3399");
	foreach (c, colors)
	{
		CDiagramNodeStyle		s(  QPen(c, m_penWidth, Qt::SolidLine), QBrush(c) );
		m_styles.push_back(s);
	}

	// third line
	colors.clear();
	colors  << QColor("#CCCCFF") << QColor("#FFCCCC") << QColor("#99CCCC") << QColor("#99CC99")
		<< QColor("#CCFF99") << QColor("#FF9966") << QColor("#99CC99") << QColor("#FFFFCC");
	foreach (c, colors)
	{
		CDiagramNodeStyle		s(QPen(c, m_penWidth, Qt::SolidLine), QBrush(c));
		m_styles.push_back(s);
	}
}

void CUiNodeStylePicker::paintEvent( QPaintEvent *event )
{
	int			i, j, row, col;
	qreal		step, radius = 3;
	QRectF		r;	
	QPointF		p;
	QPainter	painter(this);

	step = m_gridSize;
	
	// Resize the widget to reasonable size
	resize(m_columns * step, m_rows * step );

	// painter.setPen( QPen(Qt::gray, 1.5) );
	painter.setPen( Qt::NoPen );
	painter.setBrush( Qt::white );
	painter.drawRect( rect() );
	painter.setRenderHint( QPainter::Antialiasing, true);

	for (i = 0; i < m_styles.length(); ++i)
	{
		CDiagramNodeStyle	s = m_styles[i];
		
		row = i / m_columns;
		col = i % m_columns;
		if (row < 0 && row >= m_rows && col < 0 && col >= m_columns)
			break;
		r.setTopLeft( QPointF(col * step, row * step) );
		r.setSize( QSizeF(m_gridSize, m_gridSize) );
		r.adjust(m_space,m_space,-m_space,-m_space);
		painter.setPen( s.pen() );
		painter.setBrush( s.brush() );
		painter.drawRoundedRect(r, radius, radius);
	}

	// locate the hovered color
	p = m_mousePos;
	if (rect().contains(p.toPoint()))
	{
		col = qFloor(p.x() / step);
		row = qFloor(p.y() / step);
		m_currStyle = row * m_columns + col;

		if (m_currStyle >= 0 && m_currStyle < m_styles.length())
		{
			painter.setPen( Qt::NoPen );
			painter.setBrush( QColor(0,0,255, 128) );
			painter.drawRoundedRect( col * step, row * step, m_gridSize, m_gridSize, 5, 5 );
		}
	}
	else
		m_currStyle = -1;
}

void CUiNodeStylePicker::mouseDoubleClickEvent( QMouseEvent *event )
{
	if (m_currStyle >= 0 && m_currStyle < m_styles.length())
	{
		emit styleSelected( m_styles[m_currStyle] );
		
		if (isSelfClose())
			close();
	}

	m_currStyle = -1;
	repaint();
	QWidget::mouseDoubleClickEvent(event);
}										

void CUiNodeStylePicker::mousePressEvent( QMouseEvent *event )
{
	if (m_currStyle >= 0 && m_currStyle < m_styles.length())
	{
		emit styleSelected( m_styles[m_currStyle] );
		
		if (isSelfClose())
			close();
	}
	QWidget::mousePressEvent(event);
}

void CUiNodeStylePicker::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();

	if (m_currStyle >= 0 && m_currStyle < m_styles.length())
	{
		emit styleHovered( m_styles[m_currStyle] );
	}

	QWidget::mouseMoveEvent(event);
}

void CUiNodeStylePicker::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CUiNodeStylePicker::minimumSizeHint() const
{
	return QSize(m_columns * m_gridSize, m_rows * m_gridSize );
}

void CUiNodeStylePicker::setSelfClose(bool value)
{
	m_selfClose = value;
}

bool CUiNodeStylePicker::isSelfClose()
{
	return m_selfClose;
}

void CUiNodeStylePicker::setRows(int num)
{
	if (num > 0)
		m_rows = num;
}

int CUiNodeStylePicker::rows()
{
	return m_rows;
}

void CUiNodeStylePicker::setColumns(int num)
{
	if (num > 0)
		m_columns = num;
}

int CUiNodeStylePicker::columns()
{
	return m_columns;
}

void CUiNodeStylePicker::addStyle(const CDiagramNodeStyle &style)
{
	m_styles.push_back(style);
}

void CUiNodeStylePicker::setStyles(const QList<CDiagramNodeStyle> &styles)
{
	m_styles = styles;
	update();
}

QList<CDiagramNodeStyle> CUiNodeStylePicker::styles()
{
	return m_styles;
}

void CUiNodeStylePicker::setPenWidth(qreal value)
{
	if (value <= 0) value = 1.0;
	m_penWidth = value;

	setupStyles();
}

qreal CUiNodeStylePicker::penWidth()
{
	return m_penWidth;
}