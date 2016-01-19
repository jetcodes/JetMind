/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiColorGrid.h"

CUiColorGrid::CUiColorGrid(QWidget *parent)
	: QWidget(parent),
	  m_gridSize(10),
	  m_space(1),
	  m_columns(20),
	  m_rows(12),
	  m_currColor(-1)
{	
	setupColors();
	setMouseTracking(true);	
}

CUiColorGrid::~CUiColorGrid()
{
}

void CUiColorGrid::setupColors()
{
	int				i, r, g, b;

	QStringList		colors;
	
	colors << "#000000" << "#333333" << "#666666" << "#999999"\
		<< "#CCCCCC" << "#FFFFFF" << "#FF0000" << "#00FF00"\
		<< "#0000FF" << "#FFFF00" << "#00FFFF" << "#FF00FF";

	for (i = 0; i < colors.size(); ++i) m_colors.push_back( QColor(colors[i]) );
	for (i = 0; i < colors.size(); ++i) m_colors.push_back( QColor(0,0,0) );

	for (r = 0; r <= 255; r += 51)
	{
		for (g = 0; g <= 255; g += 51 )
		{
			for (b = 0; b <= 255; b+= 51)
			{
				m_colors.push_back( QColor(r, g, b) );
			}
		}
	}
}

void CUiColorGrid::paintEvent(QPaintEvent *event)
{
	int			i, j, row, col;
	qreal		step;
	QRectF		r;	
	QPainter	painter(this);

	step = m_gridSize + m_space;
	
	// Resize the widget to reasonable size
	resize(m_columns * step, m_rows * step );

	// draw the left side of the color picker
	for (i = 0; i < 2 * m_rows; ++i)
	{
		col = i / m_rows;
		row = i % m_rows;
		r.setTopLeft( QPointF(col * step, row * step) );
		r.setSize( QSizeF(m_gridSize, m_gridSize) );
		painter.setPen( Qt::white );
		painter.fillRect(r, QBrush( m_colors[i] ) );
	}

	// draw the right side of the color picker
	for (i = 2 * m_rows; i < m_colors.length(); ++i)
	{
		j = i - 2 * m_rows;
		col = 2 + ((j / 36) % 3) * 6 + (j % 36) % 6;
		row = ((j / 36) / 3) * 6 + (j % 36) / 6;
		r.setTopLeft( QPointF(col * step, row * step ) );
		r.setSize( QSizeF(m_gridSize, m_gridSize) );
		painter.setPen( Qt::white );
		painter.fillRect(r, QBrush( m_colors[i] ) );
	}

	if (m_currColor != -1)
	{
		painter.setPen( Qt::blue );
		painter.drawRect( m_currColumn * step, m_currRow * step, m_gridSize, m_gridSize );			
	}
}

void CUiColorGrid::mousePressEvent(QMouseEvent *event)
{
	if (m_currColor >= 0 && m_currColor < m_colors.length())
	{
		emit colorSelected( m_colors[m_currColor] );
	}
	QWidget::mousePressEvent(event);
}

void CUiColorGrid::mouseMoveEvent(QMouseEvent *event)
{
	qreal		step;
	int			col, row;
	QPointF		p;

	step = m_gridSize + m_space;

	p = event->pos();
	if (rect().contains(p.toPoint()))
	{
		col = qFloor(p.x() / step);
		row = qFloor(p.y() / step);
		if (col < 2)
			m_currColor = col * 12 + row;
		else
			m_currColor = 24 + (row / 6) * 108 + ((col - 2) / 6) * 36 + ((row % 6) * 6 + (col - 2) % 6 ); 

		if (m_currColor >= 0 && m_currColor < m_colors.length())
		{	
			emit colorHovered( m_colors[m_currColor] );
		}
		else
		{
			m_currColor = -1;
		}
		m_currColumn = col;
		m_currRow = row;
	}
	else
		m_currColor = -1;
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CUiColorGrid::mouseReleaseEvent(QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

void CUiColorGrid::mouseDoubleClickEvent ( QMouseEvent * event )
{
	QWidget::mouseDoubleClickEvent(event);
	emit colorSelectionDone();
}

QSize CUiColorGrid::minimumSizeHint() const
{
	qreal	step;

	step = m_gridSize + m_space;
	return QSize(m_columns * step, m_rows * step);
}
