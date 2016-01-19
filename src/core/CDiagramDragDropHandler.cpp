/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <QSvgRenderer>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramCommands>
#include <core/CDiagramItemFormat>
#include <core/CDiagramLibraryModel>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramDragDropHandler>

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

class DragImage: public QGraphicsItem
{
public:
	DragImage();
	virtual ~DragImage() {}

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void updateDraggedItemsRect(CDiagram *d, const QRectF &rect);

protected:
	QRectF				m_rect;
	QPixmap				*m_pixmap;

}; // End of class DragImage

DragImage::DragImage()
	: m_pixmap(NULL)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
}

QRectF DragImage::boundingRect() const
{
	qreal	w, h;

	w = m_rect.width();
	h = m_rect.height();
	return QRectF(-w / 2.0, -h / 2.0, w, h);
}

QPainterPath DragImage::shape() const
{
	QPainterPath	path;
	qreal			w, h;

	w = m_rect.width();
	h = m_rect.height();

	path.addRect( QRectF(-w / 2.0, -h / 2.0, w, h) );
	return 	path;
}

void DragImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

void DragImage::updateDraggedItemsRect(CDiagram *d, const QRectF &rect)
{
	QPainter				painter;
	QList<CDiagramItem*>	items;

	if (m_pixmap)
		delete m_pixmap;

	m_pixmap = new QPixmap(rect.width(), rect.height());
	m_pixmap->fill(Qt::transparent);
	
	items = d->selectedTopLevelItems();
	for (int i = 0; i < items.length(); ++i)
	{
		items.at(i)->setOpacity(0.5);
	}
	
	painter.begin(m_pixmap);
	d->drawSelectedItems(&painter, QRectF(0, 0, rect.width(), rect.height()));
	painter.end();

	for (int i = 0; i < items.length(); ++i)
	{
		items.at(i)->setOpacity(1.0);
	}

	m_rect = rect;
	setPos(rect.center());
	update();
}

CDiagramDragDropHandler::CDiagramDragDropHandler()
	: CDiagramEventHandler(),
	  m_draggedImage(NULL),
	  m_pdata(NULL),
	  m_lineIndicator(NULL),
	  m_rectIndicator(NULL),
	  m_anchorXLine(NULL),
	  m_anchorYLine(NULL)
{
    m_name = "DiagramDragDropHandler";

	m_draggedImage = new DragImage();

	m_lineIndicator = new CLineIndicator(NULL);
	m_lineIndicator->setFlag(QGraphicsItem::ItemIsSelectable, false);

	m_rectIndicator = new QGraphicsRectItem();
	m_rectIndicator->setPen( QPen(Qt::red, 2) );
	m_rectIndicator->setFlag(QGraphicsItem::ItemIsSelectable, false);

	m_anchorXLine = new QGraphicsLineItem();
	m_anchorXLine->setFlag(QGraphicsItem::ItemIsSelectable, false);
	m_anchorXLine->setPen(QPen(Qt::darkGray, 1));

	m_anchorYLine = new QGraphicsLineItem();
	m_anchorYLine->setFlag(QGraphicsItem::ItemIsSelectable, false);
	m_anchorYLine->setPen(QPen(Qt::darkGray, 1));
}

CDiagramDragDropHandler::~CDiagramDragDropHandler()
{
	SAFE_DELETE(m_draggedImage);
	SAFE_DELETE(m_lineIndicator);
	SAFE_DELETE(m_rectIndicator);
	SAFE_DELETE(m_anchorXLine);
	SAFE_DELETE(m_anchorYLine);
}

