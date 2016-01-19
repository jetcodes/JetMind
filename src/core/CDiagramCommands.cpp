/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramGroup>
#include <core/CDiagramPort>
#include <core/CDiagramEdge>
#include <core/CDiagramTextNode>
#include <core/CDiagramPixmapNode>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramEdgeHandler>
#include <core/CDiagramCommands>
#include <core/CDiagramClipboard>

#define CMD_SPRITE_MOVE		 0
#define CMD_SPRITE_ROTATE    1
#define CMD_SPRITE_RESIZE	 2
#define CMD_PATH_ADJUST		 3
#define CMD_SPRITE_DRAGGING  4
#define CMD_SPRITE_LABELPOS  5
#define CMD_EDGE_CONNPOINT	 6
#define PI 3.1415926

bool xcoordLessThan(const CDiagramItem *item1, const CDiagramItem *item2)
{
	bool	r = false;

	if (item1 && item2)
	{
		r = item1->x() < item2->x();
	}
	return r;
}

bool ycoordLessThan(const CDiagramItem *item1, const CDiagramItem *item2)
{
	bool	r = false;

	if (item1 && item2)
	{
		r = item1->y() < item2->y();
	}
	return r;
}

QRectF getVisibleRect(QGraphicsScene *sc, QGraphicsView *view = NULL)
{
	QRectF			r;
	QGraphicsView	*v = NULL;

	if (view)
		v = view;
	else if (sc)
		v = sc->views().first();

	if (v)
		r = v->mapToScene(v->viewport()->geometry()).boundingRect();
	return r;
}

CNewItemCommand::CNewItemCommand(CDiagram *d, 
								 CDiagramItem *item, 
								 const QPointF &p, 
								 QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_item(item),
	  m_pos(p)
{
}

void CNewItemCommand::redo()
{
	m_item->defaultInit();
	m_d->insertItem(m_item, m_pos);
}

void CNewItemCommand::undo()
{
	m_d->deleteItem(m_item);
}

CDeleteItemCommand::CDeleteItemCommand(CDiagram *d, const QList<CDiagramItem*> &items, QUndoCommand *parent)
	: CDiagramCommand(d, parent)
{
	CDiagramItem			*item = NULL;
	CDiagramEdge			*edge = NULL;
	
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (item->interType() == Global::Edge)
		{
			edge = dynamic_cast<CDiagramEdge*>(item);
			if (edge) m_edges.push_back(edge);
		}
		else
		{
			m_items.push_back(item);
		}
	}
}

void CDeleteItemCommand::redo()
{
	CDiagramItem	*item = NULL;
	CDiagramNode	*node = NULL;
	CDiagramEdge	*edge = NULL;
	CDiagramPort	*port = NULL;

	for (int i = 0; i < m_edges.length(); ++i)
	{
		deleteEdge( m_edges.at(i) );
	}

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
			{	
				foreach (edge, node->getEdges())
				{
					port = edge->startPort();
					if (port && port->getParentNode() == node)
					{
						edge->setStartPort(NULL);
						if (m_startMaps.count(edge) == 0)
						{
							m_startMaps[edge] = QList<CDiagramPort*>();
						}
						m_startMaps[edge].push_back(port);
						// port->removeEdge(edge);
					}

					port = edge->endPort();
					if (port && port->getParentNode() == node)
					{
						edge->setEndPort(NULL);
						if (m_endMaps.count(edge) == 0)
						{
							m_endMaps[edge] = QList<CDiagramPort*>();
						}
						m_endMaps[edge].push_back(port);
						// port->removeEdge(edge);
					}
				}
			}
		}
		m_d->deleteItem(item);
	}

    QUndoCommand::redo();
}

void CDeleteItemCommand::undo()
{
	CDiagramItem	*item = NULL;
	CDiagramNode	*node = NULL;
	CDiagramEdge	*edge = NULL;
	CDiagramPort	*port = NULL;
	QMap<CDiagramEdge*, QList<CDiagramPort*> >::iterator	it;

    QUndoCommand::undo();

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		m_d->insertItem(item, item->pos(), false);
	}

	//
	for (it = m_startMaps.begin(); it != m_startMaps.end(); ++it)
	{
		edge = it.key();
		foreach (port, it.value())
		{
			edge->setStartPort(port);
		}
	}

	for (it = m_endMaps.begin(); it != m_endMaps.end(); ++it)
	{
		edge = it.key();
		foreach (port, it.value())
		{
			edge->setEndPort(port);
		}
	}

	// reconstruct the edges, we only have to reassign
	// the edges to the specified nodes, and the nodes
	// are kept stored in the edges all the way down here.
	for (int i = 0; i < m_edges.length(); ++i)
	{
		edge = m_edges.at(i);
		m_d->insertItem(edge, edge->pos(), false);

		if (edge->endPort()) 
		{
			edge->endPort()->addEdge(edge);
		}
		if (edge->startPort()) 
		{
			edge->startPort()->addEdge(edge);
		}
		edge->refresh();
	}
}

void CDeleteItemCommand::deleteEdge(CDiagramEdge *c)
{
	CDiagramPort	*r1 = NULL;
	CDiagramPort	*r2 = NULL;

	if (!c) return;
	
	r1 = c->startPort();
	r2 = c->endPort();

	// The port will not be deleted from the edge
	// only the nodes will remove the edge
	if (r1) r1->removeEdge(c);
	if (r2)	r2->removeEdge(c);

	m_d->deleteItem(c);
}

