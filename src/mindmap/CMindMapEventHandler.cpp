/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramCommand>
#include <core/CDiagramCommands>
#include <core/CDiagramEventHandler>

#include "CMindMapNode.h"
#include "CMindMapCommands.h"
#include "CMindMapEventHandler.h"

#define DND_DRAGMINDMAPNODE "application/x-dnddragmindmapnode"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);
extern void selectAllSubNodes(CMindMapNode *node);

// A line path used antialiasing.
class CLineIndicator: public QGraphicsLineItem
{
public:
	CLineIndicator(QGraphicsItem *parent = NULL)
		: QGraphicsLineItem(parent)
	{
	}

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
	{
		if (painter)
		{
			painter->setRenderHint(QPainter::Antialiasing, true);
			QGraphicsLineItem::paint(painter, option, widget);
		}
	}
}; // End of class CLineIndicator

CMindMapDragImage::CMindMapDragImage()
    : QGraphicsItem(),
      m_pixmap(NULL)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
}

QRectF CMindMapDragImage::boundingRect() const
{
	qreal	w, h;

	w = m_rect.width();
	h = m_rect.height();
	return QRectF(-w / 2.0, -h / 2.0, w, h);
}

QPainterPath CMindMapDragImage::shape() const
{
	QPainterPath	path;
	qreal			w, h;

	w = m_rect.width();
	h = m_rect.height();

	path.addRect( QRectF(-w / 2.0, -h / 2.0, w, h) );
	return 	path;
}

void CMindMapDragImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QRectF	r;

    r = boundingRect();
	painter->save();
	painter->drawPixmap(r, *m_pixmap, QRectF(1, 1, r.width(), r.height()) );
	painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect( r );
	painter->restore();
}

void CMindMapDragImage::updateDraggedItemsRect(CDiagram *d, QGraphicsItem *item)
{
	QRectF					rect;
	QPainter				painter;
    CMindMapNode			*node = NULL;

	node = dynamic_cast<CMindMapNode*>(item);
	if (!node)
		return;

	if (m_pixmap)
		delete m_pixmap;

	rect = node->contentRect();
	rect = QRectF(node->mapToScene(rect.topLeft()), QSizeF(rect.width(), rect.height()));
	m_pixmap = new QPixmap(rect.width(), rect.height());
	m_pixmap->fill(Qt::transparent);
	
	node->setOpacity(0.5);
	
	painter.begin(m_pixmap);
	d->drawArea(&painter, rect);
	painter.end();

	node->setOpacity(1.0);

	m_rect = rect;
	setPos(rect.center());
	update();
}

CMindMapEventHandler::CMindMapEventHandler()
	: CDiagramEventHandler(),
	  m_draggedImage(NULL),
	  m_pdata(NULL),
	  m_areaSize(180),
	  m_indicatorLine(NULL),
	  m_indicatorRect(NULL),
      m_target(NULL),
      m_tempNode(NULL)
{
    m_name = "MindMapEventHandler";

	m_priority = 1.0;

	m_draggedImage = new CMindMapDragImage();
	m_indicatorRect = new QGraphicsRectItem();
	m_indicatorRect->setPen(QPen(Qt::red, 2, Qt::SolidLine));
	m_indicatorRect->setBrush(Qt::NoBrush);
	m_indicatorLine = new CLineIndicator();
	m_indicatorLine->setPen(QPen(Qt::red, 2, Qt::DashLine));

    m_tempNode = new CMindMapNode(NULL);
    m_tempNode->setText(QObject::tr("......"));
    m_tempNode->setBrush(Qt::transparent);
    m_tempNode->setPen( QPen(Qt::transparent) );
    m_tempNode->setCustomPen(true);
    m_tempNode->setNodeType( CMindMapNode::RoundedRect );
}

CMindMapEventHandler::~CMindMapEventHandler()
{
    SAFE_DELETE(m_draggedImage);
    SAFE_DELETE(m_indicatorLine);
	SAFE_DELETE(m_indicatorRect);
}