bool CDiagramDragDropHandler::onEvent(CDiagramPrivateData *p, QEvent *evt)
{
	bool		r = false;
	CDiagram	*d = NULL;
	
	if (!p || !evt)
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
		m_pdata->m_pressedItem = NULL;
		m_pdata->m_draggedItems.clear();
		break;

	case QEvent::GraphicsSceneMouseMove:
		{
			CDiagramItem				*item = NULL;
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
				item = d->itemUnderPressed();
				endPos = event->scenePos();
				startPos = event->buttonDownScenePos(Qt::LeftButton);
				if (item && (event->buttons() & Qt::LeftButton) )
				{
					// start moving the items, record the items being moved.
					if (p->m_draggedItems.isEmpty())
						p->m_draggedItems = d->selectedTopLevelItems();

					// check whether we should start the moving action
					// using manhattanLength.mm
					if (m_draggedImage->scene() != d && ((startPos, endPos).manhattanLength() > QApplication::startDragDistance() ) )
					{
						((DragImage*)m_draggedImage)->updateDraggedItemsRect(d, d->getItemsBoundingRect(true) );
						d->addItem(m_draggedImage);
						m_draggedImage->setVisible(true);
						m_draggedImageInitPos = m_draggedImage->pos();
						
						dataStream << event->buttonDownScenePos(Qt::LeftButton);
						mime = new QMimeData();
						mime->setData(Global::DND_DRAGITEMSMIMEDATA, mimeData);

						drag = new QDrag( d->getMainView() );
						drag->setMimeData(mime);
						
						if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction) == Qt::MoveAction)
						{
							// Should do something here.
						}
					}
					event->accept();
					r = true;
				}
			}
		}
		break;

	case QEvent::GraphicsSceneMouseRelease:
		if (d->itemUnderPressed())
		{
			p->m_draggedItems.clear();
			p->m_pressedItem = NULL;
			// evt->accept();
		}
		break;

	default:
		break;
	}; // End of switch

	return r;
}

bool CDiagramDragDropHandler::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	bool				r = true;
	const QMimeData		*mime = NULL;

	mime = event->mimeData();
	if (mime->hasFormat(Global::DND_DRAGITEMSMIMEDATA))
	{
		if (event->modifiers() & Qt::ControlModifier)
			event->setDropAction(Qt::CopyAction);
		else
			event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else
	{
		if (mime->hasFormat("text/uri-list") || 
			mime->hasFormat(Global::DND_ITEMMIMEDATA) || 
			mime->hasFormat(Global::DND_FORMATMIMEDATA))
		{
			event->acceptProposedAction();
		}
		else
		{
			// Only here, we ignore the event, and
			// return false to the diagram
			event->ignore();
			r = false;
		}
	}
	return r;
}

bool CDiagramDragDropHandler::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData		*mime = event->mimeData();
	bool				r = true;
	QPointF				startPos, endPos;

	endPos = event->scenePos();

	// drag items around
	if (mime->hasFormat(Global::DND_DRAGITEMSMIMEDATA))
	{
		QByteArray			itemData = mime->data(Global::DND_DRAGITEMSMIMEDATA);
		QDataStream			dataStream(&itemData, QIODevice::ReadOnly);

		dataStream >> startPos;

		// check whether we should scroll
		// the view a little bit.
		scrollView(endPos);
		
		// align dragged items according to the
		// existed symbols
		if (m_pdata->m_d->isAutoAlignment() && m_pdata->m_pressedItem->isAlignable())
			alignSymbols(startPos, endPos);
		else
			m_draggedImage->setPos( m_draggedImageInitPos + endPos - startPos );
		
		// Check the drag move is a copy or a connection, according to
		// whether CTRL or SHIFT key is pressed.
		checkModifiers(event, startPos, endPos);
	}
	else
	{
		if (mime->hasFormat("text/uri-list") ||
			mime->hasFormat(Global::DND_ITEMMIMEDATA) ||
			mime->hasFormat(Global::DND_FORMATMIMEDATA))
		{
			if (mime->hasFormat(Global::DND_FORMATMIMEDATA))
			{
				if (m_pdata->m_d->getItemAt(endPos))
				{
					event->setDropAction( Qt::CopyAction );
					event->acceptProposedAction();
				}
				else
				{
					event->setDropAction( Qt::IgnoreAction );
				}
			}
			else
			{
				event->acceptProposedAction();
			}
		}
		else
		{
			event->ignore();
		}
	}
	return r;
}