CGroupCommand::CGroupCommand(CDiagram *d, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_group(NULL)
{
	m_items = m_d->selectedTopLevelItems();
	if (m_items.size() >= 2)
		m_group = new CDiagramGroup(m_d);
}

void CGroupCommand::redo()
{
	if ( m_group )
	{
		m_d->insertItem(m_group, m_group->pos() );
		m_group->initGroup(m_items);
	}
}

void CGroupCommand::undo()
{
	if (m_group)
	{
		m_group->destroyGroup();
		m_d->deleteItem(m_group);
	}
}


CUnGroupCommand::CUnGroupCommand(CDiagram *d, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_group(NULL)
{
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	if (items.length() == 1)
	{
		m_group = dynamic_cast<CDiagramGroup*>(items.first());
		if (m_group)
			m_items = m_group->items();
	}
}

void CUnGroupCommand::redo()
{
	if (m_group)
	{
		m_group->destroyGroup();
		m_d->deleteItem(m_group);
		m_d->clearSelection();
		m_items.at(0)->setSelected(true);
	}
}

void CUnGroupCommand::undo()
{
	if (m_group)
	{
		m_d->insertItem(m_group, m_group->pos() );
		m_group->initGroup( m_items );
	}
}

CRotateCommand::CRotateCommand(CDiagram *d, 
							   const QPointF &origin, 
							   qreal delta, 
							   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_origin(origin),
	  m_delta(delta)
{
	foreach (CDiagramItem *item, d->selectedTopLevelItems())
	{
		if ((item->interType() == Global::Node || item->interType() == Global::Group) && item->rotatable() )
		{
			m_items.push_back(item);
			m_oldPos.push_back(item->pos());
		}
	}
}

void CRotateCommand::undo()
{
	QPointF			p;
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0;i < m_items.length(); ++i)
	{
		item = m_items[i];
		item->setPos( m_oldPos[i] );
		item->setRotation( item->rotation() - m_delta );
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsRotationChanged(-m_delta);
}

void CRotateCommand::redo()
{
	qreal			r, x1, y1, x2, y2;
	QPointF			p;
	CDiagramNode	*node = NULL;
	CDiagramItem	*m = NULL;

	r = m_delta * PI / 180.0;
	foreach (CDiagramItem *item, m_items)
	{
		p = item->mapToScene( QPointF(0,0) );
		x1 = p.x() - m_origin.x();
		y1 = p.y() - m_origin.y();
		x2 = x1 * qCos(r) - y1 * qSin(r); 
		y2 = x1 * qSin(r) + y1 * qCos(r);
		item->setPos( item->parentItem()->mapFromScene( QPointF(m_origin.x() + x2, m_origin.y() + y2) ) );
		item->setRotation( item->rotation() + m_delta );
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsRotationChanged(m_delta);
}

bool CRotateCommand::mergeWith(const QUndoCommand *cmd)
{
	if (cmd->id() != CMD_SPRITE_ROTATE)
		return false;

	const CRotateCommand *c = dynamic_cast<const CRotateCommand*>(cmd);
	if ( c->m_items.length() != m_items.length() )
		return false;
	for (int i=0; i<m_items.length(); ++i)
		if (m_items[i] != c->m_items[i])
			return false;
	if (m_origin != c->m_origin) return false;
	m_delta += c->m_delta;
	return true;
}

int CRotateCommand::id() const
{
	return CMD_SPRITE_ROTATE;
}

CResizeCommand::CResizeCommand(CDiagram *d,
							   const QList<CDiagramItem*> &items,
							   const QList<QRectF> &oldGeometry,
							   const QList<QRectF> &newGeometry,
							   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_oldGeometry(oldGeometry),
	  m_newGeometry(newGeometry)
{
	foreach (CDiagramItem *item, items)
	{
		if (item->interType() == Global::Node || item->interType() == Global::Group)
		{
			m_items.push_back(item);
		}
	}
}

void CResizeCommand::undo()
{
	Q_ASSERT(m_items.length() == m_oldGeometry.length());

	int				i = 0;
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;
	
	for (i = 0; i < m_items.length(); ++i)
	{
		item = m_items[i];
		item->setGeometry(m_oldGeometry[i]);
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}

	m_d->emitItemsGeometryChanged();
}

void CResizeCommand::redo()
{
	Q_ASSERT(m_items.length() == m_oldGeometry.length());

	int				i = 0;
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;
	
	for (i = 0; i < m_items.length(); ++i)
	{
		item = m_items[i];
		item->setGeometry(m_newGeometry[i]);
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsGeometryChanged();
}

bool CResizeCommand::mergeWith(const QUndoCommand *cmd)
{
	if (cmd->id() != CMD_SPRITE_RESIZE)
		return false;

	const CResizeCommand *c = dynamic_cast<const CResizeCommand*>(cmd);
	if ( c->m_items.length() != m_items.length() )
		return false;
	for (int i=0; i<m_items.length(); ++i)
		if (m_items[i] != c->m_items[i])
			return false;
	m_newGeometry = c->m_newGeometry;
	return true;
}

int CResizeCommand::id() const
{
	return CMD_SPRITE_RESIZE;
}

CDraggingItemsCommand::CDraggingItemsCommand(CDiagram *d, QList<CDiagramItem*> &selectedItems, QList<QPointF> &newPositions, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_items(selectedItems),
	  m_newPositions(newPositions)
{
	foreach (CDiagramItem *item, selectedItems)
	{
		m_oldPositions.push_back(item->pos());
	}
}

void CDraggingItemsCommand::redo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;
	CDiagramEdge	*edge = NULL;
	CDiagramPort	*port = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->interType() == Global::Edge)
		{
			edge = dynamic_cast<CDiagramEdge*>(item);
			if (edge)
			{
				port = edge->startPort();
				if (port && !m_items.contains( port->getParentNode() ) ) continue;
				port = edge->endPort();
				if (port && !m_items.contains( port->getParentNode() ) ) continue;

				edge->setPos(m_newPositions.at(i));
				edge->setStartPoint( edge->startPoint() + m_newPositions.at(i) - m_oldPositions.at(i) );
				edge->setEndPoint( edge->endPoint() + m_newPositions.at(i) - m_oldPositions.at(i) );
				edge->retrack();
			}
		}
		else
		{
			item->setPos( m_newPositions.at(i) );
		}
	}

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsGeometryChanged();
}

void CDraggingItemsCommand::undo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;
	CDiagramEdge	*edge = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		item->setPos( m_oldPositions.at(i) );
		if (item->interType() == Global::Edge)
		{
			edge = dynamic_cast<CDiagramEdge*>(item);
			if (edge)
			{
				edge->setStartPoint( edge->startPoint() - (m_newPositions.at(i) - m_oldPositions.at(i)) );
				edge->setEndPoint( edge->endPoint() - (m_newPositions.at(i) - m_oldPositions.at(i)) );
				edge->retrack();
			}
		}
	}

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item && item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsGeometryChanged();
}

bool CDraggingItemsCommand::mergeWith(const QUndoCommand *cmd)
{
	int		i = 0;

	if (cmd->id() != CMD_SPRITE_DRAGGING)
		return false;

	const CDraggingItemsCommand *c = static_cast<const CDraggingItemsCommand*>(cmd);
	if (!c || (c->m_items.length() != m_items.length()) || m_items.length() == 0)
		return false;

	for (i=0; i<m_items.length(); ++i)
	{
		if (m_items[i] != c->m_items[i])
			return false;
	}

	m_newPositions = c->m_newPositions;

	return true;
}

int CDraggingItemsCommand::id() const
{
	return CMD_SPRITE_DRAGGING;
}

CMoveItemsCommand::CMoveItemsCommand(CDiagram *d, const QList<CDiagramItem*> &selectedItems, qreal dx, qreal dy, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_items(selectedItems),
	  m_dx(dx),
	  m_dy(dy)
{
}

void CMoveItemsCommand::redo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->lockPosition()) continue;
		item->setPos( item->pos() + QPointF(m_dx, m_dy) );
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsMoving(m_dx, m_dy);
}

void CMoveItemsCommand::undo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->lockPosition()) continue;
		item->setPos( item->pos() - QPointF(m_dx, m_dy) );
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
	m_d->emitItemsMoving(-m_dx, -m_dy);
}