bool CMindMapEventHandler::onEvent(CDiagramPrivateData *p, QEvent *evt)
{
	bool					r = false;
	CDiagram				*d = NULL;
	CDiagramItem			*item = NULL;
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	
	if ( !p || !p->m_d || p->m_d->mode() == CDiagram::EditTextMode )
		return r;

	d = p->m_d;
	m_pdata = p;

	switch(evt->type())
	{
	case QEvent::GraphicsSceneDragEnter:
        r = dragEnterEvent(static_cast<QGraphicsSceneDragDropEvent*>(evt));
		break;

	case QEvent::GraphicsSceneDragMove:
        r = dragMoveEvent(static_cast<QGraphicsSceneDragDropEvent*>(evt));
		break;

	case QEvent::GraphicsSceneDrop:
        r = dropEvent(static_cast<QGraphicsSceneDragDropEvent*>(evt));
		break;

	case QEvent::GraphicsSceneMousePress:
        if (m_draggedImage->scene() == d)
            d->removeItem(m_draggedImage);
		m_pdata->m_customPressedItem = NULL;
		m_target = NULL;
		break;

	case QEvent::GraphicsSceneMouseMove:
		{
			QGraphicsSceneMouseEvent	*event = NULL;
			QPointF						endPos;
			QPointF						startPos;
			QDrag						*drag = NULL;
			QMimeData					*mime = NULL;
			QByteArray					mimeData;
			QDataStream					dataStream(&mimeData, QIODevice::WriteOnly);

			event = static_cast<QGraphicsSceneMouseEvent*>(evt);
			if (event && d->mode() == CDiagram::CursorMode)
			{
				if (event->buttons() & Qt::LeftButton )
				{
					endPos = event->scenePos();
					startPos = event->buttonDownScenePos(Qt::LeftButton);

					if (!m_pdata->m_customPressedItem)
					{
						items = d->selectedTopLevelItems();
						if (items.length() == 1)
						{
							node = dynamic_cast<CMindMapNode*>(items.first());
                            if (node && node->isMovable() && node->mapToScene( node->boundingRect() ).containsPoint(event->scenePos(), Qt::WindingFill ) )
							{
								m_pdata->m_customPressedItem = node;
							}
							else if (node && node->isRoot())
							{
								r = true;
							}
						}
						else
						{
							foreach (item, items)
							{
								node = dynamic_cast<CMindMapNode*>(item);
								if (node && node->isRoot())
								{
									r = true;
									break;
								}
							}
						}
					}

					// check whether we should start the moving action
					// using manhattanLength.
					if (m_pdata->m_customPressedItem && 
						m_draggedImage->scene() != d && 
						((startPos, endPos).manhattanLength() > QApplication::startDragDistance() ) )
					{
						((CMindMapDragImage*)m_draggedImage)->updateDraggedItemsRect(d, m_pdata->m_customPressedItem);
                        m_pdata->m_d->addItem(m_draggedImage);
						m_draggedImage->setVisible(true);
						m_draggedImageInitPos = m_draggedImage->pos();
						
						dataStream << event->buttonDownScenePos(Qt::LeftButton);
						mime = new QMimeData();
						mime->setData(DND_DRAGMINDMAPNODE, mimeData);

                        drag = new QDrag( d->getMainView() );
						drag->setMimeData(mime);
						
						if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction) == Qt::MoveAction)
						{
							// Should do something here.
						}

						event->accept();
                        r = true;
					}
				}
			}
		}

		break;

	case QEvent::GraphicsSceneMouseRelease:
		if (m_pdata->m_customPressedItem && m_indicatorLine->scene() == m_pdata->m_d)
		{
			m_pdata->m_d->removeItem(m_indicatorLine);
			m_pdata->m_d->removeItem(m_indicatorRect);
		}
		m_pdata->m_customPressedItem = NULL;
		m_target = NULL;
		break;

	case QEvent::KeyPress:
		r = keyPressEvent(dynamic_cast<QKeyEvent*>(evt));
		break;

	default:
		break;
	}; // End of switch

	return r;
}

bool CMindMapEventHandler::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	bool				r = false;
	const QMimeData		*mime = NULL;

	mime = event->mimeData();
    if (mime->hasFormat(DND_DRAGMINDMAPNODE))
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();

		r = true;
	}

	return r;
}

