/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramPort>

CDiagramPort::CDiagramPort(CDiagramNode *parent, int side, int index)
	: QGraphicsItem(parent), 
	  m_side(side),
	  m_index(index),
	  m_width(8),
	  m_height(8),
	  m_hover(false),
	  m_pen( QPen( QColor(0, 0, 0), 2 ) ),
	  m_stroke( QPen( QColor(255, 255, 255, 128), 4 ) ),
	  m_brush( QBrush( QColor(161, 173, 215) ) )
{
	setAcceptHoverEvents(true);
}

CDiagramPort::~CDiagramPort()
{

}

QRectF CDiagramPort::boundingRect() const
{
	return QRectF(-m_width/2.0, -m_height/2.0, m_width, m_height);
}

QPainterPath CDiagramPort::shape() const
{
	QPainterPath		path;

	path.addRect( boundingRect() );
	return path;
}

void CDiagramPort::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = 0 */)
{
	qreal				hw, hh;
	QPainterPath		path;

	Q_UNUSED(option);
	Q_UNUSED(widget);

	hw = m_width / 2.0;
	hh = m_height / 2.0;

	path.moveTo(0, -hh);
	path.lineTo(0, hh);
	path.moveTo(-hw, 0);
	path.lineTo(hw, 0);

	painter->save();	
	painter->setPen( m_pen );
	painter->drawPath(path);
	if (!m_hover) {
		painter->strokePath(path, m_stroke );
	}
	painter->restore();
}

QVariant CDiagramPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change)
	{
	case ItemPositionChange:
		break;

	default:
		break;
	};
	return QGraphicsItem::itemChange(change, value);
}

void CDiagramPort::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	m_hover = true;
	update();
}

void CDiagramPort::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	m_hover = false;
	update();
}

void CDiagramPort::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (isVisible()) {
		setSelected(true);
		update();
	}
}


int CDiagramPort::addEdge(CDiagramEdge *c)
{
	if (c && !m_edges.contains(c))
	{
		m_edges.insert(c, 0.5);
		return 0;
	}
	return -1;
}

int CDiagramPort::removeEdge(CDiagramEdge* c)
{
	if (!c || !m_edges.contains(c)) 
		return 0;
	m_edges.remove(c);
	return 0;
}

QList<CDiagramEdge*> CDiagramPort::getEdges()
{
	return m_edges.keys();
}

void CDiagramPort::retrack()
{
	QList<CDiagramEdge*>	list;

	list = m_edges.keys();
	for (int i = 0; i < list.length(); ++i)
	{
		list.at(i)->retrack();
	}
}

void CDiagramPort::setConnectPosition(CDiagramEdge *edge, qreal value)
{
	if (edge && m_edges.contains(edge))
	{
		m_edges[edge] = value;
		edge->retrack();
	}
}

qreal CDiagramPort::getConnectPosition(CDiagramEdge *edge) const
{
	qreal	r = 0.0;

	if (edge && m_edges.contains(edge))
	{	
		r = m_edges[edge];
	}
	return r;
}

int CDiagramPort::getConnectPosition(CDiagramEdge *edge, QPointF& out) const
{
	int					side = m_side;
	bool				mark = false;
	qreal				angle = 0;
	qreal				percent = 0.5, prev = 0, next = 1.0;
	QLineF				line, radius;
	QPointF				p1, p2;
	QRectF				r;
	CDiagramNode		*item = NULL;
	QPainterPath		path;

	if (!edge || !m_edges.contains(edge)) 
		return -1;

	item = dynamic_cast<CDiagramNode*>(QGraphicsItem::parentItem());

	// find out the side
	p1 = mapToScene(0.0, 0.0);
	if (item)
	{
		p2 = item->mapToScene( QPointF(0,0) );
		line.setP1(p2);
		line.setP2(p1);
		angle = line.angle();
		angle = angle > 180 ? angle - 360: angle;
		side = int(qAbs(angle / 45));
		if (side == 0) side = Global::East;
		else if (side > 0 && side < 3 && angle > 0) side = Global::North;
		else if (side > 2) side = Global::West;
		else side = Global::South;
	}
	
	// find out the correct connection point
	percent = m_edges[edge];
	r = item->boundingRect();

	// the direction is clockwise
	switch (m_side)
	{
	case Global::North:
		line = QLineF(r.topLeft(), r.topRight());
		radius = QLineF(r.center(), line.pointAt( percent ));
		break;

	case Global::South:
		line = QLineF(r.bottomRight(), r.bottomLeft());
		radius = QLineF(r.center(), line.pointAt(percent));
		break;

	case Global::West:
		line = QLineF(r.bottomLeft(), r.topLeft());
		radius = QLineF(r.center(), line.pointAt(percent));
		break;

	case Global::East:
		line = QLineF(r.topRight(), r.bottomRight());
		radius = QLineF(r.center(), line.pointAt(percent));
		break;

	default:
		break;
	};
	
	// divide to find the closed connect point
	path = item->shape();
	p2 = radius.p1();
	percent = 0.5;
	while (true)
	{
		p1 = radius.pointAt(percent);
		if (path.contains(p1))
		{
			prev = percent;
			percent = (next + percent) / 2.0;
		}
		else
		{
			next = percent;
			percent = (prev + percent) / 2.0;
		}
		if ( (p2 - p1).manhattanLength() < 0.5)
		{
			p2 = p1;
			break;
		}
		p2 = p1;
	}
	p2 = item->mapToScene(p2);
	out.setX( p2.x());
	out.setY( p2.y());

	return side;
}

qreal CDiagramPort::getPercent(const QPointF &p)
{
	qreal			v = -1.0, t, d;
	QRectF			r;
	QLineF			line, radius;
	QLineF			l1, l2;
	QPointF			mark, tmp;
	CDiagramNode	*node = NULL;

	node = getParentNode();
	if (!node) return 0;

	r = node->boundingRect();

	tmp = node->mapFromScene(p);
	radius = QLineF(r.center(), tmp);
	switch (m_side)
	{
	case Global::North:
		line = QLineF(r.topLeft(), r.topRight());
		break;

	case Global::South:
		line = QLineF(r.bottomRight(), r.bottomLeft());
		break;

	case Global::West:
		line = QLineF(r.bottomLeft(), r.topLeft());
		break;

	case Global::East:
		line = QLineF(r.topRight(), r.bottomRight());
		break;

	default:
		break;
	};

	l1 = QLineF(tmp, line.p1());
	l2 = QLineF(tmp, line.p2());
	t = l1.angleTo(l2);
	d = qAbs(l1.length() * l2.length() * sin(t)) / line.length();
	if (radius.length() < QLineF(r.center(), r.topLeft()).length() || d < 24) 
	{
		line.intersect(radius, &mark);
		v = qMin(1.0, QLineF(mark, line.p1()).length() / line.length());
	}
	return v;
}

CDiagramNode* CDiagramPort::getParentNode()
{
	CDiagramNode	*item = NULL;

	if (QGraphicsItem::parentItem())
	{
		item = dynamic_cast<CDiagramNode*>(QGraphicsItem::parentItem());
	}
	return item;
}