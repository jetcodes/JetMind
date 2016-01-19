/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramPort>

CDiagramNodeStyle::CDiagramNodeStyle()
{
}

CDiagramNodeStyle::CDiagramNodeStyle(const QPen &pen, const QBrush &brush)
	: m_pen(pen),
	  m_brush(brush)
{

}

bool CDiagramNodeStyle::isGradient()
{
	Qt::BrushStyle	s = m_brush.style();

	return (s == Qt::LinearGradientPattern || s == Qt::ConicalGradientPattern || s == Qt::RadialGradientPattern);
}

void CDiagramNodeStyle::apply(CDiagramNode *item)
{
	// TO BE OVERRIDED IN SUBCLASSES
}

bool CDiagramNodeStyle::operator==(const CDiagramNodeStyle &style) const
{
	return (style.m_pen == m_pen) && (style.m_brush == m_brush);
}

bool CDiagramNodeStyle::operator!=(const CDiagramNodeStyle &style) const
{
	return (style.m_pen != m_pen) || (style.m_brush != m_brush);
}


CDiagramNode::CDiagramNode(CDiagram* parent)
	: CDiagramItem(parent),
	  m_mode(CDiagramNode::Normal),
	  m_connectable(true),
	  m_padding(5),
	  m_labelPosition(CDiagramNode::Center)
{	
	QRectF		r;
	qreal		w;
	qreal		h;

	setAcceptHoverEvents(true);

	// MUST BE CALLED BEFORE THE THE INITIALIZATION
	// OF CONNECTORS
	resize(48, 48);
	
	m_rotatable = false;

	initPorts();
	layoutPorts();
	
	//connect(m_label, SIGNAL(resizeRequest(QGraphicsLayoutItem*)), this, SLOT(onChildResizeRequest(QGraphicsLayoutItem*)));
	connect(m_label, SIGNAL(sizeAutoChanged()), this, SLOT(layoutContent()));
	connect(this, SIGNAL(geometryChanged()), this, SLOT(onGeometryChanged()));
}

CDiagramNode::~CDiagramNode()
{
	foreach(CDiagramPort *p, m_ports)
	{
		SAFE_DELETE(p);
	}
}

QRectF CDiagramNode::boundingRect() const
{
	qreal		hs, hw, hh;
	qreal		pw;
	QRectF		r;

	pw = m_pen.widthF() + m_stroke.widthF();
	hw = width() / 2.0;
	hh = height() / 2.0;
	hs = pw / 2.0;

	r = QRectF(-(hw + hs), -(hh + hs), width() + pw, height() + pw);
	//if (m_label && m_labelPosition != Center)
	//	r = r.united( m_label->mapToParent( m_label->boundingRect() ).boundingRect() );
	return r;
}

QPainterPath CDiagramNode::shape() const
{
	QPainterPath	r;
	QPainterPath	path;

	if (m_label && !m_label->toPlainText().trimmed().isEmpty())
		r = mapFromScene( m_label->mapToScene(m_label->shape()) );

	path.setFillRule(Qt::WindingFill);
	path.addRect(-width()/2.0, -height()/2.0, width(), height());
	path.addPath( r );
	return path;
}

void CDiagramNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	Q_UNUSED(painter);
}

void CDiagramNode::onGeometryChanged()
{
	layoutPorts();
	layoutContent();
	retrack();
}

void CDiagramNode::onChildResizeRequest(QGraphicsLayoutItem *source)
{
	qreal					w, h;
	QSizeF					s;
	QRectF					r1, r2;
	QGraphicsItem			*item = NULL;
	QGraphicsLayout			*l = NULL;
	QGraphicsLayoutItem		*layoutItem = NULL;
	QList<QGraphicsItem*>	items;
	
	l = layout();
	if (l)
	{
		l->invalidate();

		// The geometry has to be updated, or the child resize
		// can't affect the item's size
		s = l->effectiveSizeHint(Qt::PreferredSize);
		w = qMax(width(), s.width());
		h = qMax(height(), s.height());
		setGeometry(QRectF(pos() + QPointF(w - width(), h - height()) / 2.0, QSizeF(w, h)));
	}
	else
	{
		if (scene())
		{
			items = childItems();
			for (int i = 0; i < items.length(); ++i)
			{
				item = items.at(i);
				layoutItem = dynamic_cast<QGraphicsLayoutItem*>(item);
				if (layoutItem)
				{
					r1 = item->mapRectToParent( QRectF( QPointF(0,0), layoutItem->preferredSize()) );
					r2 = r2.united(r1);
				}
			}
			s = r2.size();
			w = qMax(width(), s.width() + 2 * m_padding);
			h = qMax(height(), s.height() + 2 * m_padding);
			setGeometry(QRectF(pos() + QPointF(w - width(), h - height()) / 2.0, QSizeF(w, h)));
		}
	}
}