bool CMindMapEventHandler::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData			*mime = event->mimeData();
	bool					r = false;
    int                     index = -1, tmpIndex = -1;
    qreal					threshold = 32;
	QRectF					rect;
	QPointF					startPos, endPos, tmpPos, targetPos;
	CMindMapNode			*draggedNode = NULL, *tmpNode = NULL, *attachedNode = NULL;
	QList<QGraphicsItem*>	items;

	endPos = event->scenePos();

	// drag items around
	if (mime->hasFormat(DND_DRAGMINDMAPNODE))
	{
		QByteArray			itemData = mime->data(DND_DRAGMINDMAPNODE);
		QDataStream			dataStream(&itemData, QIODevice::ReadOnly);

		dataStream >> startPos;
		
		tmpPos = m_draggedImageInitPos + endPos - startPos;
		draggedNode = dynamic_cast<CMindMapNode*>(m_pdata->m_customPressedItem);
		if (draggedNode && 
			!draggedNode->isRoot() && 
			QLineF(tmpPos, draggedNode->mapToScene(QPointF(0, 0))).length() > threshold)
		{
            rect = draggedNode->contentRect();
            rect.moveCenter(tmpPos);

            // detect the intersect item
            items = m_pdata->m_d->items(tmpPos, Qt::ContainsItemShape, Qt::DescendingOrder);
            //items = m_pdata->m_d->items(rect, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
			for (int i = 0; i < items.length(); ++i)
			{
                CMindMapNodeChildPanel  *panel = NULL;

                panel = dynamic_cast<CMindMapNodeChildPanel*>(items.at(i));
                if (!panel || panel->node() == m_tempNode)
                    continue;

                tmpNode = panel->node();
				if (tmpNode && 
                    tmpNode != m_pdata->m_customPressedItem &&
					!tmpNode->isDescendantOf(draggedNode))
				{
                    attachedNode = tmpNode;
                    break;
				}
			}

            // the node is founded, then we insert the temp node to
            // identify the insert position

            // first, we clear things up if the new target is different
            // from the previous one
            if (m_target && m_target != attachedNode)
            {
                m_target->dropChild(m_tempNode);
                m_pdata->m_d->deleteItem(m_tempNode);
            }

            if (attachedNode)
            {
                // if the new attached node is new, then we have to
                // setup the temp node for it
                if (m_target != attachedNode)
                {
                    m_pdata->m_d->insertItem(m_tempNode, tmpPos);
                    m_tempNode->setVisible(true);
                    index = attachedNode->connectIndex(tmpPos);
                    attachedNode->addChild(m_tempNode, index);
                }
                else // or we have to update the index
                {
                    // only when the index is different, we have to renew it
                    index = m_target->connectIndex(tmpPos);
                    tmpIndex = m_target->indexOf(m_tempNode);
                    if (index > tmpIndex)
                        --index;

                    if (tmpIndex != index)
                    {
                        m_target->dropChild(m_tempNode);
                        m_target->addChild(m_tempNode, index);
                    }
                }

                targetPos = attachedNode->mapToScene( QPointF(0, 0) );
                rect = attachedNode->mapRectToScene( attachedNode->contentRect() );
				if (m_indicatorLine->scene() != m_pdata->m_d)
				{
					m_pdata->m_d->addItem(m_indicatorLine);
					m_pdata->m_d->addItem(m_indicatorRect);
				}
				m_indicatorLine->setVisible(true);
				m_indicatorLine->setLine(tmpPos.x(), tmpPos.y(), targetPos.x(), targetPos.y());
				m_indicatorRect->setVisible(true);
				m_indicatorRect->setRect( rect );
			}
			else
			{
				if (m_pdata->m_d == m_indicatorLine->scene())
				{
					m_pdata->m_d->removeItem(m_indicatorLine);
					m_pdata->m_d->removeItem(m_indicatorRect);
                }
			}

			m_target = attachedNode;
		}
		else
		{
            if (m_target)
            {
                m_target->dropChild(m_tempNode);
                m_pdata->m_d->deleteItem(m_tempNode);
                m_target = NULL;
            }

			if (m_pdata->m_d == m_indicatorLine->scene())
			{
				m_pdata->m_d->removeItem(m_indicatorLine);
				m_pdata->m_d->removeItem(m_indicatorRect);
			}
		}
		
        m_draggedImage->setPos( tmpPos );
		r = true;
	}

	return r;
}

