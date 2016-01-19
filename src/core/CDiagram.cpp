/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <QSvgRenderer>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramText>
#include <core/CDiagramContext>
#include <core/CDiagramLayer>
#include <core/CDiagramClipboard>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramCommand>
#include <core/CDiagramCommandBuilder>
#include <core/CDiagramEventHandler>
#include <core/CDiagramItemFormat>
#include <core/CDiagramLibraryModel>

bool stackOrder(CDiagramItem *first,  CDiagramItem *second)
{
	if (!first || !second) return false;

	return first->stackOrder() < second->stackOrder();
}

CDiagram::CDiagram()
	: m_mode( CDiagram::CursorMode ),
	  m_context(NULL),
	  m_undoStack(NULL),
	  m_pdata(NULL),
	  m_idStep(30),
	  m_layer(NULL),
	  m_background(NULL),
	  m_tagLayer(NULL),
	  m_handlerLayer(NULL),
	  m_controlLayer(NULL),
	  m_padding(0),
	  m_scale(1.0),
	  m_newItem(NULL),
	  m_newItemMeta(NULL),
	  m_autoExpand(true),
	  m_autoAlignment(true),
	  m_snapToGrid(false),
	  m_gridSize(10),
	  m_rotateStep(15),
	  m_pageSize( Utils::getPageSize(QPrinter::A4) ),
	  m_isMouseAction(false),
	  m_selectedItemsCount(0),
	  m_textUnderEditing(NULL)
{
	QRect		r;

	m_pdata = new CDiagramPrivateData(this);

	m_undoStack = new QUndoStack(this);
	m_undoStack->setUndoLimit(30);

	m_rubberBand = new QGraphicsRectItem();
	m_rubberBand->setFlag(QGraphicsItem::ItemIsSelectable, false);
	
	// default layers will be added
	initDefaultLayers();

	// The NoIndex is critical here, or it will cause 
	// the diagram to crash.
	setItemIndexMethod( QGraphicsScene::NoIndex );
	setBackgroundBrush(Qt::NoBrush);

	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	connect(this, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(onSceneRectChanged(const QRectF&)));

  setGridVisible(false);
	
  r = QApplication::desktop()->availableGeometry();
	setSceneRect( QRectF(-r.width(), -r.height(), 2 * r.width(), 2 * r.height() ) );
}

CDiagram::~CDiagram()
{
	QList<CDiagramItem*>	allItems;
	
	allItems = topLevelItems();
	foreach(CDiagramItem *item, allItems)
	{
		item->deleteLater();
	}

	m_background->deleteLater();
	m_handlerLayer->deleteLater();
	m_controlLayer->deleteLater();
	m_tagLayer->deleteLater();

	SAFE_DELETE(m_rubberBand);
	SAFE_DELETE(m_pdata);
	SAFE_DELETE(m_undoStack);
}

void CDiagram::initDefaultLayers()
{
	// The background layer added first, but background layer
	// is not included in the layer list, because it is not editable
	m_background = new CDiagramBackgroundLayer(this, m_gridSize);
	addItem(m_background);

	// the middle layer is the item layer, the item layer is added
	// to the layer list as a mark of editable
	m_layer = new CDiagramItemLayer(this);
	addLayer(m_layer);

	// This two layers are NOT actually added to the scene.
	m_handlerLayer = new CDiagramGadgetLayer(this);
	m_controlLayer = new CDiagramGadgetLayer(this);
	m_handlerLayer->setZValue( CDiagram::HandlerLayer );
	m_controlLayer->setZValue( CDiagram::ControlLayer );

	m_tagLayer = new CDiagramTagLayer(this);
	m_tagLayer->setZValue( CDiagram::TagLayer );
	addItem(m_tagLayer);
	m_tagLayer->setVisible(false);

	// More default layers can be added in the
	// future.
	// addLayer(...)
}

void CDiagram::setContext(CDiagramContext *cxt)
{
	m_context = cxt;
}

CDiagramContext* CDiagram::context()
{
	return m_context;
}

void CDiagram::setContextMenu(int mode, QMenu *menu)
{
	if (!menu || mode < UserMode || mode > AddMultipleItemMode )
		return;

	if (m_contextMenus.contains(mode))
		m_contextMenus[mode] = menu;
	else
		m_contextMenus.insert(mode, menu);
}

QMenu* CDiagram::contextMenu(int mode)
{
	QMenu		*menu = NULL;

	if (m_contextMenus.contains(mode))
		menu = m_contextMenus[mode];
	
	return menu;
}

void CDiagram::insertLayer(int index, CDiagramLayer *layer)
{
	if (!layer || m_layers.contains(layer) ) 
		return;
	m_layers.insert(index, layer);
	QGraphicsScene::addItem(layer);
	// TODO: re-order the layers to ensure the "insert" action
}

void CDiagram::addLayer(CDiagramLayer *layer)
{
	if (!layer || m_layers.contains(layer) ) 
		return;
	m_layers.push_back(layer);
	QGraphicsScene::addItem(layer);
}

void CDiagram::removeLayer(CDiagramLayer *layer)
{
	if (!layer || !m_layers.contains(layer))
		return;
	m_layers.removeOne(layer);
	QGraphicsScene::removeItem(layer);
}

