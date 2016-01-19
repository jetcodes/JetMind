/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramLine>

CDiagramLine::CDiagramLine(CDiagram *parent)
	: CDiagramItem(parent),
	  m_line( QLineF() )
{
	setCategory( QString::fromUtf8("Core") );
	setName( QString::fromUtf8("Line") );
}

CDiagramLine::CDiagramLine(const QPointF &p1, const QPointF &p2, CDiagram *parent)
	: CDiagramItem(parent),
	  m_line( QLineF(p1, p2) )
{
}

CDiagramLine::CDiagramLine(const QLineF &line, CDiagram *parent)
	: CDiagramItem(parent),
	  m_line(line)
{
}

CDiagramLine::CDiagramLine(qreal x1, qreal y1, qreal x2, qreal y2, CDiagram *parent)
	: CDiagramItem(parent),
	  m_line( QLineF(QPointF(x1, y1), QPointF(x2, y2)) )
{
}

QRectF CDiagramLine::boundingRect() const
{
	QRectF		rect;

	if (!m_line.isNull())
	{
		rect = shape().boundingRect();
	}
	
	return rect;
}

QPainterPath CDiagramLine::shape() const
{
	QPainterPath	path;

	if (!m_line.isNull() )
	{
		path.moveTo( m_line.p1() );
		path.lineTo( m_line.p2() );
	}
	return path;
}

void CDiagramLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(widget);
	
	if (!m_line.isNull())
	{
		painter->save();
		painter->setPen(m_pen);
		painter->drawLine(m_line);
		if (option->state & QStyle::State_Selected)
		{
			painter->strokePath(shape(), m_stroke);
		}
		painter->restore();
	}
}

void CDiagramLine::setLine(const QLineF &line)
{
	m_line = line;
}

void CDiagramLine::setLine(const QPointF &p1, const QPointF &p2)
{
	m_line.setP1(p1);
	m_line.setP2(p2);
}

void CDiagramLine::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
	m_line.setLine(x1, y1, x2, y2);
}