bool CMindMapEventHandler::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData			*mime = NULL;
	bool					r = false;
    int                     newIndex = -1, oldIndex = -1;
	CDiagram				*d = NULL;
    QPointF					startPos, endPos;
    CMindMapNode			*draggedNode = NULL;

	d = m_pdata->m_d;
	mime = event->mimeData();
	if (mime->hasFormat(DND_DRAGMINDMAPNODE))
	{
		startPos = m_draggedImageInitPos;
		endPos = m_draggedImage->pos();

		// different handlers
		draggedNode = dynamic_cast<CMindMapNode*>(m_pdata->m_customPressedItem);
		if (draggedNode)
		{
			// move the root node
			if (draggedNode->isRoot())
			{
				//
			}
            else if (m_target)
			{
                newIndex = m_target->indexOf(m_tempNode);
                oldIndex = m_target->indexOf(draggedNode);
                if (oldIndex > newIndex)
                    --oldIndex;
                else
                    --newIndex;
                m_target->dropChild(m_tempNode);
                if (m_target != draggedNode->upLevelNode())
				{
                    m_pdata->m_d->addUndoCommand( new CMindMapReparentCommand(m_pdata->m_d, draggedNode, m_target, newIndex, 0) );
				}
                else
                {
                    if (oldIndex != newIndex)
                    {
                        m_pdata->m_d->addUndoCommand( new CMindMapReorderCommand(m_pdata->m_d, draggedNode, oldIndex, newIndex, 0) );
                    }
                }
			}
			draggedNode->setSelected(true);
			d->emitItemsDraggingStop();
		}
		
		// clear things up
        m_pdata->m_customPressedItem = NULL;
		if (m_pdata->m_d == m_draggedImage->scene())
			m_pdata->m_d->removeItem(m_draggedImage);
		if (m_pdata->m_d == m_indicatorLine->scene())
			m_pdata->m_d->removeItem(m_indicatorLine);
		if (m_pdata->m_d == m_indicatorRect->scene())
			m_pdata->m_d->removeItem(m_indicatorRect);
        if (m_pdata->m_d == m_tempNode->scene())
            m_pdata->m_d->deleteItem(m_tempNode);

		m_indicatorLine->update();
		m_indicatorRect->update();
		m_target = NULL;

		event->setDropAction(Qt::MoveAction);
		event->acceptProposedAction();
		event->accept();

		r = true;
	}

	return r;
}