bool CDiagramDragDropHandler::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData	*mime = NULL;
	bool			r = true;

	mime = event->mimeData();
	if (mime->hasFormat(Global::DND_DRAGITEMSMIMEDATA))
	{
		acceptDraggingDrop(event);
		event->acceptProposedAction();
		event->accept();

		if (m_anchorXLine->scene() == m_pdata->m_d)
			m_pdata->m_d->removeItem(m_anchorXLine);
		if (m_anchorYLine->scene() == m_pdata->m_d)
			m_pdata->m_d->removeItem(m_anchorYLine);
		m_pdata->m_pressedItem = NULL;
		m_pdata->m_draggedItems.clear();
	}
	else if (mime->hasFormat(Global::DND_ITEMMIMEDATA))
	{
		acceptNewItemDrop(event);
		event->acceptProposedAction();
		event->accept();
	}
	else if (mime->hasFormat(Global::DND_FORMATMIMEDATA))
	{
		acceptFormatDrop(event);
		event->acceptProposedAction();
		event->accept();
	}
	else if (mime->hasFormat("text/uri-list"))
	{
		acceptResourceDrop(event);
		event->acceptProposedAction();
		event->accept();
	}
	else
	{
		r = false;
	}

	return r;
}

void CDiagramDragDropHandler::acceptDraggingDrop(QGraphicsSceneDragDropEvent *event)
{
	bool					move = true;
	QPointF					startPos, endPos;
	QList<QPointF>			newPositions;
	CDiagram				*d = NULL;
	CDiagramItem			*item = NULL;

	d = m_pdata->m_d;
	startPos = m_draggedImageInitPos;
	endPos = m_draggedImage->pos();
	foreach (item, m_pdata->m_draggedItems)
	{
		newPositions.push_back( item->pos() + item->mapToParent(item->mapFromScene(endPos)) - item->mapToParent(item->mapFromScene(startPos)) );
	}

	// Ctrl key to copy
	if (event->modifiers() & Qt::ControlModifier)
	{
		d->addUndoCommand( new CDragCopyItemsCommand(d, m_pdata->m_draggedItems, newPositions, NULL) );
		event->setDropAction(Qt::CopyAction);
	}
	else
	{
		// Shift key to connect/clone
		if (m_pdata->m_draggedItems.length() == 1 && (event->modifiers() & Qt::ShiftModifier) )
		{
			item = d->getItemAt(event->scenePos());
			if (item)
			{
				d->addUndoCommand( new CDragNewItemCommand(d, d->itemUnderPressed(), item, NULL));
				event->setDropAction(Qt::LinkAction);
			}
			else
			{
				d->addUndoCommand( new CDragNewItemCommand(d,  d->itemUnderPressed(), newPositions.first(), NULL ) );
				event->setDropAction(Qt::CopyAction);
			}
		}
		else
		{
			for (int i = 0; i < m_pdata->m_draggedItems.length(); ++i)
			{
				item = m_pdata->m_draggedItems.at(i);
				if (item->interType() == Global::Node && item->lockPosition())
				{
					move = false;
					break;
				}
			}

			if (move)
			{
				d->addUndoCommand( new CDraggingItemsCommand(d, m_pdata->m_draggedItems, newPositions, NULL) );
				d->emitItemsDraggingStop();
			}

			event->setDropAction(Qt::MoveAction);
		}
	}
	m_pdata->m_draggedItems.clear();
	d->removeItem(m_draggedImage);

	if (m_lineIndicator->scene() == d)
		d->removeItem(m_lineIndicator);

	if (m_rectIndicator->scene() == d)
		d->removeItem(m_rectIndicator);
}