CDiagramLayer* CDiagram::getDefaultLayer(CDiagram::DiagramLayer layer)
{
	CDiagramLayer	*r = NULL;

	switch(layer)
	{
	case TagLayer:
		r = m_tagLayer;
		break;

	default:
		break;
	}; // End of switch

	return r;
}

QGraphicsView* CDiagram::getMainView()
{
	QGraphicsView	*view = NULL;

	if (views().length() != 0)
	{
		view = views().first();
	}
	return view;
}

CDiagramItem* CDiagram::getItemAt(const QPointF& pos)
{
	CDiagramItem			*item = NULL;
	QList<QGraphicsItem*>	p;

	p = items(pos);
	for (int i = 0; i < p.length(); ++i)
	{
		item = dynamic_cast<CDiagramItem*>( p.at(i) );
		if (item && isTopLevelItem(item) )
			break;
	}
	return item;
}

CDiagramItem* CDiagram::getItemById(int id)
{
	CDiagramItem	*item = NULL;
	
	if (m_layer)
		item = m_layer->getItemById(id);
	return item;
}

void CDiagram::insertItem(CDiagramItem *item, const QPointF &pos, bool needNewId, bool useOldPos)
{
	CDiagram			*d = NULL;
	QGraphicsScene		*sc = NULL;

	if (!m_layer || !item) return;

	// if the item belongs to a scene, it will first be
	// deleted from the previous scene. The scene can be
	// a diagram instance or other normal scene, we have to
	// treate it differently.
	sc = item->scene();
	if (sc)
	{
		d = dynamic_cast<CDiagram*>(sc);
		if (d)
			d->deleteItem(item);
		else
			sc->removeItem(item);
	}

	// Whether a new item id is required.
	// It is used when the item is re-added via the redo/undo action
	if (needNewId)
		item->setId( acquireItemId() );
	
	m_ids.removeAll( item->id() );
	if (!m_allocatedIds.contains(item->id()))
		m_allocatedIds.push_back(item->id());

	// item is added to the item layer
	m_layer->insertItem(item, useOldPos?item->pos() : m_layer->mapFromScene(pos) );

	emit itemsAdded(item);
}

void CDiagram::deleteItem(CDiagramItem *item)
{
	if (!m_layer || !item) return;

	if (item->scene() == this)
	{
		m_layer->deleteItem(item);
		releaseItemId(item->id());

		// remove it from the layer only cause it
		// become the toplevel item in the scene, we
		// have to remove it from the diagram.
		QGraphicsScene::removeItem(item);

		emit itemsRemoved(item);
	}
}

void CDiagram::addGadget(QGraphicsItem *item, const CDiagram::DiagramLayer layer)
{
	// the gadgets are all added to the scene directly, but
	// their z-index values are setting to the 'layer', make them
	// on top of the items.
	if (!item) return;

	switch(layer)
	{
	case HandlerLayer:
		m_handlerLayer->addGadget(item);
		break;

	case ControlLayer:
		m_controlLayer->addGadget(item);
		break;

	default:
		break;

	}; // End of switch
}

void CDiagram::removeGadget(QGraphicsItem *item, const CDiagram::DiagramLayer layer)
{
	if (!item) return;

	switch(layer)
	{
	case HandlerLayer:
		m_handlerLayer->removeGadget(item);
		break;

	case ControlLayer:
		m_controlLayer->removeGadget(item);
		break;

	default:
		break;

	}; // End of switch
}

bool CDiagram::event(QEvent *ev)
{
	bool							r = false;
	CDiagramEventHandler			*handler;
	QList<CDiagramEventHandler*>	handlers;

    if (!m_pdata)
		return QGraphicsScene::event(ev);

	if (m_context)
		handlers = m_context->eventHandlers();

	foreach (handler, handlers)
	{
		r = handler->onEvent(m_pdata, ev);
		if (r)
		{
			break;
		}
	}

	if (!r)
		r = QGraphicsScene::event(ev);
	else
		ev->accept();
	return r;
}

void CDiagram::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->isAccepted())
        return;

	if (!(Qt::LeftButton & event->button()))
	{
		QGraphicsScene::mousePressEvent(event);
		return;
	}

	switch(m_mode)
	{
	case AddSingleItemMode:
	case AddMultipleItemMode:
		if (m_newItemMeta && !m_newItem)
		{
			m_newItem = dynamic_cast<CDiagramItem*>( m_newItemMeta->newInstance(Q_ARG(CDiagram*, this)) );
			if (m_newItem)
				m_newItem->startGrowing(event->scenePos());
		}
		event->accept();
		break;

	case EditTextMode:
		if (m_textUnderEditing)
		{
			CDiagramItem			*focusItem = NULL;
			CDiagramItem			*clickItem = NULL;
			CDiagramText			*text = NULL;

			// Record in case it is reset in QGraphicsScene::mousePressEvent
			text = m_textUnderEditing;
			focusItem = getTopLevelParentItem(m_textUnderEditing);
			clickItem = getItemAt(event->scenePos());

			// Only when the mouse is clicking the gadget or click 
			if (isMouseClickingGadget(event->scenePos()) )
			{
				m_textUnderEditing->lockFocus(true);
			}
			else if (isMouseClickingItem(event->scenePos()) && clickItem == focusItem)
			{
				if (m_textUnderEditing->shape().contains( m_textUnderEditing->mapFromScene(event->scenePos()) ) )
					m_textUnderEditing->lockFocus(true);
				else
					m_textUnderEditing->lockFocus(false);
			}
			else
			{
				if (m_textUnderEditing->shape().contains( m_textUnderEditing->mapFromScene(event->scenePos()) ) )
					m_textUnderEditing->lockFocus(true);
				else
					m_textUnderEditing->lockFocus(false);
			}

			QGraphicsScene::mousePressEvent(event);

			// after the mousePressEvent, m_textUnderEditing may be reset
			if (text->focusLocked())
				setFocusItem(text, Qt::OtherFocusReason);
			else
				text->clearFocus();
		}
		else
		{
			QGraphicsScene::mousePressEvent(event);
		}
		break;

	case UserMode:
		emit userModeEvent(this, event);
        // event->accept();
		break;

	default:
		QGraphicsScene::mousePressEvent(event);
		break;
	}
}

void CDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	switch (m_mode)
	{
	case AddSingleItemMode:
	case AddMultipleItemMode:
		if (m_newItem != NULL)
		{
			// Only when the item grows to a specified size big enough to be shown,
			// which is determined by onGrowing's return value, then it will be added
			// to the scene
			if (m_newItem->onGrowing( event->scenePos() ) && m_newItem->scene() != this)
				QGraphicsScene::addItem(m_newItem);
		}
		event->accept();
		break;

	case UserMode:
		emit userModeEvent(this, event);
        //event->accept();
		break;

	default:
		QGraphicsScene::mouseMoveEvent(event);
        //event->accept();
		break;
	};
}

void CDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF					p = event->scenePos();
	CDiagramCommandBuilder	*b = NULL;

    if (event->buttons() & Qt::LeftButton)
    {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    b = CDiagramCommandBuilder::instance();
	switch (m_mode)
	{
	case AddSingleItemMode:
	case AddMultipleItemMode:
		if (m_newItem)
		{
			// Check whether the item should be added to the scene
			// by using the stopGrowing function, different item types
			// will have different algorithm to determine that.
			if (m_newItem->stopGrowing(event->scenePos()))
			{
				p = m_newItem->pos();
				if ( m_newItem->scene() == this)
					QGraphicsScene::removeItem(m_newItem);
				addUndoCommand( b->createCommand( Global::CMD_NEWITEM, 
												  Q_ARG(CDiagram*, this), 
												  Q_ARG(CDiagramItem*, m_newItem), 
												  Q_ARG(QPointF, p),
												  Q_ARG(QUndoCommand*, NULL)) );
				clearSelection();
				m_newItem->setSelected(true);
				emit m_newItem->addedToDiagram();
			}
			else
			{
				if ( m_newItem->scene() == this)
					QGraphicsScene::removeItem(m_newItem);
			}
			m_newItem = NULL;
		}
		else
		{
			SAFE_DELETE(m_newItem);
		}

		// if single add mode, the mode is changed back to cursor mode
		if (mode() == AddSingleItemMode)
			setMode( CDiagram::CursorMode );
		event->accept();

		break;

	case UserMode:
		emit userModeEvent(this, event);
        //event->accept();
		break;

	default:
		QGraphicsScene::mouseReleaseEvent(event);
		break;
	};

	// NOTE: Used to track the mouse release event. Because the CTRL-C event will be
	// triggered after the mouse double-click or dragging action, in order to filter
	// the fake CTRL-C event, we can only use a timeout event to notify that, during
	// this period, if an CTRL-C event is received, then it is fake.
	m_isMouseAction = true;
	int msec = 100;
	QTimer::singleShot(msec, this, SLOT(onCheckMouseActionTimeout()) );
}

void CDiagram::onCheckMouseActionTimeout()
{
	// after this period, the mark is cleared, the CTRL-C event should be
	// true now.
	m_isMouseAction = false;
}

