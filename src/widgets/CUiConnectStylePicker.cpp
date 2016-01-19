/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramArrow>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramEdge>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramIconProvider>

#include "CUiConnectStylePicker.h"

CUiConnectStylePicker::CUiConnectStylePicker(QWidget *parent)
	: QMenu(parent),
	  m_gridSize(32),
	  m_space(5),
	  m_columns(5),
	  m_rows(6),
	  m_currType(-1),
	  m_arrowMode(EndArrow),
	  m_defaultType(NULL)
{
	setupTypes();
	setMouseTracking(true);
}

CUiConnectStylePicker::~CUiConnectStylePicker()
{

}

void CUiConnectStylePicker::setupTypes()
{
    CUiConnectStyle             *edgeType = NULL;
	QList<CDiagramArrow*>		arrows;
	CDiagramArrowCategory		*ctg = NULL;
	CDiagramEdgeStyle			*style = NULL;

	ctg = CDiagramArrowDatabase::instance()->getCategory( Global::CTG_COREITEMS );
	if (!ctg) return;

	style = CDiagramEdgeStyleDatabase::instance()->getDefaultRouter();
	arrows = ctg->arrows();

	for (int i = 0; i < arrows.length(); ++i)
	{
		edgeType = new CUiConnectStyle;
		edgeType->startArrow = arrows.at(i);
		edgeType->endArrow = arrows.at(i);
		edgeType->penStyle = Qt::SolidLine;
		m_types.push_back(edgeType);
	}

	m_defaultType = m_types.length() > 0 ? m_types.first() : NULL;
	m_rows = (m_types.length() + m_columns - 1) / m_columns;

    // Resize the widget to reasonable size
    resize(m_columns * m_gridSize + 2 * m_space, m_rows * m_gridSize + 2 * m_space );
}

void CUiConnectStylePicker::setArrowMode(int mode)
{
	if (mode >= 0 && mode <= BothArrows)
		m_arrowMode = mode;
}

int CUiConnectStylePicker::arrowMode() const
{
	return m_arrowMode;
}

QSize CUiConnectStylePicker::sizeHint() const
{
	return QSize(geometry().size());
}

void CUiConnectStylePicker::paintEvent( QPaintEvent *event )
{
    int             i, row, col;
    qreal           step, radius = 5;
    QRectF          r;
    QPointF         p;
    QPainter        painter(this);
    QStyleOption	opt;

    // draw background
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	step = m_gridSize;

	for (i = 0; i < m_types.length(); ++i)
	{
		CUiConnectStyle		*edgeType = NULL;
		QPainterPath		path;
        QPointF				p1, p2;
		
		row = i / m_columns;
		col = i % m_columns;
		r.setTopLeft( QPointF(col * step + m_space, row * step + m_space) );
		r.setSize( QSizeF(m_gridSize, m_gridSize) );
		r.adjust(m_space,m_space,-m_space,-m_space);
		
		p1 = r.center() + QPointF(-r.width() / 2.0, 0);
		p2 = r.center() + QPointF( r.width() / 2.0, 0);

		painter.setRenderHint(QPainter::Antialiasing, true);
		edgeType = m_types.at(i);
		switch( m_arrowMode )
		{
		case StartArrow:
			p1 = edgeType->startArrow->draw(&painter, p2, p1, path);
			edgeType->icon = CDiagramIconProvider::instance()->getIconForStartArrow(edgeType->startArrow);
			break;

		case EndArrow:
			p2 = edgeType->endArrow->draw(&painter, p1, p2, path);
			edgeType->icon = CDiagramIconProvider::instance()->getIconForEndArrow(edgeType->endArrow);
			break;

		default:
			break;
		}; // End of switch

		painter.setRenderHint(QPainter::Antialiasing, false);
		painter.setPen( QPen(Qt::black, 1, edgeType->penStyle) );
		painter.drawLine(p1, p2);
	}

	// locate the hovered color
	p = m_mousePos;
	if (rect().contains(p.toPoint()))
	{
		col = qFloor((p.x() - m_space) / step);
		row = qFloor((p.y() - m_space) / step);
		m_currType = row * m_columns + col;

		if (m_currType >= 0 && m_currType < m_types.length())
		{
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen( Qt::NoPen );
			painter.setBrush( QColor(0,0,255, 48) );
			painter.drawRoundedRect(m_space + col * step, m_space + row * step, m_gridSize, m_gridSize, radius, radius );
		}
	}
	else
		m_currType = -1;
}

void CUiConnectStylePicker::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);
	if (m_currType >= 0 && m_currType < m_types.length())
	{
		hide();
		m_defaultType = m_types[m_currType];
		emit typeSelected( m_defaultType );
	}
}

void CUiConnectStylePicker::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CUiConnectStylePicker::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

CUiConnectStyle* CUiConnectStylePicker::defaultEdgeType()
{
	return m_defaultType;
}