void CDiagramDragDropHandler::acceptNewItemDrop(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData			*mime = event->mimeData();
	QMetaObject				*itemMeta = NULL;
	QByteArray				itemData = mime->data(Global::DND_ITEMMIMEDATA);
	QDataStream				dataStream(&itemData, QIODevice::ReadOnly);
	QString					name, category, libCategory, libName;
	QString					initType, initParam;
	CDiagram				*d = NULL;
	CDiagramItem			*item = NULL;
	qreal					initWidth, initHeight;

	dataStream >> libCategory >> libName >> category >> name >> initType >> initParam >> initWidth >> initHeight;

	d = m_pdata->m_d;
	itemMeta = CDiagramItemTypeDatabase::instance()->getCategory(category)->getItemMetaObject(name);
	item = dynamic_cast<CDiagramItem*>( itemMeta->newInstance(Q_ARG(CDiagram*, d)) );
	if (item)
	{
		item->setLibraryCategory(libCategory);
		item->setLibraryName(libName);
		item->setCategory(category);
		item->setName(name);
		item->init( QVariant(initParam) );

		if (initType == InitedByImage && initWidth > 0 && initHeight > 0)
			item->resize(initWidth, initHeight);

		// undo stack's redo to finish to the job
		d->addUndoCommand( new CNewItemCommand(d, item, event->scenePos(), NULL) );

		// make the first added item to be selected.
		d->emitItemsAdded(item);
		d->clearSelection();
		d->getMainView()->setFocus(Qt::TabFocusReason);
		item->setSelected(true);
		
		// reset the mode to cursor if the mode is
		// NOT multiple insertion.
		if (d->mode() != CDiagram::AddMultipleItemMode)
		{
			d->setMode( CDiagram::CursorMode );
		}
	}
}

void CDiagramDragDropHandler::acceptFormatDrop(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData			*mime = event->mimeData();
	QByteArray				itemData = mime->data(Global::DND_FORMATMIMEDATA);
	QDataStream				dataStream(&itemData, QIODevice::ReadOnly);
	CDiagramItemFormat		*format = NULL;
	CDiagramItem			*item = NULL;

	item = m_pdata->m_d->getItemAt( event->scenePos() );
	if (item)
	{
		format = new CDiagramItemFormat();
		format->fromStream(dataStream);
		format->apply(item);
	}
}

void CDiagramDragDropHandler::acceptResourceDrop(QGraphicsSceneDragDropEvent *event)
{
	QImage					image;
	QFileInfo				fileInfo;
	QSvgRenderer			svg;
	QString					fileName;
	QString					category, name;
	QList<QUrl>				urls;
	CDiagram				*d = NULL;
	CDiagramItem			*item = NULL;
	QMetaObject				*itemMeta = NULL;
	const QMimeData			*mime = event->mimeData();

	d = m_pdata->m_d;
	urls = mime->urls();
	if (urls.isEmpty())
		return;

	d->clearSelection();
	category = "Core";
	for (int i = 0; i < urls.length(); ++i)
	{
		fileName = urls.at(i).toLocalFile();
		fileInfo.setFile(fileName);
		if (fileInfo.suffix().toLower() != "svg" && image.load(fileName))
		{
			name = "Pixmap";
			itemMeta = CDiagramItemTypeDatabase::instance()->getCategory(category)->getItemMetaObject(name);
			item = dynamic_cast<CDiagramItem*>( itemMeta->newInstance(Q_ARG(CDiagram*, d), Q_ARG(QString, fileName) ) );
		}
		else if (svg.load(fileName))
		{
			name = "SVG";
			itemMeta = CDiagramItemTypeDatabase::instance()->getCategory(category)->getItemMetaObject(name);
			item = dynamic_cast<CDiagramItem*>( itemMeta->newInstance(Q_ARG(CDiagram*, d), Q_ARG(QString, fileName) ) );
		}
		else
		{
			continue;
		}

		if (item)
		{
			item->setLibraryCategory("Clip Art");
			item->setLibraryName("Clip Art");
			item->setCategory(category);
			item->setName(name);

			// undo command
			d->addUndoCommand( new CNewItemCommand(d, item, event->scenePos(), NULL) );
			
			// make the first added item to be selected.
			item->setSelected(true);
		}
	}
	
	d->setMode( CDiagram::CursorMode );
}

