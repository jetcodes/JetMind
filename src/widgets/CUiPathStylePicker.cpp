/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>

#include "CUiPathStylePicker.h"

CUiPathStylePicker::CUiPathStylePicker(QWidget *parent, int cols, int rows, int w, int h)
	: QMenu(parent),
	  m_columns(cols),
	  m_rows(rows),
	  m_gridWidth(w),
	  m_gridHeight(h),
	  m_space(4),
	  m_currStyleIndex(-1),
	  m_radius(4),
	  m_defaultStyle(NULL)
{
	m_styles.clear();
    setupStyles();
}

CUiPathStylePicker::~CUiPathStylePicker()
{

}

void CUiPathStylePicker::clearAll()
{
	m_styles.clear();
}

void CUiPathStylePicker::addStyle(CDiagramEdgeStyle *r)
{

	if (!r || m_styles.contains(r))
		return;
	m_styles.push_back(r);

}

void CUiPathStylePicker::addStyleCategory(CDiagramEdgeStyleCategory *ctg)
{
	if (!ctg) 
		return;

	foreach (CDiagramEdgeStyle *r, ctg->styles())
	{
		addStyle(r);
    }

    // calculate the exact value of columns and rows
    // resize the widget to reasonable size
    m_columns = m_styles.length() < m_columns ? m_styles.length() : m_columns;
    if (m_columns == 0)
        m_columns = 1;
}

CDiagramEdgeStyle* CUiPathStylePicker::defaultStyle()
{
	CDiagramEdgeStyle		*r = NULL;

	if (m_defaultStyle) 
	{
		r = m_defaultStyle;
	}
	else
	{
		if (m_styles.count() > 4)
			r = m_styles[4];
		else if (m_styles.count() > 0)
			r = m_styles[0];
		m_defaultStyle = r;
	}
	return r;
}

void CUiPathStylePicker::setDefaultStyle(CDiagramEdgeStyle *style)
{
	if (style)
		m_defaultStyle = style;
}

void CUiPathStylePicker::setupStyles()
{
	CDiagramEdgeStyleDatabase		*db = NULL;
	CDiagramEdgeStyleCategory		*ctg = NULL;

	db = CDiagramEdgeStyleDatabase::instance();
	if (!db)
		return;

	ctg = db->getCategory( Global::CTG_COREITEMS );
	if (!ctg)
		return;

    addStyleCategory(ctg);
}

void CUiPathStylePicker::paintEvent( QPaintEvent *event )
{
    int					w, h;
	int					col, row;
	QRectF				r;
	QPointF				p;
	QPainter			painter(this);
    QStyleOption		opt;

	m_rows = (m_styles.length() + m_columns - 1) / m_columns;
    resize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );

    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	for (int i = 0; i < m_styles.length(); ++i)
	{
		row = i / m_columns;
		col = i % m_columns;
		
		r.setTopLeft( QPointF(col * m_gridWidth + m_space, row * m_gridHeight + m_space) );
		r.setSize( QSizeF(m_gridWidth, m_gridHeight) );
		r.adjust(m_space,m_space,-m_space,-m_space);

		Utils::drawEdgeStyleHelper(&painter, m_styles.at(i), r, 0);
	}

	// locate the hovered color
	p = m_mousePos;
	if (rect().contains(p.toPoint()))
	{
		col = qFloor((p.x() - m_space) / m_gridWidth);
		row = qFloor((p.y() - m_space) / m_gridHeight);
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

void CUiPathStylePicker::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);
	if (m_currStyleIndex >= 0 && m_currStyleIndex < m_styles.length())
	{
		hide();
		m_defaultStyle = m_styles[m_currStyleIndex];
		emit styleSelected( m_defaultStyle );
	}
}

void CUiPathStylePicker::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CUiPathStylePicker::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CUiPathStylePicker::minimumSizeHint() const
{
	return QSize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}