void CDiagram::keyPressEvent(QKeyEvent *event)
{
	QPointF					p;
	QMap<int, QPointF>		maps;
	CDiagramCommandBuilder	*b = NULL;
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	selectedItems;

	// Under Edit Text mode, the key events are
	// all sent to the text editor.
	if ( mode() == CDiagram::EditTextMode)
	{
		QGraphicsScene::keyPressEvent(event);
		return;
	}

	b = CDiagramCommandBuilder::instance();
	maps[Qt::Key_Left] = QPointF(-1, 0);
	maps[Qt::Key_Right]= QPointF(1,  0);
	maps[Qt::Key_Up]   = QPointF(0, -1);
	maps[Qt::Key_Down] = QPointF(0,  1);
	switch (event->key())
	{
	case Qt::Key_Backspace:
	case Qt::Key_Delete:
		selectedItems = selectedTopLevelItems();
		if (selectedItems.length() > 0)
		{
			// The "const" keyword for the second parameters must be provided
			// under windows platform, or it will not be able to create the command 
			// instance
			addUndoCommand( b->createCommand( Global::CMD_DELETEITEMS, 
											  Q_ARG(CDiagram*, this), 
											  Q_ARG(const QList<CDiagramItem*>&, selectedItems),
											  Q_ARG(QUndoCommand*, NULL) ) );
		}
		else
		{
			QGraphicsScene::keyPressEvent(event);
		}
		break;

	case Qt::Key_Left:
	case Qt::Key_Right:
	case Qt::Key_Up:
	case Qt::Key_Down:
		p = maps[event->key()];
		selectedItems = selectedTopLevelItems();
		if (mode() == CDiagram::CursorMode && selectedItems.length() > 0)
		{
			if (event->modifiers() & Qt::ControlModifier)
			{
				p.setX(p.x() * 8);
				p.setY(p.y() * 8);
			}

			// The "const" keyword for the second parameters must be provided
			// under windows platform, or it will not be able to create the command 
			// instance
			addUndoCommand( b->createCommand(Global::CMD_MOVEITEMS, 
											 Q_ARG(CDiagram*, this), 
											 Q_ARG(const QList<CDiagramItem*>&, selectedItems ),
											 Q_ARG(qreal, p.x()), 
											 Q_ARG(qreal, p.y()),
											 Q_ARG(QUndoCommand*, NULL)) );
		}
		else
		{
			QGraphicsScene::keyPressEvent(event);
		}
		break;
	
	case Qt::Key_C:
		if ((event->modifiers() & Qt::ControlModifier) && !m_isMouseAction)
		{
			copy();
		}
		else
		{
			QGraphicsScene::keyPressEvent(event);
		}
		break;

	case Qt::Key_V:
		if ((event->modifiers() & Qt::ControlModifier) && !m_isMouseAction)
		{
			addUndoCommand( b->createCommand(Global::CMD_PASTE, 
											 Q_ARG(CDiagram*, this),
											 Q_ARG(QUndoCommand*, NULL)) );
		}
		else
		{
			QGraphicsScene::keyPressEvent(event);
		}

	case Qt::Key_G:
		if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) )
		{
			addUndoCommand( b->createCommand(Global::CMD_UNGROUP, 
											 Q_ARG(CDiagram*, this),
											 Q_ARG(QUndoCommand*, NULL)) );
		}
		else if (event->modifiers() & Qt::ControlModifier)
		{
			addUndoCommand( b->createCommand(Global::CMD_GROUP, 
											 Q_ARG(CDiagram*, this),
											 Q_ARG(QUndoCommand*, NULL)) );
		}
		else
		{
			QGraphicsScene::keyPressEvent(event);
		}

	default:
		QGraphicsScene::keyPressEvent(event);
		break;

	}; // End of switch
}

void CDiagram::setMode(CDiagram::DiagramMode mode)
{
	if (mode == m_mode) return;

	if (mode == TagMode && m_tagLayer)
	{
		clearSelection();
		m_handlerLayer->setGadgetsVisible(false);
		//m_controlLayer->setVisible(false);
		m_tagLayer->setVisible(true);
	}
	else if (m_mode == TagMode)
	{
		m_tagLayer->setVisible(false);
		m_handlerLayer->setGadgetsVisible(true);
		//m_controlLayer->setVisible(true);
	}
	m_mode = mode;
	emit modeChanged(mode);
}

CDiagram::DiagramMode CDiagram::mode() const
{
	return m_mode;
}

bool CDiagram::requestEditingText(CDiagramText *text)
{
	qreal		degree;
	QPointF		p1, p2;
	QRectF		r;
	QLineF		line;

	if (!text || text->scene() != this) 
		return false;

	m_textUnderEditing = text;
	setMode( CDiagram::EditTextMode );

	clearSelection();

	r = text->boundingRect();
	
	p1 = text->mapToScene( r.center() );
	p2 = text->mapToScene( QPointF(r.right(), r.center().y() ) );
	
	line.setPoints( p1, p2);
	degree = line.angle();

	m_textRotation = text->rotation();
	text->setTransformOriginPoint( r.center() );
	text->setRotation(degree);
	
	emit editingText(text);
	return true;
}

void CDiagram::finishEditingText(CDiagramText *text, const QString &oldText, const QRectF &oldRect)
{
	CDiagramItem			*item = NULL;
	CDiagramCommandBuilder	*b = NULL;

	if (!text || text->scene() != this || text != m_textUnderEditing)
		return;

	item = dynamic_cast<CDiagramItem*>( text->parentItem() );
	if (!item)
		return;

	if (text->hasModification())
	{
		b = CDiagramCommandBuilder::instance();
		addUndoCommand( b->createCommand( Global::CMD_EDITTEXT, 
											  Q_ARG(CDiagram*, this), 
											  Q_ARG(CDiagramItem*, item),
											  Q_ARG(CDiagramText*, text),
											  Q_ARG(const QString&, oldText),
											  Q_ARG(const QRectF&, oldRect),
											  Q_ARG(QUndoCommand*, NULL) ));
	}

	setMode(CDiagram::CursorMode);
	emit textChanged(text);
	m_textUnderEditing = NULL;

	text->setRotation(m_textRotation);
}

void CDiagram::onSelectionChanged()
{
	QList<CDiagramItem*>	selecteds;

	if (m_mode == EditTextMode)
	{
		clearSelection();
	}
	selecteds = selectedTopLevelItems();
	m_selectedItemsCount = selecteds.length();

	emit itemsSelectionChanged(selecteds, selectedItemsBoundingRect() );
}

void CDiagram::onSceneRectChanged(const QRectF &rect)
{
	qreal	w, h;

	w = rect.width();
	h = rect.height();

	m_background->setSize(w, h);
	m_handlerLayer->setSize(w, h);
	m_controlLayer->setSize(w, h);
	m_tagLayer->setSize(w, h);
	m_layer->setSize(w, h);

	m_background->setPos( rect.topLeft() + QPointF( w/2.0, h/2.0) );
	m_layer->setPos(m_background->pos());
}