void CDiagramNode::layoutPorts()
{
	qreal		hw, hh;
	qreal		x[4] = {0, -1, 0, 1};
	qreal		y[4] = {-1, 0, 1, 0};

	hw = width() / 2.0;
	hh = height() / 2.0;

	if (m_ports.length() < Global::East ) return;

	// Update the position of the connectors
	// They should be the same with four controllers
	for (int i=Global::North; i<=Global::East; ++i)
	{
		m_ports[i]->setPos( x[i] * hw, y[i] * hh );
	}
	update();
}

void CDiagramNode::initPorts()
{
	CDiagramPort	*conn = NULL;

	for (int i=Global::North; i<= Global::East; ++i)
	{
		conn = new CDiagramPort(this, i, i);
		conn->setVisible(false);
		conn->setParentItem(this);
		m_ports.append(conn);
	}
}

QList<CDiagramPort*> CDiagramNode::getPorts() const
{
	if (connectable())
		return m_ports;
	else
		return QList<CDiagramPort*>();
}

QList<CDiagramPort*> CDiagramNode::getPortsBySide(int side) const
{
	CDiagramPort					*c;
	QList<CDiagramPort*>			list;

	// side is Global::North,Global::East, etc.
	if (connectable())
	{
		foreach (c, m_ports)
		{
			if (c->side() == side)
			{
				list.append(c);
			}
		}
	}
	return list;
}

CDiagramPort* CDiagramNode::getPortByIndex(int index) const
{
	CDiagramPort *c = NULL;

	if (connectable())
	{
		if (0 <= index && index < m_ports.length())
			c = m_ports[index];
	}
	return c;
}

CDiagramPort* CDiagramNode::getClosestPort(CDiagramNode *node) const
{
	qreal				w, h, l, m = 100000.0;
	QLineF				line, edges[4];
	QPointF				p, r;
	CDiagramPort		*c1 = NULL, *c2 = NULL;
	int					t, i;

	if (!node || !connectable()) 
		return NULL;

	w = width() / 2.0;
	h = height() / 2.0;
	p = mapFromItem(node, node->boundingRect().center());	

	line = QLineF(0, 0, p.x(), p.y());
	edges[0].setLine(-w, -h, w, -h);	// North
	edges[1].setLine(-w, -h, -w, h);	// West
	edges[2].setLine(-w, h, w, h);		// South
	edges[3].setLine(w, -h, w, h);		// East

	// Get the intersect edge
	for (i=Global::North; i<=Global::East; ++i)
	{
		t = line.intersect(edges[i], &r);
		if (QLineF::BoundedIntersection == t)
		{
			break;
		}
	}

	// Get the closed connector
	foreach (c1, getPortsBySide(i))
	{
		line.setLine(c1->pos().x(), c1->pos().y(), r.x(), r.y());
		l = line.length();
		if (l < m)
		{
			m = l;
			c2 = c1;
		}
	}
	return c2;
}

void CDiagramNode::showPorts(bool show)
{
	if (!m_connectable) return;

	for (int i=0; i<m_ports.length(); ++i)
	{
		m_ports[i]->setVisible( show );
	}
}

void CDiagramNode::setMode(CDiagramNode::Mode mode)
{
	m_mode = mode;
	switch (mode)
	{
	default:
	case Normal:		
		showPorts(false);
		break;

	case Linking:
		showPorts(true);
		break;
	};
}

void CDiagramNode::setConnectable(bool value)
{
	m_connectable = value;
}

bool CDiagramNode::connectable() const
{
	 return m_connectable;
}

