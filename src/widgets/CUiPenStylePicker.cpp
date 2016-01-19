/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiPenStylePicker.h"

CUiPenStylePicker::CUiPenStylePicker(QWidget *parent, int cols, int rows, int w, int h)
	: QMenu(parent),
	  m_columns(cols),
	  m_rows(rows),
	  m_gridWidth(w),
	  m_gridHeight(h),
	  m_space(4),
	  m_currStyleIndex(-1),
	  m_radius(4),
	  m_defaultStyle( Qt::SolidLine )
{
	setupLineStyles();
}

CUiPenStylePicker::~CUiPenStylePicker()
{
	
}

void CUiPenStylePicker::setupLineStyles()
{
	m_styles.clear();
	for (int i = Qt::NoPen; i <= Qt::CustomDashLine; ++i)
	{
		m_styles.push_back( (Qt::PenStyle) i);
	}
	m_columns = m_styles.length() < m_columns ? m_styles.length() : m_columns;
	if (m_columns == 0)
		m_columns = 1;
	m_rows = (m_styles.length() + m_columns - 1) / m_columns;
}

void CUiPenStylePicker::paintEvent( QPaintEvent *event )
{
	int			w, h;
	int			col, row;
	QRectF		r;
	QPointF		p1, p2;
	QPainter	painter(this);

	// Resize the widget to reasonable size
	resize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
	
	painter.setPen( QPen(Qt::gray, 1, Qt::SolidLine) );
	painter.setBrush( Qt::white );
	painter.drawRect( rect().adjusted(0,0,-1,-1) );

	painter.setRenderHint(QPainter::Antialiasing, false);
	for (int i = 0; i < m_styles.length(); ++i)
	{
		row = i / m_columns;
		col = i % m_columns;
		
		r.setTopLeft( QPointF(col * m_gridWidth + m_space, row * m_gridHeight + m_space) );
		r.setSize( QSizeF(m_gridWidth, m_gridHeight) );
		r.adjust(m_space,m_space,-m_space,-m_space);

		if (m_styles[i] == Qt::NoPen)
		{
			painter.setPen( QPen(Qt::black, 1) );
			painter.setFont( QFont("Arial", 10) );
			painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, tr("No Pen"));
		}
		else
		{	
			p1 = r.center() + QPointF(-r.width() / 2.0, 0);
			p2 = r.center() + QPointF( r.width() / 2.0, 0);
			painter.setPen( QPen(Qt::black, 1, m_styles[i]) );
			painter.drawLine(p1, p2);
		}
	}

	// locate the hovered color
	p1 = m_mousePos;
	if (rect().contains(p1.toPoint()))
	{
		col = qFloor((p1.x() - m_space) / m_gridWidth);
		row = qFloor((p1.y() - m_space) / m_gridHeight);
		m_currStyleIndex = row * m_columns + col;

		if (m_currStyleIndex >= 0 && m_currStyleIndex < m_styles.length())
		{
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen( Qt::NoPen );
			painter.setBrush( QColor(0,0,255, 48) );
			painter.drawRoundedRect(m_space + col * m_gridWidth, m_space + row * m_gridHeight, m_gridWidth, m_gridHeight, m_radius, m_radius );
		}
	}
	else
		m_currStyleIndex = -1;
}

void CUiPenStylePicker::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);
	if (m_currStyleIndex >= 0 && m_currStyleIndex < m_styles.length())
	{
		hide();
		m_defaultStyle = m_styles[m_currStyleIndex];
		emit styleSelected( m_defaultStyle );
	}
}

void CUiPenStylePicker::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CUiPenStylePicker::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CUiPenStylePicker::minimumSizeHint() const
{
	return QSize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}