void CDiagram::selectAll()
{
	QPainterPath	path;

	path.addRect( sceneRect() );
	setSelectionArea(path);

	// emit selectionAreaChanged( selectionArea() );
}

void CDiagram::copy()
{
	QMimeData				*data = NULL;
	QDomDocument			doc;
	QRectF					r = printArea(true);
	QPixmap					image(r.width(), r.height());
	QPainter				painter;
	CDiagramClipboard		*clipboard = CDiagramClipboard::clipboard();

	// fetch the image
	//image.fill(Qt::transparent);
	image.fill(Qt::white);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	drawSelectedItems(&painter, QRectF(0, 0, r.width(), r.height()) );
	painter.end();
	
	// fetch the copied items in string
	copyItems(selectedTopLevelItems(), doc);
	
	data = new QMimeData();
	data->setImageData(image);
	clipboard->clear(CDiagramClipboard::Global);
	clipboard->setMimeData(data, CDiagramClipboard::Global);

	data = new QMimeData();
	data->setText(doc.toString());
	clipboard->clear(CDiagramClipboard::Application);
	clipboard->setMimeData(data, CDiagramClipboard::Application);
}

void CDiagram::copyItems(const QList<CDiagramItem*> &items, QDomDocument &doc)
{
	QDomElement		els;
	CDiagramItem	*item = NULL;

	els = doc.createElement( QLatin1String("items") );
	doc.appendChild(els);
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		QDomElement e = doc.createElement( QLatin1String("item") );
		item->toXml(e);
		els.appendChild(e);
	}
}

int CDiagram::acquireItemId()
{
	int		id;

	if (m_ids.isEmpty())
		updateIds();

	id = m_ids.first();
	m_ids.pop_front();
	return id;
}

void CDiagram::releaseItemId(int id)
{
	if (id < 0) return;

	if (!m_ids.contains(id))		 m_ids.push_back(id);
	if (m_allocatedIds.contains(id)) m_allocatedIds.removeAll(id);
}

void CDiagram::updateIds()
{
	int	maxId = 0;
	int	prev = -1, now = 0;

	m_ids.clear();
	qStableSort(m_allocatedIds.begin(), m_allocatedIds.end());
	if (!m_allocatedIds.isEmpty()) maxId = m_allocatedIds.last() + 1;
	for (int i = 0; i < m_allocatedIds.length(); ++i)
	{
		now = m_allocatedIds.at(i);
		while (now > prev + 1)
			m_ids.push_back(++prev);
		prev = now;
	}

	// still empty
	if (m_ids.isEmpty())
	{
		for (int i = 0; i < m_idStep; ++i)
			m_ids.push_back(maxId + i);
	}
}

QRectF CDiagram::getItemsBoundingRect(bool filterSelected)
{
	QRectF					r;
	QList<CDiagramItem*>	allSelectedItems;

	if (filterSelected)
	{
		r = selectedItemsBoundingRect();
	}
	else
	{
		allSelectedItems = selectedTopLevelItems();
		selectAll();
		r = selectedItemsBoundingRect();
		//r.adjust(-m_padding, -m_padding, m_padding, m_padding);
		clearSelection();

		for (int i = 0; i < allSelectedItems.length(); ++i)
			allSelectedItems.at(i)->setSelected(true);
	}
	return r;
}

void CDiagram::drawAllItems(QPainter *painter, const QRectF &target)
{
	QRectF					r;
	QGraphicsItem			*item = NULL;
	QList<CDiagramItem*>	allSelectedItems;

	if (!painter) return;

	allSelectedItems = selectedTopLevelItems();
	selectAll();
	r = printArea();
	clearSelection();

	m_handlerLayer->enterTemporaryHide();
	m_controlLayer->enterTemporaryHide();
	m_background->setVisible(false);

	render(painter, target, r);
	
	m_background->setVisible(true);
	m_handlerLayer->exitTemporaryHide();
	m_controlLayer->exitTemporaryHide();

	for (int i = 0; i < allSelectedItems.length(); ++i)
		allSelectedItems.at(i)->setSelected(true);
}

void CDiagram::drawSelectedItems(QPainter *painter, const QRectF &target)
{
	QGraphicsItem			*item = NULL;
	QList<QGraphicsItem*>	visibleItems;
	QList<CDiagramItem*>	allSelectedItems;
	QList<QGraphicsItem*>	boundedItems;
	QRectF					r;
	
	if (!painter) return;

	r = printArea(true);

	allSelectedItems = selectedTopLevelItems();
	if (allSelectedItems.length() == 0)
		return;

	boundedItems = items(r, Qt::IntersectsItemShape);
	for (int i = 0; i < boundedItems.length(); ++i)
	{
		item = boundedItems.at(i);
		if (item->isVisible() && item->type() == CDiagramItem::Type && isTopLevelItem( (CDiagramItem*)item ) && !item->isSelected())
		{
			item->setVisible(false);
			visibleItems.push_back(item);
		}
	}

	clearSelection();
	m_handlerLayer->enterTemporaryHide();
	m_controlLayer->enterTemporaryHide();
	m_background->setVisible(false);

	render(painter, target, r);

	m_background->setVisible(true);
	m_handlerLayer->exitTemporaryHide();
	m_controlLayer->exitTemporaryHide();

	for (int i = 0; i < allSelectedItems.length(); ++i)
		allSelectedItems.at(i)->setSelected(true);

	for (int i = 0; i < visibleItems.length(); ++i)
		visibleItems.at(i)->setVisible(true);
}