bool CMoveItemsCommand::mergeWith(const QUndoCommand *cmd)
{
	int		i = 0;

	if (cmd->id() != CMD_SPRITE_MOVE)
		return false;

	const CMoveItemsCommand *c = static_cast<const CMoveItemsCommand*>(cmd);
	if (c->m_items.length() != m_items.length())
		return false;

	for (i=0; i<m_items.length(); ++i)
		if (m_items[i] != c->m_items[i])
			return false;

	m_dx += c->m_dx;
	m_dy += c->m_dy;
	return true;
}

int CMoveItemsCommand::id() const
{
	return CMD_SPRITE_MOVE;
}

CAdjustRouteCommand::CAdjustRouteCommand(
	CDiagram *d, 
	CDiagramEdge *c, 
	CDiagramEdgeDragger *dragger, 
	QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_edge(c),
	  m_dragger(dragger)
{
	int							index = -1;
	CDiagramEdgeControlPoint	*ctrl = NULL;

	index = dragger->index();
	if (index + 1 < m_edge->getCtrlPoints().length())
	{
		ctrl = m_edge->getCtrlPoints()[index+1];

		m_xAnchored[1] = ctrl->anchorX();
		m_yAnchored[1] = ctrl->anchorY();
	}

	ctrl = m_edge->getCtrlPoints()[index];
	m_xAnchored[0] = ctrl->anchorX();
	m_yAnchored[0] = ctrl->anchorY();

	m_oldPos = m_edge->mapToScene(m_edge->getCtrlPoints()[index]->pos());
	m_startPoint = m_edge->startPoint();
	m_endPoint = m_edge->endPoint();
	m_newPos = dragger->scenePos();
}

void CAdjustRouteCommand::redo()
{
	int			index = -1;
	QPointF		p = m_edge->mapFromScene(m_newPos);
	QPointF		sceneP =m_newPos;

	index = m_dragger->index();
	switch (m_dragger->dragPolicy())
	{
	case Global::BothDirections:
		if (index == 0)
		{
			m_edge->setStartPoint(sceneP);
			m_edge->retrack();
		}
		else
		{
			m_edge->setEndPoint(sceneP);
			m_edge->retrack();
		}
		break;

	case Global::Horizontally:
		m_edge->getCtrlPoints()[index]->setX(p.x(), true);
		m_edge->getCtrlPoints()[index + 1]->setX(p.x(), true);
		m_edge->refresh();
		break;

	case Global::Vertically:
		m_edge->getCtrlPoints()[index]->setY(p.y(), true);
		m_edge->getCtrlPoints()[index + 1]->setY(p.y(), true);
		m_edge->refresh();
		break;

	default:
		break;
	};
	m_d->emitItemsGeometryChanged();
}

void CAdjustRouteCommand::undo()
{
	int			index = -1;
	QPointF		p = m_edge->mapFromScene(m_oldPos);
	QPointF		sceneP;

	index = m_dragger->index();
	sceneP = m_oldPos;
	switch (m_dragger->dragPolicy())
	{
	case Global::BothDirections:
		if (index == 0) 
		{
			m_edge->setStartPoint(m_startPoint);
			m_edge->retrack();
		}
		else 
		{
			m_edge->setEndPoint(m_endPoint);
			m_edge->retrack();
		}
		break;

	case Global::Horizontally:
		m_edge->getCtrlPoints()[index]->setX(p.x(), m_xAnchored[0]);
		m_edge->getCtrlPoints()[index + 1]->setX(p.x(), m_xAnchored[1]);
		m_edge->refresh();
		break;

	case Global::Vertically:
		m_edge->getCtrlPoints()[index]->setY(p.y(), m_yAnchored[0]);
		m_edge->getCtrlPoints()[index + 1]->setY(p.y(), m_yAnchored[1]);
		m_edge->refresh();
		break;

	default:
		break;
	};
	m_d->emitItemsGeometryChanged();
}

bool CAdjustRouteCommand::mergeWith(const QUndoCommand *cmd)
{
	if (cmd->id() != CMD_PATH_ADJUST)
		return false;

	const CAdjustRouteCommand *c = static_cast<const CAdjustRouteCommand*>(cmd);
	if (c->m_dragger != m_dragger || c->m_edge != m_edge)
		return false;

	m_newPos = c->m_newPos;
	return true;
}

int CAdjustRouteCommand::id() const 
{
	return CMD_PATH_ADJUST;
}

CReconnectCommand::CReconnectCommand(
	CDiagram *d,
	CDiagramEdge *c,
	CDiagramEdgeDragger *dragger,
	CDiagramPort *n,
	CDiagramPort *o,
	QUndoCommand *parent)
	: CDiagramCommand(d, parent ),
	  m_dragger(dragger),
	  m_edge(c),
	  m_newPort(n),
	  m_oldPort(o)
{
}

void CReconnectCommand::redo()
{
	if (m_dragger->index() == 0) 
		m_edge->setStartPort(m_newPort);
	else 
		m_edge->setEndPort(m_newPort);
	
	if (m_oldPort) 
		m_oldPort->removeEdge(m_edge);
	
	if (m_newPort)
		m_newPort->addEdge(m_edge);
	
	m_edge->retrack();
}

void CReconnectCommand::undo()
{
	if (m_dragger->index() == 0) 
		m_edge->setStartPort(m_oldPort);
	else 
		m_edge->setEndPort(m_oldPort);
	
	if (m_oldPort)
		m_oldPort->addEdge(m_edge);

	if (m_newPort)
		m_newPort->removeEdge(m_edge);	
	
	m_edge->retrack();
}

CChangePropertyCommand::CChangePropertyCommand(CDiagram *d, 
						   const QList<CDiagramItem*> items,
						   const char *name, 
						   const QVariant &value, 
						   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_items(items),
	  m_name(name),
	  m_value(value)
{
	foreach (CDiagramItem *s, m_items)
	{
		m_oldValues.push_back( s->property(m_name) );
	}
}

CChangePropertyCommand::CChangePropertyCommand(CDiagram *d,
						   const QList<CDiagramItem*> items,
						   const QList<QVariant> &values,
						   const char *name,
						   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_items(items),
	  m_name(name),
	  m_newValues(values),
	  m_value(QVariant())
{
	foreach (CDiagramItem *s, items)
	{
		m_oldValues.push_back( s->property(m_name) );
	}
}

void CChangePropertyCommand::undo()
{
	int				i = 0;
	CDiagramItem	*s = NULL;
	
	if (m_oldValues.size() != m_items.size()) return;

	for (i = 0; i < m_items.size(); ++i)
	{
		s = m_items[i];
		s->setProperty(m_name, m_oldValues[i]);
	}
	QUndoCommand::undo();
}

void CChangePropertyCommand::redo()
{
	if (m_value.isValid())
	{
		foreach (CDiagramItem *s, m_items)
		{
			s->setProperty(m_name, m_value);
		}
	}
	else if (m_newValues.size() == m_items.size())
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			m_items[i]->setProperty(m_name, m_newValues[i]);
		}
	}
	QUndoCommand::redo();
}

CChangeRouterCommand::CChangeRouterCommand(CDiagram *d, 
									   const QList<CDiagramEdge*> &edges, 
									   const CDiagramEdgeStyle *style, 
									   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_edges(edges),
	  m_newStyle(style)
{
	foreach (CDiagramEdge *c, edges)
	{
		m_oldStyles.push_back( c->edgeStyle() );
	}
}

void CChangeRouterCommand::redo()
{
	foreach (CDiagramEdge *c, m_edges)
	{
		c->setEdgeStyle( m_newStyle );
		c->retrack();
	}
}

void CChangeRouterCommand::undo()
{
	if (m_edges.size() != m_oldStyles.size())
		return;

	for (int i = 0; i < m_edges.size(); ++i)
	{
		m_edges[i]->setEdgeStyle( m_oldStyles[i] );
		m_edges[i]->retrack();
	}
}

CChangeStartArrowCommand::CChangeStartArrowCommand(CDiagram *d, 
											   const QList<CDiagramEdge*> &conns, 
											   const CDiagramArrow *style, 
											   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_edges(conns),
	  m_newStyle(style)
{
	foreach (CDiagramEdge *c, conns)
	{
		m_oldStyles.push_back( c->startArrow() );
	}
}

void CChangeStartArrowCommand::undo()
{
	if (m_edges.size() != m_oldStyles.size()) return;

	for (int i = 0; i < m_edges.size(); ++i)
	{
		m_edges[i]->setStartArrow( m_oldStyles[i] );
		m_edges[i]->refresh();
	}
}

void CChangeStartArrowCommand::redo()
{
	foreach (CDiagramEdge *c, m_edges)
	{
		c->setStartArrow(m_newStyle);
		c->refresh();
	}
}

CChangeEndArrowCommand::CChangeEndArrowCommand(CDiagram *d, 
											   const QList<CDiagramEdge*> &conns, 
											   const CDiagramArrow *style, 
											   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_edges(conns),
	  m_newStyle(style)
{
	foreach (CDiagramEdge *c, conns)
	{
		m_oldStyles.push_back( c->startArrow() );
	}
}

void CChangeEndArrowCommand::undo()
{
	if (m_edges.size() != m_oldStyles.size()) return;

	for (int i = 0; i < m_edges.size(); ++i)
	{
		m_edges[i]->setEndArrow( m_oldStyles[i] );
		m_edges[i]->refresh();
	}
}

void CChangeEndArrowCommand::redo()
{
	foreach (CDiagramEdge *c, m_edges)
	{
		c->setEndArrow(m_newStyle);
		c->refresh();
	}
}

CPasteCommand::CPasteCommand(CDiagram *d, QUndoCommand *parent)
	: CDiagramCommand(d, parent)
{
	CDiagramClipboard	*clipboard = CDiagramClipboard::clipboard();

	m_image = clipboard->image(CDiagramClipboard::Global);
	m_pixmap = clipboard->pixmap(CDiagramClipboard::Global);
	m_text = clipboard->text(CDiagramClipboard::Application);
	if (m_text.isNull() || m_text.isEmpty())
		m_text = clipboard->text(CDiagramClipboard::Global);
	clipboard->clear(CDiagramClipboard::All);
}

void CPasteCommand::redo()
{
	int							r;
	QDomNode					n;	
	QDomNodeList				list;
	QDomElement					element;
	QString						id, type, ctg, name;
	QDomDocument				doc;
	CDiagramTextNode			*textNode = NULL;
	CDiagramPixmapNode			*imageNode = NULL;
	CDiagramNode				*node = NULL;
	CDiagramEdge				*edge = NULL;
	CDiagramPort				*port = NULL;
	CDiagramItem				*item = NULL;
	CDiagramItemTypeCategory	*factory = NULL;
	QList<CDiagramItem*>		items;
	QPointF						delta(10, 10);
	QRectF						rect, sc;
	QGraphicsView				*view = NULL;

	if (doc.setContent(m_text))
	{
		list = doc.elementsByTagName(QLatin1String("item"));
		for (int i = 0; i < list.count(); ++i)
		{
			n = list.at(i);
			if (!n.isNull() && n.isElement())
			{
				element = n.toElement();
				id = element.attribute(QLatin1String("id"));
				type = element.attribute(QLatin1String("type"));
				ctg = element.attribute(QLatin1String("category"));
				name = element.attribute(QLatin1String("name"));

				factory = CDiagramItemTypeDatabase::instance()->getCategory(ctg);
				if (factory)
				{
					item = factory->createItem(name, Q_ARG(CDiagram*, m_d));
					m_d->insertItem(item, item->pos());
					r = item->id();
					item->fromXml(element);
					
					// construct the rect
					rect = rect.united( item->mapToScene(item->boundingRect()).boundingRect() );

					// after reconstruction, the id/pos is changed to the original
					// ones, so we have to update to the new one recorded before.
					item->setId(r);
					if (item->interType() == Global::Edge)
					{
						edge = dynamic_cast<CDiagramEdge*>(item);
						m_edges.push_back(edge);
					}
					else
					{
						m_items.insert(id.toInt(), item);
					}
				}
			}
		}

		// calculate the best place to put the copied nodes
		view = m_d->getMainView();
		if (view)
			sc = view->mapToScene(view->viewport()->geometry()).boundingRect();
		if (sc.contains(rect))
			delta = QPointF(10, 10);
		else
			delta = sc.center() - rect.center();

		m_d->clearSelection();
		// relocate the items
		foreach(item, m_items) 
		{
			item->setPos( item->pos() + delta );
			item->setSelected(true);
		}

		// relocate the edges
		foreach(edge, m_edges) 
		{
			edge->setStartPoint(edge->startPoint() + delta );
			edge->setEndPoint(edge->endPoint() + delta );
			edge->setSelected(true);

			// start port
			port = edge->startPort();
			if (port)
			{
				r = port->getParentNode()->id();
				if (m_items.contains(r))
				{
					node = dynamic_cast<CDiagramNode*>( m_items[r] );
					if (node)
					{
						port->removeEdge(edge);
						port = node->getPortByIndex(port->index());
						if (port)
							port->addEdge(edge);
						edge->setStartPort(port);
					}
				}
				else
				{
					edge->setStartPort(NULL);
					port->removeEdge(edge);
				}
			}
			
			// end port
			port = edge->endPort();
			if (port)
			{
				r = port->getParentNode()->id();
				if (m_items.contains(r))
				{
					node = dynamic_cast<CDiagramNode*>( m_items[r] );
					if (node)
					{
						port->removeEdge(edge);
						port = node->getPortByIndex(port->index());
						if (port)
							port->addEdge(edge);

						edge->setEndPort(port);
					}
				}
				else
				{
					edge->setEndPort(NULL);
					port->removeEdge(edge);
				}
			}
			edge->retrack();
		}
	}
	else
	{
		if (!m_text.trimmed().isEmpty())
		{
			textNode = new CDiagramTextNode(m_d);
			textNode->setText(m_text);

			m_d->insertItem(textNode, getVisibleRect(m_d).center());
			m_items.insert(textNode->id(), textNode);
		}
		else if (!m_image.isNull())
		{
			imageNode = new CDiagramPixmapNode(m_d);
			imageNode->setImage( m_image );

			m_d->insertItem(imageNode, getVisibleRect(m_d).center());
			m_items.insert(imageNode->id(), imageNode);
		}
		else if (!m_pixmap.isNull())
		{
			imageNode = new CDiagramPixmapNode(m_d);
			imageNode->setPixmap( m_pixmap );

			m_d->insertItem(imageNode, getVisibleRect(m_d).center());
			m_items.insert(imageNode->id(), imageNode);
		}
	}
}

void CPasteCommand::undo()
{
	CDiagramEdge		 *edge = NULL;
	CDiagramPort		 *p1 = NULL, *p2 = NULL;
	QList<CDiagramItem*> items;

	// on deletion, first, we must remove all
	// the connections.
	for (int i = 0; i < m_edges.length(); ++i)
	{
		edge = m_edges.at(i);
		p1 = edge->startPort();
		p2 = edge->endPort();
		if (p1)
		{
			p1->removeEdge(edge);
			edge->setStartPort(NULL);
		}
		if (p2)
		{
			p2->removeEdge(edge);
			edge->setEndPort(NULL);
		}
		m_d->deleteItem(edge);
	}

	// second, delete all the new added items
	items = m_items.values();
	for (int i = 0; i < items.length(); ++i)
	{
		m_d->deleteItem( items.at(i) );
	}

	m_edges.clear();
	m_items.clear();
}

CDragCopyItemsCommand::CDragCopyItemsCommand(CDiagram *d, QList<CDiagramItem*> &draggedItems, QList<QPointF> &newPositions, QUndoCommand *parent)
	: CDiagramCommand(d),
	  m_draggedItems(draggedItems),
	  m_newPositions(newPositions)
{
	
}

void CDragCopyItemsCommand::redo()
{
	int						r;
	QPointF					oldPos;
	QDomNode				n;	
	QDomNodeList			list;
	QDomElement				element;
	QString					id, type, ctg, name;
	QDomDocument			doc;
	CDiagramNode			*node = NULL;
	CDiagramEdge			*edge = NULL;
	CDiagramPort			*port = NULL;
	CDiagramItem			*item = NULL;
	CDiagramItemTypeCategory	*factory = NULL;
	QList<CDiagramItem*>	items;

	m_d->copyItems(m_draggedItems, doc);
	list = doc.elementsByTagName(QLatin1String("item"));
	for (int i = 0; i < list.count(); ++i)
	{
		n = list.at(i);
		if (!n.isNull() && n.isElement())
		{
			element = n.toElement();
			id = element.attribute(QLatin1String("id"));
			type = element.attribute(QLatin1String("type"));
			ctg = element.attribute(QLatin1String("category"));
			name = element.attribute(QLatin1String("name"));

			factory = CDiagramItemTypeDatabase::instance()->getCategory(ctg);
			if (factory)
			{
				item = factory->createItem(name, Q_ARG(CDiagram*, m_d));
				m_d->insertItem(item, item->pos());
				r = item->id();
				item->fromXml(element);
				oldPos = m_draggedItems.at(i)->pos();
				
				// after reconstruction, the id/pos is changed to the original
				// ones, so we have to update to the new ones
				item->setId(r);
				item->setPos( m_newPositions.at(i) );
				
				if (item->interType() == Global::Edge)
				{
					edge = dynamic_cast<CDiagramEdge*>(item);
					edge->setStartPoint(edge->startPoint() + m_newPositions.at(i) - oldPos );
					edge->setEndPoint(edge->endPoint() + m_newPositions.at(i) - oldPos );
					m_edges.push_back(edge);
				}
				else
				{
					m_items.insert(id.toInt(), item);
				}
			}
		}
	}

	for (int i = 0; i < m_edges.length(); ++i)
	{
		edge = m_edges.at(i);
		port = edge->startPort();
		if (port)
		{
			r = port->getParentNode()->id();
			if (m_items.contains(r))
			{
				node = dynamic_cast<CDiagramNode*>( m_items[r] );
				if (node)
				{
					port->removeEdge(edge);
					port = node->getPortByIndex(port->index());
					if (port)
						port->addEdge(edge);

					edge->setStartPort(port);
				}
			}
			else
			{
				edge->setStartPort(NULL);
				port->removeEdge(edge);
			}
		}
		
		port = edge->endPort();
		if (port)
		{
			r = port->getParentNode()->id();
			if (m_items.contains(r))
			{
				node = dynamic_cast<CDiagramNode*>( m_items[r] );
				if (node)
				{
					port->removeEdge(edge);
					port = node->getPortByIndex(port->index());
					if (port)
						port->addEdge(edge);

					edge->setEndPort(port);
				}
			}
			else
			{
				edge->setEndPort(NULL);
				port->removeEdge(edge);
			}
		}
		edge->retrack();
	}
	
	m_d->clearSelection();
	for (QMap<int, CDiagramItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		it.value()->setSelected(true);
	}

	for (int i = 0; i < m_edges.length(); ++i)
	{
		m_edges.at(i)->setSelected(true);
	}
}

void CDragCopyItemsCommand::undo()
{
	CDiagramEdge		 *edge = NULL;
	CDiagramPort		 *p1 = NULL, *p2 = NULL;
	QList<CDiagramItem*> items;

	// on deletion, first, we must remove all
	// the connections.
	for (int i = 0; i < m_edges.length(); ++i)
	{
		edge = m_edges.at(i);
		p1 = edge->startPort();
		p2 = edge->endPort();
		if (p1)
		{
			p1->removeEdge(edge);
			edge->setStartPort(NULL);
		}
		if (p2)
		{
			p2->removeEdge(edge);
			edge->setEndPort(NULL);
		}
		m_d->deleteItem(edge);
	}

	// second, delete all the new added items
	items = m_items.values();
	for (int i = 0; i < items.length(); ++i)
	{
		m_d->deleteItem( items.at(i) );
	}

	m_edges.clear();
	m_items.clear();

	m_d->clearSelection();
	for (int i = 0; i < m_draggedItems.length(); ++i)
	{
		m_draggedItems.at(i)->setSelected(true);
	}
}

CDragNewItemCommand::CDragNewItemCommand(CDiagram *d, CDiagramItem *item, const QPointF &newPosition, QUndoCommand *parent)
	: CDiagramCommand(d),
	  m_item(item),
	  m_item2(NULL),
	  m_newEdge(NULL),
	  m_newItem(NULL),
	  m_newPosition(newPosition)
{
}

CDragNewItemCommand::CDragNewItemCommand(CDiagram *d, CDiagramItem *i1, CDiagramItem *i2, QUndoCommand *parent)
	: CDiagramCommand(d),
	  m_item(i1),
	  m_item2(i2),
	  m_newEdge(NULL)
{
}

void CDragNewItemCommand::redo()
{
	if (m_item2)
		addEdgeRedo();
	else
		addNodeRedo();
}

void CDragNewItemCommand::undo()
{
	if (m_item2)
		addEdgeUndo();
	else
		addNodeUndo();
}

void CDragNewItemCommand::addEdgeRedo()
{
	CDiagramNode	*n1 = NULL, *n2 = NULL;
	CDiagramPort	*p1 = NULL, *p2 = NULL;

	if (!m_item || !m_item2 || m_item->interType() != Global::Node || m_item2->interType() != Global::Node)
		return;

	n1 = dynamic_cast<CDiagramNode*>(m_item);
	n2 = dynamic_cast<CDiagramNode*>(m_item2);

	p1 = n1->getClosestPort(n2);
	p2 = n2->getClosestPort(n1);

	if (!p1 || !p2)
		return;

	m_newEdge = new CDiagramEdge(m_d);
	m_d->insertItem(m_newEdge, m_newEdge->pos());

	m_newEdge->setStartPort(p1);
	m_newEdge->setEndPort(p2);
	p1->addEdge(m_newEdge);
	p2->addEdge(m_newEdge);
	m_newEdge->retrack();
}

void CDragNewItemCommand::addEdgeUndo()
{
	CDiagramPort	*p1 = NULL, *p2 = NULL;

	if (!m_item || !m_item2 || m_item->interType() != Global::Node || m_item2->interType() != Global::Node)
		return;
	
	if (!m_newEdge)
		return;

	p1 = m_newEdge->startPort();
	p2 = m_newEdge->endPort();
	if (!p1 || !p2)
		return;

	p1->removeEdge(m_newEdge);
	p2->removeEdge(m_newEdge);
	
	m_newEdge->setStartPort(NULL);
	m_newEdge->setEndPort(NULL);

	m_d->deleteItem(m_newEdge);
}

void CDragNewItemCommand::addNodeRedo()
{
	int						id;
	QString					ctg, name;
	QDomElement				element;
	QDomDocument			doc;
	CDiagramNode			*n1 = NULL, *n2 = NULL;
	CDiagramPort			*p1 = NULL, *p2 = NULL;
	CDiagramItemTypeCategory	*factory = NULL;

	if (!m_item || m_item->interType() != Global::Node)
		return;

	element = doc.createElement("item");
	m_item->toXml(element);
	ctg = element.attribute(QLatin1String("category"));
	name = element.attribute(QLatin1String("name"));
	factory = CDiagramItemTypeDatabase::instance()->getCategory(ctg);
	if (factory)
	{
		m_newItem = factory->createItem(name, Q_ARG(CDiagram*, m_d));
		if (m_newItem)
		{
			m_d->insertItem(m_newItem, m_newItem->pos());
			id = m_newItem->id();

			m_newItem->fromXml(element);
			m_newItem->setId(id);
			m_newItem->setPos( m_newPosition );

			m_newEdge = new CDiagramEdge(m_d);
			m_d->insertItem(m_newEdge, m_newEdge->pos());

			n1 = dynamic_cast<CDiagramNode*>(m_item);
			n2 = dynamic_cast<CDiagramNode*>(m_newItem);
			if (n1 && n2)
			{
				p1 = n1->getClosestPort(n2);
				p2 = n2->getClosestPort(n1);
				if (p1 && p2)
				{
					p1->addEdge(m_newEdge);
					p2->addEdge(m_newEdge);

					m_newEdge->setStartPort(p1);
					m_newEdge->setEndPort(p2);
					m_newEdge->retrack();

					m_d->emitItemsAdded(m_newItem);
				}
				else
				{
					m_d->deleteItem(m_newItem);
					m_d->deleteItem(m_newEdge);
					SAFE_DELETE(m_newItem);
					SAFE_DELETE(m_newEdge);
				}
			}
			else
			{
				m_d->deleteItem(m_newItem);
				m_d->deleteItem(m_newEdge);
				SAFE_DELETE(m_newItem);
				SAFE_DELETE(m_newEdge);
			}
		}
	}
}

void CDragNewItemCommand::addNodeUndo()
{
	CDiagramPort	*p1 = NULL, *p2 = NULL;

	if (!m_item || m_item->interType() != Global::Node)
		return;

	if (m_newEdge)
	{
		p1 = m_newEdge->startPort();
		p2 = m_newEdge->endPort();
		p1->removeEdge(m_newEdge);
		p2->removeEdge(m_newEdge);
		m_newEdge->setStartPort(NULL);
		m_newEdge->setEndPort(NULL);

		m_d->deleteItem(m_newEdge);
		SAFE_DELETE(m_newEdge);

		m_d->deleteItem(m_newItem);
		SAFE_DELETE(m_newItem);
	}
}

CAlignItemsCommand::CAlignItemsCommand(CDiagram *d, Qt::Alignment align, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_align(align)
{
	qreal			x1, y1, x2, y2;
	qreal			mx = 0, my = 0;
	qreal			t = 0;
	QPointF			p;
	QRectF			r;
	CDiagramItem	*item = NULL;

	foreach (item, m_d->selectedTopLevelItems())
	{
		if (item->interType() == Global::Node || item->interType() == Global::Group)
		{
			m_items.push_back(item);
			r = item->geometry();
			p = item->pos();
			if (m_oldPos.isEmpty())
			{
				x1 = x2 = p.x();
				y1 = y2 = p.y();
			}

			t = p.x() - r.width() / 2.0;
			if (t < x1) x1 = t;

			t = p.x() + r.width() / 2.0;
			if (t > x2) x2 = t;

			t = p.y() - r.height() / 2.0;
			if (t < y1) y1 = t;

			t = p.y() + r.height() / 2.0;
			if (t > y2) y2 = t;
			mx += p.x();
			my += p.y();

			m_oldPos.push_back(p);
		}
	}

	switch (m_align)
	{
	case Qt::AlignLeft:
		foreach (item, m_items) m_newPos.push_back( QPointF(x1 + item->boundingRect().width() / 2.0,  item->y()) );
		break;

	case Qt::AlignRight:
		foreach (item, m_items) m_newPos.push_back( QPointF( x2 - item->boundingRect().width() / 2.0, item->y()) );
		break;

	case Qt::AlignHCenter:
		foreach (item, m_items) m_newPos.push_back( QPointF( mx / m_items.length(), item->y() ) );
		break;

	case Qt::AlignTop:
		foreach (item, m_items) m_newPos.push_back( QPointF(item->x(), y1 + item->boundingRect().height() / 2.0) );
		break;

	case Qt::AlignBottom:
		foreach (item, m_items) m_newPos.push_back( QPointF(item->x(), y2 - item->boundingRect().height() / 2.0) );
		break;

	case Qt::AlignVCenter:
		foreach (item, m_items) m_newPos.push_back( QPointF(item->x(), my / m_items.length() ) );
		break;
	
	default:
		break;
	};
}

void CAlignItemsCommand::redo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_newPos.length(); ++i)
	{
		item = m_items.at(i);
		item->setPos(m_newPos.at(i));
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
}

void CAlignItemsCommand::undo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_oldPos.length(); ++i)
	{
		item = m_items.at(i);
		item->setPos(m_oldPos.at(i));
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
}


CEqualItemsSizeCommand::CEqualItemsSizeCommand(CDiagram *d, Global::MatchSizeFlag flag, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_matchSizeflag(flag)
{
	bool	resizable = true;
	QSizeF	s;

	m_minH = m_minW = 16777215;
	m_maxH = m_maxW = 0;

	foreach (CDiagramItem *item, m_d->selectedTopLevelItems())
	{
		if (item->interType() == Global::Node || item->interType() == Global::Group)
		{
			m_items.push_back(item);
			s = item->size();
			m_oldSize.push_back(s);
			m_minH = qMin(s.height(), m_minH);
			m_maxH = qMax(s.height(), m_maxH);
			m_minW = qMin(s.width(), m_minW);
			m_maxW = qMax(s.width(), m_maxW);
			if (!item->resizable()) 
			{
				resizable = false;
				break;
			}
		}
	}

	if (!resizable)
	{
		m_items.clear();
		m_oldSize.clear();
	}
}

void CEqualItemsSizeCommand::redo()
{
	QSizeF			s;
	qreal			xratio, yratio, ratio;
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item->keepSizeRatio())
		{
			if (m_matchSizeflag == Global::MatchMinimumHeight)		ratio = m_minH / item->height();
			else if (m_matchSizeflag == Global::MatchMaximumHeight)	ratio = m_maxH / item->height();
			else if (m_matchSizeflag == Global::MatchMinimumWidth)	ratio = m_minW / item->width();
			else ratio = m_maxW / item->width();
			s = QSizeF(item->width() * ratio, item->height() * ratio);
		}
		else
		{
			yratio = 1.0;
			xratio = 1.0;
			if (m_matchSizeflag == Global::MatchMinimumHeight)			yratio = m_minH / item->height();
			else if (m_matchSizeflag == Global::MatchMaximumHeight)		yratio = m_maxH / item->height();
			else if (m_matchSizeflag == Global::MatchMinimumWidth)		xratio = m_minW / item->width();
			else xratio = m_maxW / item->width();
			s = QSizeF(item->width() * xratio, item->height() * yratio);
		}

		item->resize(s.width(), s.height());
		
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
}

void CEqualItemsSizeCommand::undo()
{
	QSizeF			size;
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_oldSize.length(); ++i)
	{
		item = m_items.at(i);
		size = m_oldSize.at(i);
		item->resize(size.width(), size.height());
		if (item->interType() == Global::Node)
		{
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
		}
	}
}

CLayoutItemsCommand::CLayoutItemsCommand(CDiagram *d, QList<CDiagramItem*> items, int mode)
	: CDiagramCommand(d),
	  m_mode(mode)
{
	CDiagramItem	*item = NULL;

	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (item && (item->interType() == Global::Node || item->interType() == Global::Group))
		{
			m_items.push_back(item);
			m_pos.push_back(item->pos());
		}
	}
}

void CLayoutItemsCommand::redo()
{
	CDiagramItem	*item = NULL;
	CDiagramNode	*node = NULL;
	QRectF			r1, r2;
	qreal			dist = 0;
	qreal			length = 0, widthSum = 0, heightSum = 0;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		if (item)
		{
			r2 = item->boundingRect();
			r2 = item->mapRectToScene(r2);
			r1 = r1.united(r2);
			widthSum +=  r2.width();
			heightSum += r2.height();
		}
	}
	
	switch(m_mode)
	{
	case Qt::Horizontal:
		qStableSort(m_items.begin(), m_items.end(), xcoordLessThan);
		length = r1.width() - widthSum;
		dist = length / (m_items.length() - 1);

		// relayout
		r1 = m_items.at(0)->boundingRect();
		for (int i = 1; i < m_items.length(); ++i)
		{
			item = m_items.at(i);
			r2 = item->boundingRect();
			item->setPos( m_items.at(i-1)->x() + r1.width() / 2.0 + dist + r2.width() / 2.0, item->y() );
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
			r1 = r2;
		}
		break;

	case Qt::Vertical:
		qStableSort(m_items.begin(), m_items.end(), ycoordLessThan);
		length = r1.height() - heightSum;
		dist = length / (m_items.length() - 1);

		// relayout
		r1 = m_items.at(0)->boundingRect();
		for (int i = 1; i < m_items.length(); ++i)
		{
			item = m_items.at(i);
			r2 = item->boundingRect();
			item->setPos( item->x(), m_items.at(i-1)->y() + r1.height() / 2.0 + dist + r2.height() / 2.0 );
			node = dynamic_cast<CDiagramNode*>(item);
			if (node)
				node->retrack();
			r1 = r2;
		}
		break;

	default:
		break;

	}; // End of switch
}

