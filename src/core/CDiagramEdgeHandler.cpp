/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <limits>
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramGroup>
#include <core/CDiagramPort>
#include <core/CDiagramCommandBuilder>
#include <core/CDiagramEdgeHandler>

static void updateItemLinkMode(CDiagramItem *item, bool linking)
{
	CDiagramGroup			*g = NULL;
	CDiagramNode			*n = NULL;
	CDiagramItem			*t = NULL;
	QList<CDiagramItem*>	list;

	switch(item->interType())
	{
	case Global::Node:
		n = dynamic_cast<CDiagramNode*>(item);
		if (n && n->connectable() )
		{
			if (linking)
				n->setMode( CDiagramNode::Linking );
			else
				n->setMode( CDiagramNode::Normal );
		}
		break;

	case Global::Group:
		g = dynamic_cast<CDiagramGroup*>(item);
		if (g)
		{
			list = g->items();
			for (int i = 0; i < list.length(); ++i)
			{
				updateItemLinkMode(list.at(i), linking);
			}
		}
		break;

	default:
		break;

	}; // End of switch
}

static CDiagramPort* getDiagramPortAt(CDiagram *d, const QPointF &p)
{
	QGraphicsItem	*item = NULL;
	CDiagramPort*	c = NULL;

	if (!d) return c;
	
	foreach (item, d->items(p))
	{
		c = qgraphicsitem_cast<CDiagramPort*>(item);
		if (c)
			break;
	}

	return c;
}

CDiagramEdgeDragger::CDiagramEdgeDragger(CDiagramEdgeHandler *parent, int index, int size)
	: CDiagramItemHandlerDragger(parent, index, size),
	  m_overPort(false),
	  m_port(NULL),
	  m_overPortSize(32)
{
}

QRectF CDiagramEdgeDragger::boundingRect() const
{
	qreal		hs, hh;
	QRectF		r;

	if (m_overPort)
	{
		hs = (m_overPortSize + m_pen.widthF() ) / 2.0;
		hh = m_overPortSize + m_pen.widthF();
		r = QRectF(-hs, -hs, hh, hh);
	}
	else
	{
		hs = (m_size + m_pen.widthF() ) / 2.0;
		hh = m_size + m_pen.widthF();
		r = QRectF(-hs, -hs, hh, hh);
	}
	return r;
}

void CDiagramEdgeDragger::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	Q_UNUSED(option);

	QRadialGradient		radialGradient;
	QColor				colors[2];
	qreal				s = m_size / 3.0;
	
	colors[0] = QColor( 255, 153, 0, 255);
	colors[1] = QColor( 255, 255, 255, 255);
	colors[0].setAlphaF(0.9);
	colors[1].setAlphaF(0.9);

	radialGradient = QRadialGradient(-s, -s, m_size);
	radialGradient.setCenter(s, s);
	radialGradient.setFocalPoint(s, s);
	radialGradient.setColorAt(0, colors[0]);
	radialGradient.setColorAt(1, colors[1]);

	painter->save();
	painter->setRenderHint( QPainter::Antialiasing, true );
	if (m_overPort)
	{
		m_pen.setWidth(2);
		m_pen.setColor(Qt::black );
		
		painter->setPen( m_pen );
		painter->setBrush( QColor(0,255,0,128) );
		painter->drawEllipse(-m_overPortSize/2.0, -m_overPortSize/2.0, m_overPortSize, m_overPortSize);
	}
	else
	{
		if (dragPolicy() == Global::BothDirections)
			m_type = CDiagramItemHandlerDragger::Circle;
		else
			m_type = CDiagramItemHandlerDragger::Diamond;
		m_pen.setWidth(1);
		m_pen.setColor(Qt::gray);
		painter->setPen( m_pen );
		painter->setBrush(radialGradient);
		painter->drawPath(computePath());
	}
	painter->restore();
}

void CDiagramEdgeDragger::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		m_dragging = true;
		update();
		m_p->grabFocus();
		m_p->startDragging(this);
		QGraphicsItem::mousePressEvent(event);
		update();
	}
	else
	{
		event->ignore();
	}
}