bool CMindMapEventHandler::keyPressEvent(QKeyEvent *event)
{
    bool					r = false, mark = false;
	qreal					delta = 320, minAngle = 45, minDist = -1;
	QRectF					rect;
    CMindMapNode			*node = NULL;
	CMindMapNode			*tmpNode = NULL, *nextNode = NULL;
	QList<CMindMapNode*>	nodes;
	QList<CDiagramItem*>	items;
	QList<QGraphicsItem*>	aroundItems;
	QMap<int, QLineF>		maps;

	maps[Qt::Key_Left]  = QLineF(0, 0, -1, 0);
	maps[Qt::Key_Right] = QLineF(0, 0, 1, 0);
	maps[Qt::Key_Up]	= QLineF(0, 0, 0, -1);
	maps[Qt::Key_Down]  = QLineF(0, 0, 0, 1);

	if (event)
	{
		switch(event->key())
		{
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Up:
		case Qt::Key_Down:
			items = m_pdata->m_d->selectedTopLevelItems();
			if (items.length() == 1)
			{
				node = dynamic_cast<CMindMapNode*>(items.first());
				if (node)
				{
					rect = node->mapToScene(node->boundingRect()).boundingRect();
					rect.adjust(-delta, -delta, delta, delta);

					aroundItems = m_pdata->m_d->items(rect, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder);
					foreach (QGraphicsItem *tmpItem, aroundItems)
					{
						tmpNode = dynamic_cast<CMindMapNode*>(tmpItem);
						if (tmpNode && tmpNode != node)
							nodes.push_back( tmpNode );
					}

					foreach (tmpNode, nodes)
					{
						qreal	angle = 0;
						qreal	dist = 0;
						QLineF	line(QPointF(0, 0), tmpNode->mapToItem(node, QPointF(0, 0)));

						angle = line.angleTo( maps[event->key()] );
						angle = angle > 180 ? 360 - angle : angle;
						dist = line.length();

						if (angle < minAngle)
						{
							if (minDist < 0 || dist < minDist)
							{
								nextNode = tmpNode;
								minDist = dist;
							}
						}
					}

					if (nextNode)
					{
						m_pdata->m_d->clearSelection();
						nextNode->setSelected(true);
					}
					r = true;
				}
			}

			break;

        case Qt::Key_Backspace:
		case Qt::Key_Delete:
			items = m_pdata->m_d->selectedTopLevelItems();
			foreach(CDiagramItem *item, items)
			{
				node = dynamic_cast<CMindMapNode*>(item);
                if (node)
                {
                    if (node->hasFocus())
                        nodes.push_back(node);
                    else
                        mark = true;
                }
			}

            if (nodes.length() == 0 && mark)
                r = true;
            else
                m_pdata->m_d->addUndoCommand(new CMindMapDelNodeCommand(m_pdata->m_d, nodes, 0) );
			break;

        case Qt::Key_Tab:
			// used to enable creation new node when press tab key
			items = m_pdata->m_d->selectedTopLevelItems();
			if (items.length() == 1)
			{
				node = dynamic_cast<CMindMapNode*>(items.first());
				if (node)
                {
                    m_pdata->m_d->addUndoCommand(new CMindMapNewNodeCommand(m_pdata->m_d,
                                                                            node,
                                                                            NULL,
                                                                            false,
                                                                            (event->modifiers() & Qt::Key_Alt)));
					r = true;
				}
			}
			break;
		
		case Qt::Key_G:
			if (event->modifiers() & Qt::ControlModifier)
			{
				items = m_pdata->m_d->selectedTopLevelItems();
				foreach (CDiagramItem *item, items)
				{
					node = dynamic_cast<CMindMapNode*>(item);
					if (node)
						node->setSelected(false);
				}
			}
			break;
		
        case Qt::Key_C:
            if (event->modifiers() & Qt::ControlModifier)
            {
                nodes.clear();
                items = m_pdata->m_d->selectedTopLevelItems();
                foreach (CDiagramItem *item, items)
                {
                    node = dynamic_cast<CMindMapNode*>(item);
                    if (node && !node->isRoot())
                        nodes.push_back(node);
                    else
                        item->setSelected(false);
                }

                if (nodes.length() > 0)
                {
                    m_pdata->m_d->copy();
                    r = true;
                }
            }
            break;

        case Qt::Key_V:
            if (event->modifiers() & Qt::ControlModifier)
            {
                nodes = getAllSelectedNodes(m_pdata->m_d);
                if (nodes.length() > 0)
                {
                    m_pdata->m_d->addUndoCommand( new CMindMapPasteCommand(m_pdata->m_d,
                                                                           nodes,
                                                                           NULL) );
                }
            }

            r = true;
			break;

		case Qt::Key_X: // cut
			if (event->modifiers() & Qt::ControlModifier)
			{
                nodes.clear();
                items = m_pdata->m_d->selectedTopLevelItems();
                foreach (CDiagramItem *item, items)
                {
                    node = dynamic_cast<CMindMapNode*>(item);
                    if (node && !node->isRoot())
                        nodes.push_back(node);
                    else
                        item->setSelected(false);
                }

                if (nodes.length() > 0)
                {
                    foreach (node, nodes)
                    {
                        selectAllSubNodes(node);
                    }

                    m_pdata->m_d->copy();
                    m_pdata->m_d->addUndoCommand( new CMindMapDelNodeCommand(m_pdata->m_d,
                                                                             nodes,
                                                                             NULL));
                    r = true;
                }
			}
			break;

		default:
			break;

		}; // End of switch
	}

	return r;
}