void CLayoutItemsCommand::undo()
{
	CDiagramNode	*node = NULL;
	CDiagramItem	*item = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		item->setPos( m_pos.at(i) );
		node = dynamic_cast<CDiagramNode*>(item);
		if (node)
			node->retrack();
	}
}

CEditTextCommand::CEditTextCommand(
	CDiagram *d, 
	CDiagramItem *item, 
	CDiagramText *text, 
	const QString &str, 
	const QRectF &rect, 
	QUndoCommand *parent)
	: CDiagramCommand(d),
	m_item(item),
	m_text(text),
	m_oldText(str),
	m_oldRect(rect),
	m_newRect(item->boundingRect()),
	m_newText(text->toHtml()),
	m_first(true)
{
}

void CEditTextCommand::redo()
{
	if (m_first)
	{
		m_first = false;
		return;
	}
	m_text->setHtml(m_newText);
	m_item->resize(m_newRect.size());
}

void CEditTextCommand::undo()
{
	m_text->setHtml(m_oldText);
	m_item->resize(m_oldRect.size());
}

CSendToFrontCommand::CSendToFrontCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent)
	:CDiagramCommand(d),
	 m_items(items)
{

}

void CSendToFrontCommand::redo()
{
	/*
	QList<CDiagramItem*>	items;

	items = m_d->topLevelItems(item);
	if (items.length() > 0)
	{
		items.last()->stackBefore(item);
	}
	*/
}