void CDiagram::drawArea(QPainter *painter, const QRectF &source, const QRectF &target)
{
	QGraphicsItem			*item = NULL;
	QList<CDiagramItem*>	allSelectedItems;

	if (!painter) return;
	
	allSelectedItems = selectedTopLevelItems();
	clearSelection();
	
	m_handlerLayer->enterTemporaryHide();
	m_controlLayer->enterTemporaryHide();
	m_background->setVisible(false);

	render(painter, target, source );
	
	m_background->setVisible(true);
	m_handlerLayer->exitTemporaryHide();
	m_controlLayer->exitTemporaryHide();

	for (int i = 0; i < allSelectedItems.length(); ++i)
		allSelectedItems.at(i)->setSelected(true);
}

void CDiagram::setScale(qreal factor)
{
	QSizeF		s;
	QRectF		r;

	m_layer->setScale(factor);
	m_background->setScale(factor);

	r = sceneRect();
	s = m_background->size();
    m_background->setPos( QPointF(0, 0) );
    //m_background->setPos( r.topLeft() + QPointF( s.width() * factor /2.0, s.height() * factor /2.0) );
	m_layer->setPos(m_background->pos());

	m_scale = factor;

	emit scaleChanged(m_scale);
	emit itemsGeometryChanged( selectedItemsBoundingRect() );
}

qreal CDiagram::getScale()
{
	return m_scale;
}

void CDiagram::setAutoExpand(bool value)
{
	m_autoExpand = value;
}

bool CDiagram::isAutoExpand() const
{
	return m_autoExpand;
}

void CDiagram::setAutoAlignment(bool value)
{
	m_autoAlignment = value;
}

bool CDiagram::isAutoAlignment() const
{
	return m_autoAlignment;
}

const QSizeF CDiagram::getPageSize()
{
	return m_pageSize;
}

void CDiagram::setPageSize(const QSizeF &size)
{
	setPageSize(size.width(), size.height());
}

void CDiagram::setPageSize(qreal w, qreal h)
{
	m_pageSize.setWidth(w);
	m_pageSize.setHeight(h);
}

QRectF CDiagram::printArea(bool onlySelectedItems)
{
	QRectF						r;
	QList<CDiagramItem*>		items;

	if (!m_printArea.isNull())
		return m_printArea;

	if (onlySelectedItems)
		items = selectedTopLevelItems();
	else
		items = topLevelItems();

	foreach (CDiagramItem *item, items)
	{
		r = r.united(item->printArea());
	}

	return r;
}

void CDiagram::setGridVisible(bool value)
{
	if (m_background)
	{
		m_background->setGridVisible(value);
		update();
	}
}

bool CDiagram::isGridVisible() const
{
	bool	r = false;

	if (m_background)
	{
		r = m_background->gridVisible();
	}
	return r;
}

void CDiagram::setSnapToGrid(bool snap)
{
	m_snapToGrid = snap;
}

bool CDiagram::snapToGrid() const
{
	return m_snapToGrid;
}

void CDiagram::setRotateStep(int r)
{
	m_rotateStep = r;
}

int CDiagram::rotateStep() const
{
	return m_rotateStep;
}

void CDiagram::setGridSize(int r)
{
	if (r <= 0)
		return;
	m_gridSize = r;
	m_background->setTick(r);
}

int CDiagram::gridSize() const
{
	return m_gridSize;
}

void CDiagram::addUndoCommand(CDiagramCommand *cmd)
{
	if (m_undoStack && cmd)
	{
		m_undoStack->push( static_cast<QUndoCommand*>(cmd));
		emit changed();
	}
}

QUndoStack* CDiagram::undoStack()
{
	return m_undoStack;
}

bool CDiagram::isTopLevelItem(CDiagramItem *item)
{
	bool	r = false;
	if (item && item->parentItem() == m_layer)
		r = true;
	return r;
}

QList<CDiagramItem*> CDiagram::getItemsIsObscuringTo(CDiagramItem *item)
{
	CDiagramItem				*tmp = NULL;
	QList<CDiagramItem*>		rs;
	QList<QGraphicsItem*>		allItems;

	if (!item) return rs;

	allItems = items(item->mapToScene(item->shape()));
	for (int i = allItems.length() - 1; i >= 0; --i)
	{
		tmp = dynamic_cast<CDiagramItem*>(allItems[i]);
		if (!tmp) continue;
		if (tmp == item)
			break;

		if ( isTopLevelItem(tmp))
			rs.push_front(tmp);
	}
	return rs;
}

QList<CDiagramItem*> CDiagram::getItemsIsObscuredBy(CDiagramItem *item)
{
	CDiagramItem				*tmp = NULL;
	QList<CDiagramItem*>		rs;
	QList<QGraphicsItem*>		allItems;

	if (!item) return rs;

	allItems = items(item->mapToScene(item->shape()));
	for (int i = 0; i < allItems.length(); ++i)
	{
		tmp = dynamic_cast<CDiagramItem*>(allItems[i]);
		if (!tmp) continue;
		if (tmp == item)
			break;

		if (isTopLevelItem(tmp))
			rs.push_front(tmp);
	}
	return rs;
}


