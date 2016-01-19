/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiPenWeightPicker.h"

CUiPenWeightPicker::CUiPenWeightPicker(QWidget *parent, int cols, int rows, int w, int h)
	: QMenu(parent),
	  m_columns(cols),
	  m_rows(rows),
	  m_gridWidth(w),
	  m_gridHeight(h),
	  m_space(6),
	  m_currWidthIndex(-1),
	  m_radius(4),
	  m_defaultWidth( 1 )
{
	setupLineWeights();
}

CUiPenWeightPicker::~CUiPenWeightPicker()
{

}

void CUiPenWeightPicker::setupLineWeights()
{
	m_widths.clear();
	for (int i = 1; i < 9; ++i)
	{
		m_widths.push_back(i);
	}
	m_rows = (m_widths.length() + m_columns - 1) / m_columns;
}


void CUiPenWeightPicker::paintEvent( QPaintEvent *event )
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
	for (int i = 0; i < m_widths.length(); ++i)
	{
		row = i / m_columns;
		col = i % m_columns;
		
		r.setTopLeft( QPointF(col * m_gridWidth + m_space, row * m_gridHeight + m_space) );
		r.setSize( QSizeF(m_gridWidth, m_gridHeight) );
		r.adjust(m_space,m_space,-m_space,-m_space);

		painter.setPen( QPen(Qt::black, m_widths[i], Qt::SolidLine) );
		p1 = r.center() + QPointF(-r.width() / 2.0, 0);
		p2 = r.center() + QPointF( r.width() / 2.0, 0);
		painter.drawLine(p1, p2);
	}

	// locate the hovered color
	p1 = m_mousePos;
	if (rect().contains(p1.toPoint()))
	{
		col = qFloor((p1.x() - m_space) / m_gridWidth);
		row = qFloor((p1.y() - m_space) / m_gridHeight);
		m_currWidthIndex = row * m_columns + col;

		if (m_currWidthIndex >= 0 && m_currWidthIndex < m_widths.length())
		{
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen( Qt::NoPen );
			painter.setBrush( QColor(0,0,255, 48) );
			painter.drawRoundedRect(m_space + col * m_gridWidth, m_space + row * m_gridHeight, m_gridWidth, m_gridHeight, m_radius, m_radius );
		}
	}
	else
		m_currWidthIndex = -1;
}

void CUiPenWeightPicker::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);
	if (m_currWidthIndex >= 0 && m_currWidthIndex < m_widths.length())
	{
		hide();
		m_defaultWidth = m_widths[m_currWidthIndex];
		emit weightSelected( m_defaultWidth );
	}
}

void CUiPenWeightPicker::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CUiPenWeightPicker::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CUiPenWeightPicker::minimumSizeHint() const
{
	return QSize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}