void CSendToFrontCommand::undo()
{
	
}

CSendToBackCommand::CSendToBackCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent)
	:CDiagramCommand(d),
	 m_items(items)
{

}

void CSendToBackCommand::redo()
{
	/*
	QList<CDiagramItem*>	items;

	items = m_d->topLevelItems(item);
	if (items.length() > 0)
	{
		for (int i = 0; i < m_items.length(); ++i)
		{
			
		}

	}
	*/
}

void CSendToBackCommand::undo()
{
	
}

CBringForwardsCommand::CBringForwardsCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent)
	:CDiagramCommand(d),
	 m_items(items)
{

}

void CBringForwardsCommand::redo()
{

}

void CBringForwardsCommand::undo()
{

}

CBringBackwardsCommand::CBringBackwardsCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent)
	:CDiagramCommand(d),
	 m_items(items)
{

}

void CBringBackwardsCommand::redo()
{

}

void CBringBackwardsCommand::undo()
{

}

CChangeLabelPositionCommand::CChangeLabelPositionCommand(CDiagram *d, CDiagramEdge *edge, int label, qreal pos, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_edge(edge),
	  m_label(label),
	  m_newPos(pos)
{
	if (edge)
		m_oldPos = edge->labelPosition(m_label);
}

void CChangeLabelPositionCommand::redo()
{
	if (m_edge)
		m_edge->setLabelPosition(m_label, m_newPos);
}

void CChangeLabelPositionCommand::undo()
{
	if (m_edge)
		m_edge->setLabelPosition(m_label, m_oldPos);
}

bool CChangeLabelPositionCommand::mergeWith(const QUndoCommand *cmd)
{
	if (cmd->id() != CMD_SPRITE_LABELPOS)
		return false;

	const CChangeLabelPositionCommand *c = dynamic_cast<const CChangeLabelPositionCommand*>(cmd);
	if (!c) return false;
	if ( c->m_edge != m_edge ) return false;
	if ( c->m_label != m_label) return false;
	m_newPos = c->m_newPos;
	return true;
}

int CChangeLabelPositionCommand::id() const
{
	return CMD_SPRITE_LABELPOS;
}

CChangeLabelAnchorCommand::CChangeLabelAnchorCommand(CDiagram *d, QList<CDiagramItem*> items, int anchor, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_newAnchor(anchor)
{
	CDiagramNode	*node = NULL;

	for (int i = 0; i < items.length(); ++i)
	{
		node = dynamic_cast<CDiagramNode*>(items.at(i));
		if (node)
		{
			m_items.push_back(node);
			m_oldAnchors.push_back(node->labelPosition());
		}
	}
}

void CChangeLabelAnchorCommand::redo()
{
	CDiagramNode	*node = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		node = dynamic_cast<CDiagramNode*>( m_items.at(i) );
		if (node)
		{
			node->setLabelPosition(m_newAnchor);
			node->layoutContent();
		}
	}
}

void CChangeLabelAnchorCommand::undo()
{
	CDiagramNode	*node = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		node = dynamic_cast<CDiagramNode*>( m_items.at(i) );
		if (node)
		{
			node->setLabelPosition(m_oldAnchors.at(i));
			node->layoutContent();
		}
	}
}