void CDiagramDragDropHandler::scrollView(const QPointF &p)
{
	QRectF				rect;
	QGraphicsView		*v = NULL;
	QScrollBar			*bar = NULL;

	v = m_pdata->m_d->getMainView();
	if (v)
	{
		// calculate the related visual rect of scene in the view
		rect = v->mapToScene(v->viewport()->geometry()).boundingRect();

		////////////////////////////////////////////////////////
		// the following code snippets are used to auto scroll
		// the scene.
		if (p.x() + m_pdata->m_scrollThreshold >= rect.right())
		{
			bar = v->horizontalScrollBar();
			bar->setValue(bar->value() + m_pdata->m_scrollStep);
		}
		else if (p.x() - m_pdata->m_scrollThreshold <= rect.left())
		{
			bar = v->horizontalScrollBar();
			bar->setValue(bar->value() - m_pdata->m_scrollStep);
		}

		// check whether we have to scroll the scene a little bit
		// and scroll it if needed.
		if (p.y() + m_pdata->m_scrollThreshold >= rect.bottom())
		{
			bar = v->verticalScrollBar();
			bar->setValue(bar->value() + m_pdata->m_scrollStep);
		}
		else if (p.y() - m_pdata->m_scrollThreshold <= rect.top())
		{
			bar = v->verticalScrollBar();
			bar->setValue(bar->value() - m_pdata->m_scrollStep);
		}
	}
}