QRectF CDiagram::selectedItemsBoundingRect()
{
	QRectF					rect, r;
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	items;

	items = selectedTopLevelItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		rect = item->mapRectToScene( item->boundingRect() );
		r |= rect;
	}
	return r;
}

QPainterPath CDiagram::selectedItemsShape()
{
	QPainterPath			r, path;
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	items;

	items = selectedTopLevelItems();

	// use windingfill rule to make sure, when two items are overlap
	// there will not be a hole.
	path.setFillRule(Qt::WindingFill);
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		path = item->mapToScene(item->shape());
		r.addPath(path);
	}
	return r;
}

CDiagramItem* CDiagram::selectedTopLevelItem()
{
	CDiagramItem			*item = NULL;
	QList<QGraphicsItem*>	selected;

	selected = selectedItems();
	for (int i = 0; i < selected.length(); ++i)
	{
		item = dynamic_cast<CDiagramItem*>( selected.at(i) );
		if ( item && isTopLevelItem(item) )
		{
			break;
		}
	}
	return item;
}

QList<CDiagramItem*> CDiagram::selectedTopLevelItems()
{
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	items;
	QList<QGraphicsItem*>	selected;
	QGraphicsItem			*graphicsItem = NULL;

	selected = selectedItems();
	for (int i = 0;i < selected.length(); ++i)
	{
		graphicsItem = selected.at(i);
		if (graphicsItem->type() == CDiagramItem::Type)
		{
			item = dynamic_cast<CDiagramItem*>( graphicsItem );
			if (item && isTopLevelItem(item))
				items.push_back(item);
		}
	}

	qStableSort(items.begin(), items.end(), stackOrder);
	return items;
}

QList<CDiagramItem*> CDiagram::topLevelItems(Qt::SortOrder order)
{
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	topItems;
	QList<QGraphicsItem*>	allitems;

	allitems = items(order);
	for (int i = 0; i < allitems.length(); ++i)
	{
		item = dynamic_cast<CDiagramItem*>(allitems.at(i));
		if (item && item->type() == CDiagramItem::Type && isTopLevelItem(item))
			topItems.push_back(item);
	}
	return topItems;
}

bool CDiagram::isMouseClickingItem(const QPointF &p) const
{
	bool					r = false;
	QGraphicsItem			*item = NULL;

	item = itemAt(p);
	while (item && item->parentItem())
	{
		if (item->parentItem() == m_layer)
			break;
		else
			item = item->parentItem();
	}

	if (item && item->type() == CDiagramItem::Type)
		r = true;
	return r;
}

bool CDiagram::isMouseClickingGadget(const QPointF &p) const
{
	bool					r = false;
	QGraphicsItem			*item = NULL;
	
	item = itemAt(p);
	while (item && item->parentItem())
	{
		if (item->parentItem() == m_layer)
			return r;
		else
			item = item->parentItem();
	}
	
	if (m_controlLayer->containsGadget(item) || m_handlerLayer->containsGadget(item) )
		r = true;
	return r;
}

QList<CDiagramItem*> CDiagram::getItemsHasFocus()
{
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	list;

	if (m_mode == EditTextMode && m_textUnderEditing)
	{
		item = dynamic_cast<CDiagramItem*>(m_textUnderEditing->parentItem());
		if (item)
			list.push_back(item);
	}
	else
	{
		// list = selectedTopLevelItems();
		QList<QGraphicsItem*>	allSelectedItems;

		allSelectedItems = selectedItems();
		for (int i = 0; i < allSelectedItems.length(); ++i)
		{
			item = dynamic_cast<CDiagramItem*>(allSelectedItems.at(i));
			if (item)
				list.push_back(item);
		}
	}
	return list;
}

QMetaObject* CDiagram::getNewItemMeta()
{
	return m_newItemMeta;
}

void CDiagram::setNewItemMeta(const QMetaObject *meta)
{
	m_newItemMeta = const_cast<QMetaObject*>(meta);
}

CDiagramItem* CDiagram::getNewItem()
{
	return m_newItem;
}

void CDiagram::setNewItem(const CDiagramItem *item)
{
	m_newItemMeta = NULL;
	m_newItem = const_cast<CDiagramItem*>(item);
}

void CDiagram::emitClosing()
{
	emit closing();
}

void CDiagram::emitVisibleRectChanged()
{
	QRectF			r;
	QGraphicsView	*v = NULL;

	v = getMainView();
	if (v)
		r = v->mapToScene(v->viewport()->geometry()).boundingRect();
	emit visibleRectChanged(r);
}

void CDiagram::emitItemsDragging()
{
	emit itemsDragging();
}

void CDiagram::emitItemsDraggingStop()
{
	emit itemsDraggingStop( selectedItemsBoundingRect() );
}

void CDiagram::emitItemsGeometryChanged()
{
	emit itemsGeometryChanged( selectedItemsBoundingRect() );
}

void CDiagram::emitItemsRotationChanged(qreal r)
{
	emit itemsRotationChanged(r);
}

void CDiagram::emitItemsMoving(qreal dx, qreal dy)
{
	emit itemsMoving(dx, dy);
}