CChangeConnectPointCommand::CChangeConnectPointCommand(CDiagram *d, CDiagramPort *port, CDiagramEdge *edge, qreal pos, QUndoCommand *parent)
: CDiagramCommand(d),
  m_port(port),
  m_edge(edge),
  m_newPos(pos)
{
	if (port && edge)
		m_oldPos = port->getConnectPosition(edge);
}

void CChangeConnectPointCommand::redo()
{
	if (m_port && m_edge)
	{
		m_port->setConnectPosition(m_edge, m_newPos);
		if (m_edge->startPort() == m_port)
			m_edge->setStartPercent(m_newPos);
		else
			m_edge->setEndPercent(m_newPos);
	}
}

void CChangeConnectPointCommand::undo()
{
	if (m_port && m_edge)
	{
		m_port->setConnectPosition(m_edge, m_oldPos);
		if (m_edge->startPort() == m_port)
			m_edge->setStartPercent(m_oldPos);
		else
			m_edge->setEndPercent(m_oldPos);
	}
}

bool CChangeConnectPointCommand::mergeWith(const QUndoCommand *cmd)
{
	if (cmd->id() != CMD_EDGE_CONNPOINT)
		return false;

	const CChangeConnectPointCommand *c = dynamic_cast<const CChangeConnectPointCommand*>(cmd);
	if (!c) return false;
	if ( c->m_edge != m_edge ) return false;
	if ( c->m_port != m_port) return false;
	m_newPos = c->m_newPos;
	return true;
}