void CDiagramEdgeDragger::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{	
	CDiagram			*md = NULL;
	QGraphicsScene		*sc = NULL;
	CDiagramPort		*cc = NULL;

	sc = scene();
	if (!sc) return;
	md = dynamic_cast<CDiagram*>(sc);

	if (m_dragging)
	{
		setPos(mapToParent(event->pos()));
		m_p->doDragging(this);
		m_port = getDiagramPortAt(md, event->scenePos());
		m_overPort = (m_port != NULL);
		update();
		event->accept();
	}
	else
	{
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void CDiagramEdgeDragger::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if ( !(event->buttons() & Qt::LeftButton) )
	{
		update();
		m_p->finishDragging(this);
		QGraphicsItem::mouseReleaseEvent(event);
		m_dragging = false;
		m_overPort = false;
		m_port = NULL;
	}
	else
	{
		event->ignore();
	}
}

bool CDiagramEdgeDragger::isOverPort()
{
	return m_overPort;
}

CDiagramPort* CDiagramEdgeDragger::currentPort()
{
	return m_port;
}

CDiagramEdgeHandler::CDiagramEdgeHandler()
	: CDiagramItemHandler(),
	  m_currEdge(NULL),
	  m_usedDraggersNum(0),
	  m_draggerNum(6)
{
	m_draggerSize = 8;
	initDraggers();
}

void CDiagramEdgeHandler::initDraggers()
{
	CDiagramEdgeDragger *dragger = NULL;

	m_draggers.clear();
	Q_ASSERT(m_draggers.length() == 0);

	for (int i=0; i<m_draggerNum; ++i)
	{
		dragger = new CDiagramEdgeDragger(this, i, m_draggerSize);
		if (dragger)
		{
			dragger->setVisible(true);
			dragger->setParentItem(this);
			m_draggers.append(dragger);
		}
	}

	Q_ASSERT(m_draggers.length() == m_draggerNum);
	m_draggers[0]->setUsed(true);
	m_draggers[0]->setDragPolicy( Global::BothDirections );
	m_draggers[m_draggerNum - 1]->setUsed(true);
	m_draggers[m_draggerNum - 1]->setDragPolicy( Global::BothDirections );
}

QRectF CDiagramEdgeHandler::boundingRect() const
{
	return shape().boundingRect();
}

QPainterPath CDiagramEdgeHandler::shape() const
{
	QRectF					rect;
	QPainterPath			path;
	CDiagramItemHandlerDragger	*d = NULL;

	foreach (d, m_draggers)
	{
		rect = d->boundingRect();
		rect.moveCenter(d->pos());
		path.addRect(rect);
	}
	return path;
}

void CDiagramEdgeHandler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
}

void CDiagramEdgeHandler::layoutDraggers()
{
	qreal								dx, dy;
	QPointF								p1, p2;
	int									i = 0, activePoints = 0;
	QList<CDiagramEdgeControlPoint*>	    ctrlPoints;

	if (!m_currEdge)
		return;

	ctrlPoints = m_currEdge->getCtrlPoints();
	activePoints = m_currEdge->activePoints();
	m_usedDraggersNum = (activePoints == 2) ? 2 : activePoints - 1;
	p1 = mapFromItem(m_currEdge, ctrlPoints[0]->pos());
	p2 = mapFromItem(m_currEdge, ctrlPoints[activePoints - 1]->pos());
	m_draggers[0]->setUsed(true);
	m_draggers[0]->setDragPolicy( Global::BothDirections );
	m_draggers[0]->setPos( p1 );
	m_draggers[m_draggerNum - 1]->setUsed(true);
	m_draggers[m_draggerNum - 1]->setDragPolicy( Global::BothDirections );
	m_draggers[m_draggerNum - 1]->setPos( p2 );

	for (i = 1; i<activePoints - 2; ++i)
	{
		p1 = mapFromItem(m_currEdge, ctrlPoints[i]->pos());
		p2 = mapFromItem(m_currEdge, ctrlPoints[i + 1]->pos()); 
		m_draggers[i]->setUsed(true);
		m_draggers[i]->setX((p1.x() + p2.x())/2.0);
		m_draggers[i]->setY((p1.y() + p2.y())/2.0);
		dx = qAbs(p1.x() - p2.x());
		dy = qAbs(p1.y() - p2.y());
		if (dy == 0.0)
			m_draggers[i]->setDragPolicy(Global::Vertically);
		else
			m_draggers[i]->setDragPolicy(Global::Horizontally);
		if (dx + dy < 5.0)
			m_draggers[i]->setUsed(false);
	}
	for (; i<m_draggerNum-1; ++i) m_draggers[i]->setUsed(false);
	update();
}

void CDiagramEdgeHandler::startDragging(CDiagramItemHandlerDragger *dragger)
{
	if ( !scene() )
		return;

	if (m_d && dragger->dragPolicy() == Global::BothDirections)
	{
		updateConnectableItems(true);
	}
	m_dragger = dragger;
}