void CDiagramNode::setLabelPosition(int value)
{
	if (value < 0 ||value >= CDiagramNode::InvalidLabelPosition)
		return;

	m_labelPosition = value;
	if (m_labelPosition == Center)
		m_label->setAutoWidth(false);
	else
		m_label->setAutoWidth(true);
	update();
}

int CDiagramNode::labelPosition()
{
	return m_labelPosition;
}

bool CDiagramNode::supportLabelPlacement()
{
	return true;
}

void CDiagramNode::retrack()
{
	foreach (CDiagramPort *c, m_ports)
	{
		c->retrack();
	}
}

void CDiagramNode::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	event->accept();
}

void CDiagramNode::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	event->accept();
}

void CDiagramNode::layoutContent()
{
	qreal			delta = 0;
	qreal			w = width() / 2.0;
	qreal			h = height() / 2.0;
	QRectF			r;
	QGraphicsLayout	*l = NULL;
	
	l = layout();
	if (l)
	{
		l->invalidate();
	}
	else if (m_label)
	{
		r = m_label->boundingRect();
		switch (labelPosition())
		{
		default:
		case Center:
			//if (!m_label->autoWidth())
			m_label->resize(width() - 2 * m_padding, height() - 2 * m_padding);
			r = m_label->boundingRect();
			m_label->setPos( -w + m_padding, -r.height() / 2.0);
			break;

		case Above:
			if (!m_label->autoWidth())
				m_label->resize(width(), m_label->geometry().height());
			r = m_label->boundingRect();
			m_label->setPos(-r.width() / 2.0, - h - r.height() - delta);
			break;

		case Below:
			if (!m_label->autoWidth())
				m_label->resize(width(), m_label->geometry().height());
			r = m_label->boundingRect();
			m_label->setPos(-r.width() / 2.0, h + delta);
			break;

		case Left:
			if (!m_label->autoWidth())
				m_label->resize(m_label->geometry().width(), height());
			r = m_label->boundingRect();
			m_label->setPos(-w - r.width() - delta, -r.height() / 2.0);
			break;

		case Right:
			if (!m_label->autoWidth())
				m_label->resize(m_label->geometry().width(), height());
			r = m_label->boundingRect();
			m_label->setPos(w + delta, -r.height() / 2.0);
			break;
		}
	}
}

QSet<CDiagramEdge*> CDiagramNode::getEdges()
{
	QSet<CDiagramEdge*>	edges;

	foreach (CDiagramPort *c, m_ports)
	{
		edges.unite( QSet<CDiagramEdge*>::fromList(c->getEdges()) );
	}
	return edges;
}

void CDiagramNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
}

void CDiagramNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	bool					accepted = false;
	qreal					dist = 0.0, min = 0.0;
	QRectF					rect;
	QGraphicsItem			*item = NULL, *target = NULL;
	QGraphicsLayoutItem		*layout = NULL;
	QList<QGraphicsItem*>	items;

	if (handlesChildEvents())
	{
		QGraphicsItem::mouseDoubleClickEvent(event);
		return;
	}

	items = childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (!item->isVisible()) continue;
		layout = dynamic_cast<QGraphicsLayoutItem*>(item);
		if (layout)
		{
			rect = item->mapRectToScene( item->boundingRect() );
			if (rect.contains(event->scenePos()))
			{
				scene()->sendEvent(item, event);
				event->accept();
				accepted = true;
				break;
			}
			else
			{
				dist = QLineF(rect.center(), event->scenePos()).length();
				if (i == 0 || (dist < min))
				{
					min = dist;
					target = item;
				}
			}
		}
	}
	
	if (!accepted)
	{
		if (target)
		{
			scene()->sendEvent(target, event);
			event->accept();
		}
		else if (m_label && m_label->isVisible())
		{
			scene()->sendEvent(m_label, event);
			event->accept();
		}
	}
}

void CDiagramNode::anchorPoints(QList<qreal> &left, QList<qreal> &right, QList<qreal> &top, QList<qreal> &bottom, qreal &cx, qreal &cy)
{
	QPointF		p;
	QRectF		r;

	r = mapRectToScene(boundingRect());
	p = r.center();

	left.push_back( r.left() );
	right.push_back( r.right() );
	top.push_back(r.top());
	bottom.push_back(r.bottom());
	cx = p.x();
	cy = p.y();
}