int CChangeConnectPointCommand::id() const
{
	return CMD_EDGE_CONNPOINT;
}

CReplaceSymbolCommand::CReplaceSymbolCommand(CDiagram *d, CDiagramItem *oldItem, CDiagramItem *newItem, QUndoCommand *parent)
	:CDiagramCommand(d),
	 m_oldItem(oldItem),
	 m_newItem(newItem)
{
}

void CReplaceSymbolCommand::redo()
{
	CDiagramNode			*n1 = NULL, *n2 = NULL;
	CDiagramPort			*p1 = NULL, *p2 = NULL;
	CDiagramEdge			*edge = NULL;
	QList<CDiagramEdge*>	edges;
	QList<CDiagramPort*>	ports;

	n1 = dynamic_cast<CDiagramNode*>(m_oldItem);
	n2 = dynamic_cast<CDiagramNode*>(m_newItem);
	if (!n1 || !n2)
		return;

	m_d->insertItem(n2, n1->pos());
	for (int i = Global::North; i <= Global::East; ++i)
	{
		p1 = n1->getPortsBySide(i).first();
		p2 = n2->getPortsBySide(i).first();
		edges = p1->getEdges();
		for (int j = 0; j < edges.length(); ++j)
		{
			edge = edges.at(j);
			p1->removeEdge(edge);
			p2->addEdge(edge);
			if (p1 == edge->startPort())
				edge->setStartPort(p2);
			else
				edge->setEndPort(p2);
			edge->retrack();
		}
	}
	m_d->deleteItem(n1);
	n2->setSelected(true);
}