void CDiagramEdgeHandler::doDragging(CDiagramItemHandlerDragger *dragger)
{
	int							g;
	QPointF						p;
	CDiagramCommandBuilder		*b = NULL;
	CDiagramEdgeDragger			*d = NULL;

	if (!scene() || !m_currEdge)
		return;
	
	d = dynamic_cast<CDiagramEdgeDragger*>(dragger);
	if (!d) return;

	if (m_d->snapToGrid())
	{
		g = m_d->gridSize();
		p = mapToScene(dragger->pos());
		switch(dragger->dragPolicy())
		{
		case Global::BothDirections:
			p = QPointF( qRound(p.x() / g) * g, qRound(p.y() / g) * g );
			break;

		case Global::Horizontally:
			p = QPointF( qRound(p.x() / g) * g, p.y() );
			break;

		case Global::Vertically:
			p = QPointF( p.x(), qRound(p.y() / g) * g );
			break;
		};
		dragger->setPos(mapFromScene(p));
	}

	b = CDiagramCommandBuilder::instance();
	m_d->addUndoCommand( b->createCommand( Global::CMD_ADJUSTROUTE, 
										   Q_ARG(CDiagram*, m_d),
										   Q_ARG(CDiagramEdge*, m_currEdge),
										   Q_ARG(CDiagramEdgeDragger*, d),
										   Q_ARG(QUndoCommand*, NULL)) );
	layout();
}

void CDiagramEdgeHandler::finishDragging(CDiagramItemHandlerDragger *dragger)
{
	int						index = 0;
	qreal					pc = -1.0;
	QPointF					p1, p2;
	QLineF					line;
	CDiagramPort			*cc = NULL;
	CDiagramCommandBuilder	*b = NULL;
	CDiagramEdgeDragger		*d = NULL;
	
	if (!scene() || !m_currEdge)
		return;

	d = dynamic_cast<CDiagramEdgeDragger*>(dragger);
	if (!d) return;

	index = dragger->index();
	if (d->dragPolicy() == Global::BothDirections)
	{
		if (d->isOverPort() && d->currentPort() != m_currEdge->startPort() && d->currentPort() != m_currEdge->endPort())
		{
			cc = d->currentPort();
			b = CDiagramCommandBuilder::instance();
			m_currEdge->setAutoTrack(false);
			m_d->addUndoCommand( b->createCommand( Global::CMD_RECONNECT, 
												   Q_ARG(CDiagram*, m_d),
												   Q_ARG(CDiagramEdge*, m_currEdge),
												   Q_ARG(CDiagramEdgeDragger*, d), 
												   Q_ARG(CDiagramPort*, cc), 
												   Q_ARG(CDiagramPort*, (index == 0)? m_currEdge->startPort() : m_currEdge->endPort()),
												   Q_ARG(QUndoCommand*, NULL) ) );
			updateConnectableItems(false);
			layout();
		}
		else
		{
			// check whether we should move the connect point a little bit
			cc = (index == 0)? m_currEdge->startPort() : m_currEdge->endPort();
			if (cc)
			{
				m_currEdge->setAutoTrack(false);
				pc = cc->getPercent( m_dragger->mapToScene(QPointF(0, 0)) );
				if (pc < 0)
				{
					b = CDiagramCommandBuilder::instance();
					m_d->addUndoCommand( b->createCommand( Global::CMD_RECONNECT, 
														   Q_ARG(CDiagram*, m_d),
														   Q_ARG(CDiagramEdge*, m_currEdge),
														   Q_ARG(CDiagramEdgeDragger*, d), 
														   Q_ARG(CDiagramPort*, NULL), 
														   Q_ARG(CDiagramPort*, cc),
														   Q_ARG(QUndoCommand*, NULL) ) );
				}
				else
				{
					if (index == 0)
					{
						m_currEdge->setStartPercent(pc);
					}
					else
					{
						m_currEdge->setEndPercent(pc);
					}
					cc->setConnectPosition(m_currEdge, pc);
				}
			}
			else
			{
				m_currEdge->retrack();
			}
			updateConnectableItems(false);
			layout();
		}
	}
	m_dragger = NULL;
}

bool CDiagramEdgeHandler::support(const QList<CDiagramItem *> &items)
{
	bool					r = false;
	CDiagramEdge	*item = NULL;

	if (items.length() == 1)
	{
		item = dynamic_cast<CDiagramEdge*>(items.first());
		if (item)
			r = true;
	}
	return r;
}

void CDiagramEdgeHandler::updateConnectableItems(bool linking)
{
	QList<CDiagramItem*>	list;

	list = m_d->topLevelItems();
	for (int i = 0; i < list.length(); ++i)
	{
		updateItemLinkMode(list.at(i), linking);
	}
}


void CDiagramEdgeHandler::init(CDiagram *d, QList<CDiagramItem *> &items, const QRectF &rect)
{
	CDiagramEdge*	edge = NULL;

	if (items.length() == 1)
	{
		edge = dynamic_cast<CDiagramEdge*>(items.first());
		if (!edge || edge->activePoints() < 2)
			return;
		m_currEdge = edge;
	}

	CDiagramItemHandler::init(d, items, rect);
	layout();
}