void CDiagram::emitItemsAdded(CDiagramItem *item)
{
	if (item)
		emit item->addedToDiagram();
}

void CDiagram::emitItemsLoadFinished()
{
	emit itemsLoadFinished();
}

void CDiagram::emitItemsResizing()
{
}

void CDiagram::emitItemsResizingStop()
{
	emit itemsResizingStop( selectedItemsBoundingRect() );
}

void CDiagram::emitItemsRotating()
{
}

void CDiagram::emitItemsRotatingStop()
{
	// use fake angle now
	emit itemsRotatingStop(0);
}

void CDiagram::emitSelectionChanged()
{
	emit selectionChanged();
}

void CDiagram::startRubberBandSelection(const QPointF &p)
{
	QWidget					*widget = NULL;
	QGraphicsView			*view = NULL;
	QStyleOptionRubberBand	option;
	QColor					highlight, dimHighlight;

	view = getMainView();
	widget = view->viewport();
	option.initFrom(widget);
	highlight = option.palette.color(QPalette::Active, QPalette::Highlight);
    m_rubberBand->setPen(highlight.darker(120));
    dimHighlight = QColor(qMin(highlight.red()/2 + 110, 255),
                          qMin(highlight.green()/2 + 110, 255),
                          qMin(highlight.blue()/2 + 110, 255),
                          (widget && widget->isTopLevel())? 255 : 127);
    m_rubberBand->setBrush(dimHighlight);
	m_rubberBandStartPosition = p;
}

void CDiagram::updateRubberBandSelection(const QPointF &p)
{
	qreal			x, y;
	qreal			width, height;
	QPainterPath	path;

	if (m_rubberBand->scene() != this)
		addItem(m_rubberBand);
	
	width = qAbs(p.x() - m_rubberBandStartPosition.x());
	height = qAbs(p.y() - m_rubberBandStartPosition.y());
	if (p.x() > m_rubberBandStartPosition.x())
	{
		if (p.y() > m_rubberBandStartPosition.y())
		{
			x = m_rubberBandStartPosition.x();
			y = m_rubberBandStartPosition.y();
		}
		else
		{
			x = m_rubberBandStartPosition.x();
			y = p.y();
		}
	}
	else
	{
		if (p.y() > m_rubberBandStartPosition.y())
		{
			x = p.x();
			y = m_rubberBandStartPosition.y();
		}
		else
		{
			x = p.x();
			y = p.y();
		}
	}

	m_rubberBand->setRect(x, y, width, height);
	path.addRect( m_rubberBand->rect() );
	setSelectionArea(path, getMainView()->rubberBandSelectionMode(), QTransform() );
	
	// emit selectionAreaChanged( selectionArea() );
}

void CDiagram::finishRubberBandSelection(const QPointF &p)
{
	QPainterPath path;

	path.addRect( m_rubberBand->rect() );
	setSelectionArea(path, getMainView()->rubberBandSelectionMode(), QTransform() );
	removeItem(m_rubberBand);

	// emit selectionAreaChanged( selectionArea() );
}

CDiagramItem* CDiagram::itemUnderPressed()
{
	return m_pdata->m_pressedItem;
}

CDiagramText* CDiagram::textUnderEditing()
{
	return m_textUnderEditing;
}

CDiagramItem* CDiagram::getTopLevelParentItem(QGraphicsItem *child)
{
	QGraphicsItem	*item = NULL;

	item = child;
	while (item && item->parentItem())
	{
		if (item->parentItem() == m_layer)
			break;
		else
			item = item->parentItem();
	}
	return dynamic_cast<CDiagramItem*>(item);
}

void CDiagram::refreshStackOrder()
{
	CDiagramItem			*item = NULL, *prev = NULL, *curr = NULL;
	QList<CDiagramItem*>	allItems;

	allItems = topLevelItems();
	if (allItems.length() < 2)
		return;

	prev = allItems.first();
	qStableSort(allItems.begin(), allItems.end(), stackOrder);
	
	for (int i = allItems.length() - 1; i >= 0; --i)
	{
		item = allItems.at(i);
		item->stackBefore(prev);
		prev = item;
	}
}

void CDiagram::expandSceneRect(qreal step)
{
	qreal		w, h;
	QRectF		r;

	if (step <= 0) 
		return;
	r = sceneRect();
	w = r.width() + step;
	h = r.height() + step;

	setSceneRect( QRectF(-w/2.0, -h/2.0, w, h) );
}

void CDiagram::shrinkSceneRect(qreal step)
{
	qreal		w, h;
	QRectF		r;

	if (step <= 0) 
		return;

	r = sceneRect();
	w = r.width() - step;
	h = r.height() - step;

	if (w < 0 || h < 0)
		return;

	setSceneRect( QRectF(-w/2.0, -h/2.0, w, h) );
}

void CDiagram::setHelperLayersVisible(bool value)
{
	m_handlerLayer->setGadgetsVisible(value);
	m_controlLayer->setGadgetsVisible(value);
	m_background->setVisible(value);
}

void CDiagram::setBackground(const QBrush &brush)
{
    if (m_background)
        m_background->setBrush(brush);
}

QBrush CDiagram::background() const
{
    return m_background->brush();
}


void CDiagram::toXml(QDomElement &e)
{
}

void CDiagram::fromXml(QDomElement &e)
{
}