void CReplaceSymbolCommand::undo()
{
	CDiagramNode			*n1 = NULL, *n2 = NULL;
	CDiagramPort			*p1 = NULL, *p2 = NULL;
	CDiagramEdge			*edge = NULL;
	QList<CDiagramEdge*>	edges;
	QList<CDiagramPort*>	ports;
	
	n1 = dynamic_cast<CDiagramNode*>(m_oldItem);
	n2 = dynamic_cast<CDiagramNode*>(m_newItem);
	if (!n1 || !n2)
		return;

	m_d->insertItem(n1, n1->pos());
	for (int i = Global::North; i <= Global::East; ++i)
	{
		ports = n1->getPortsBySide(i);
		if (ports.length() > 0)
			p2 = ports.first();

		ports = n2->getPortsBySide(i);
		if (ports.length() > 0)
			p1 = ports.first();

		if (p1)
			edges = p1->getEdges();

		for (int j = 0; j < edges.length(); ++j)
		{
			edge = edges.at(j);
			p1->removeEdge(edge);
			if (p2)
				p2->addEdge(edge);

			if (p1 == edge->startPort())
				edge->setStartPort(p2);
			else
				edge->setEndPort(p2);
			edge->retrack();
		}

		edges.clear();
	}
	m_d->deleteItem(n2);
	n1->setSelected(true);
}