void CDiagramDragDropHandler::alignSymbols(const QPointF &startPos, const QPointF &endPos)
{
	int						count = 3;
	qreal					dist;
	qreal					mx, cx, lx, w, dx = -1;
	qreal					my, cy, ly, h, dy = -1;
	QRectF					rect, r1, r2;
	QPointF					tmpPos;
	CDiagram				*d = NULL;
	CDiagramItem			*item = NULL;
	QGraphicsView			*v = NULL;
	QList<CDiagramItem*>	topItems;
	QList<qreal>			left, right, top, bottom;

	d = m_pdata->m_d;
	v = d->getMainView();
	topItems = d->topLevelItems();
	tmpPos = m_draggedImageInitPos + endPos - startPos;
	r1 = m_draggedImage->mapRectToScene(m_draggedImage->boundingRect());
	r1.moveCenter(tmpPos);
	w = r1.width() / 2.0;
	h = r1.height() / 2.0;
	
	// calculate the related visual rect of scene in the view
	rect = v->mapToScene(v->viewport()->geometry()).boundingRect();

	for (int i = 0; i < topItems.length(); ++i)
	{
		item = topItems.at(i);

		// skip checking the selected items
		// only check unselected nodes
		if (item->isSelected() || item->interType() != Global::Node || !item->isAnchorable() )
			continue;
	
		r2 = item->mapRectToScene( item->boundingRect() );
		if ( (qAbs( r2.left() - r1.right() ) > r2.width() + r1.width() && qAbs( r2.top() - r1.bottom() ) > r2.height() + r1.height() )||
			 (qAbs( r2.right() - r1.left() ) > r2.width() + r1.width() && qAbs( r2.bottom() - r1.top() ) > r2.height() + r1.height()) )
			continue;

		left.clear();
		right.clear();
		top.clear();
		bottom.clear();

		// locate the anchor points
		item->anchorPoints(left, right, top, bottom, cx, cy);

	
		// Determin x-coordination alignment
		// left x values alignment
		for (int j = 0; j < left.length(); ++j)
		{
			qreal _left = left.at(j);
			dist = qAbs(r1.left() - _left);
			if (dist < dx || dx < 0)
			{
				dx = dist;
				mx = _left + w;
				lx = _left;
			}
		}

		// right x values alignment
		for (int j = 0; j < right.length(); ++j)
		{
			qreal _right = right.at(j);
			dist = qAbs(r1.right() - _right);
			if (dist < dx || dx < 0)
			{
				dx = dist;
				mx = _right - w;
				lx = _right;
			}
		}

		// center x value alignment
		dist = qAbs(tmpPos.x() - cx);
		if (dist < dx || dx < 0)
		{
			dx = dist;
			mx = cx;
			lx = cx;
		}

		// determine the y-coordination alignment
		// top y values alignment
		for (int j = 0; j < top.length(); ++j)
		{
			qreal _top = top.at(j);
			dist = qAbs(r1.top() - _top);
			if (dist < dy || dy < 0)
			{
				dy = dist;
				my = _top + h;
				ly = _top;
			}
		}

		// bottom y values alignment
		for (int j = 0; j < bottom.length(); ++j)
		{
			qreal _bottom = bottom.at(j);
			dist = qAbs(r1.bottom() - _bottom);
			if (dist < dy || dy < 0)
			{
				dy = dist;
				my = _bottom - h;
				ly = _bottom;
			}
		}
		
		// align to center y value
		dist = qAbs(tmpPos.y() - cy);
		if (dist < dy || dy < 0)
		{
			dy = dist;
			my = cy;
			ly = cy;
		}
	}

	if (0 <= dx && dx <= m_pdata->m_snapThreshold)
	{
		if (m_anchorXLine->scene() != d)
		{
			d->addItem(m_anchorXLine);
		}
		m_anchorXLine->setLine(QLineF(lx, rect.top(), lx, rect.bottom()));
		tmpPos.setX(mx);
	}
	else if (m_anchorXLine->scene() == d)
	{
		d->removeItem(m_anchorXLine);
	}

	if (0 <= dy && dy <= m_pdata->m_snapThreshold)
	{
		if (m_anchorYLine->scene() != d)
		{
			d->addItem(m_anchorYLine);
		}
		m_anchorYLine->setLine(QLineF(rect.left(), ly, rect.right(), ly));
		tmpPos.setY(my);
	}
	else if (m_anchorYLine->scene() == d)
	{
		d->removeItem(m_anchorYLine);
	}

	m_draggedImage->setPos( tmpPos );
}

void CDiagramDragDropHandler::checkModifiers(QGraphicsSceneDragDropEvent *event, const QPointF &startPos, const QPointF &endPos)
{
	int				selectedItemsCount = 0;
	CDiagram		*d = NULL;
	CDiagramItem	*item = NULL;

	d = m_pdata->m_d;
	selectedItemsCount = m_pdata->m_draggedItems.length();
	if (event->modifiers() & Qt::ControlModifier)
	{
		event->setDropAction(Qt::CopyAction);
	}
	else if (event->modifiers() & Qt::ShiftModifier)
	{
		if (selectedItemsCount == 1)
		{
			if (m_lineIndicator->scene() != d)
				d->addItem(m_lineIndicator);
			m_lineIndicator->setLine(QLineF(startPos, endPos));
			event->setDropAction(Qt::CopyAction);

			item = d->getItemAt(endPos);
			if (item && m_pdata->m_draggedItems.first() != item)
			{
				if (m_rectIndicator->scene() != d)
					d->addItem(m_rectIndicator);
				m_rectIndicator->setRect( item->mapRectToScene(item->boundingRect()) );
				m_draggedImage->setVisible(false);
				event->setDropAction(Qt::LinkAction);
			}
			else
			{
				if (m_rectIndicator->scene() == d)
					d->removeItem(m_rectIndicator);
				m_draggedImage->setVisible(true);
			}
		}
		else
		{
			event->setDropAction(Qt::MoveAction);
		}
	}
	else
	{
		event->setDropAction(Qt::MoveAction);
	}
	event->accept();
}
