/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramLayer>

// A line path used antialiasing.
class CPathIndicator: public QGraphicsPathItem
{
public:
	CPathIndicator(QGraphicsItem *parent)
		: QGraphicsPathItem(parent)
	{
	}

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		QGraphicsPathItem::paint(painter, option, widget);
	}
}; // End of class CPathIndicator

CDiagramLayer::CDiagramLayer(CDiagram *parent)
	: QGraphicsObject(),
	  m_p(parent),
	  m_pen( QPen( Qt::gray, 2 ) ),
	  m_brush( QBrush( Qt::white ) )
{
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setHandlesChildEvents(false);

	// for performance optimization

	setFlag(QGraphicsItem::ItemHasNoContents);
}

CDiagramLayer::~CDiagramLayer()
{

}

QRectF CDiagramLayer::boundingRect() const
{
	qreal		hs, hw, hh;
	qreal		pw;

	pw = m_pen.widthF();
	hw = m_width / 2.0;
	hh = m_height / 2.0;
	hs = pw / 2.0;
	return QRectF(-(hw + hs), -(hh + hs), m_width + pw, m_height + pw);
}

QPainterPath CDiagramLayer::shape() const
{
	QPainterPath path;
	path.addRect(-m_width/2.0, -m_height/2.0, m_width, m_height);
	return path;
}

void CDiagramLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	// Q_UNUSED(painter);

	painter->save();
	painter->fillRect( boundingRect(), Qt::red );
	painter->restore();
}

QSizeF CDiagramLayer::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	return QSizeF(m_width, m_height);
}

void CDiagramLayer::setSize(qreal w, qreal h)
{
	m_width = w;
	m_height = h;
}

QSizeF CDiagramLayer::size()
{
	return QSizeF(m_width, m_height);
}

void CDiagramLayer::setGeometry(const QRectF &rect)
{
	prepareGeometryChange();
	
	setPos( rect.topLeft() );
	m_width = rect.width();
	m_height = rect.height();
}

void CDiagramLayer::setBrush(const QBrush &value)
{
    m_brush = value;
    update();
}

QBrush CDiagramLayer::brush() const
{
    return m_brush;
}

void CDiagramLayer::setPen(const QPen &value)
{
    m_pen = value;
    update();
}

QPen CDiagramLayer::pen() const
{
    return m_pen;
}


CDiagramItemLayer::CDiagramItemLayer(CDiagram *parent)
	: CDiagramLayer(parent)
{
}

CDiagramItemLayer::~CDiagramItemLayer()
{
}

void CDiagramItemLayer::insertItem(CDiagramItem *item, const QPointF &pos)
{
	m_items.insert(item->id(), item);
	item->m_index = m_items.size() - 1;
	item->setParentItem(this);
	item->setGeometry( QRectF(pos, item->geometry().size()) );
}

CDiagramItem* CDiagramItemLayer::deleteItem(CDiagramItem *item)
{
	m_items.remove(item->id());
	item->setParentItem(NULL);
	updateStackOrder();
	return item;
}

CDiagramItem* CDiagramItemLayer::getItemById(int value)
{
	CDiagramItem	*i = NULL;

	if (m_items.contains(value))
		i = m_items[value];
	return i;
}

void CDiagramItemLayer::changeStackOrder(CDiagramItem *item, int step, Qt::SortOrder order)
{
	int						from, to;
	QGraphicsItem			*child = NULL;
	QList<QGraphicsItem*>	childs = childItems();

	step = (step == 0) ? childs.length() : step;
	from = item->stackOrder();
	to = (order == Qt::AscendingOrder) ? qMin(from + step, childs.length() - 1) : qMax(0, from - step);

	if (order == Qt::AscendingOrder)
	{
		for (int i = from + 1; i <= to; ++i)
		{
			child = childs.at(i);
			child->stackBefore(item);
		}
	}
	else
	{
		child = childs.at(to);	
		item->stackBefore(child);
	}
	updateStackOrder();
	emit item->geometryChanged();
}

void CDiagramItemLayer::updateStackOrder()
{
	CDiagramItem			*item = NULL;
	QList<QGraphicsItem*>	childs;

	childs = childItems();
	for (int i = 0; i < childs.length(); ++i)
	{
		item = dynamic_cast<CDiagramItem*>(childs.at(i));
		if (item)
			item->m_index = i;
	}
}

CDiagramBackgroundLayer::CDiagramBackgroundLayer(CDiagram *parent, int tick)
	: CDiagramLayer(parent),
	  m_padding(0),
	  m_tick(20),
	  m_gridVisible(true),
	  m_gridColor( QColor("#E8ECFA") )
{
	m_pen = QPen( Qt::black, 1 );
    m_brush = QBrush( Qt::white );

	setFlag(QGraphicsItem::ItemHasNoContents, false);
}


CDiagramBackgroundLayer::~CDiagramBackgroundLayer()
{
}

void CDiagramBackgroundLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	Q_UNUSED(option);
	
	int				x0, y0, x1, y1, k = 0, gs = m_tick;
	const int		bufferSize = 2500;
	static QLine	buffer[bufferSize];
	QRectF			rect = boundingRect();			

	painter->save();
	painter->setPen( Qt::NoPen );
	painter->setRenderHint(QPainter::Antialiasing, false);

	painter->setBrush( QColor(0, 0, 0, 28) );
	painter->drawRect( rect.adjusted(1, 1, 1, 1) );

	painter->setBrush( m_brush );
	painter->drawRect( rect );

	if (m_gridVisible)
	{
		x0 = ceil(rect.left());
		y0 = ceil(rect.top());
		x1 = floor(rect.right());
		y1 = floor(rect.bottom());

		x1 = ((x1 - x0) % gs) < (gs / 2.0) ? x1 - (x1 - x0) % gs: x1 + gs - (x1 - x0) % gs;
		y1 = ((y1 - y0) % gs) < (gs / 2.0) ? y1 - (y1 - y0) % gs: y1 + gs - (y1 - y0) % gs;

		painter->setPen( QPen( QColor("#F9F9F9"), 1 ) );
		painter->translate(-0.5, -0.5);
		for (int i=x0; i<=x1; i+=gs)
		{
			buffer[k].setLine(i, y0, i, y1);
			++ k;
			if (k >= bufferSize)
			{
				painter->drawLines(buffer, k);
				k = 0;
			}
		}
		
		for (int j=y0; j<=y1; j+=gs)
		{
			buffer[k].setLine(x0, j, x1, j);
			++ k;
			if (k >= bufferSize)
			{
				painter->drawLines(buffer, k);
				k = 0;
			}
		}
		
		if (k > 0)
		{
			painter->drawLines(buffer, k);
			k = 0;
		}

		painter->setPen( QPen( QColor("#F0F0F0"), 1 ) );
		for (int i=x0; i<=x1; i+= 5 * gs)
		{
			buffer[k].setLine(i, y0, i, y1);
			++ k;
			if (k >= bufferSize)
			{
				painter->drawLines(buffer, k);
				k = 0;
			}
		}
		
		for (int j=y0; j<=y1; j+=5 * gs)
		{
			buffer[k].setLine(x0, j, x1, j);
			++ k;
			if (k >= bufferSize)
			{
				painter->drawLines(buffer, k);
				k = 0;
			}
		}
		
		if (k > 0)
		{
			painter->drawLines(buffer, k);
			k = 0;
		}
	}
	painter->restore();
}

void CDiagramBackgroundLayer::setGridColor(const QColor &value)
{
	m_gridColor = value;
}

QColor CDiagramBackgroundLayer::gridColor()
{
	return m_gridColor;
}

void CDiagramBackgroundLayer::setTick(int value)
{
	m_tick = value;
	update();
}

int CDiagramBackgroundLayer::tick()
{
	return m_tick;
}

void CDiagramBackgroundLayer::setGridVisible(bool value)
{
	m_gridVisible = value;
	update();
}

bool CDiagramBackgroundLayer::gridVisible()
{
	return m_gridVisible;
}

CDiagramTag::CDiagramTag(QGraphicsItem *parent)
	:QGraphicsTextItem(parent)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
}

CDiagramTag::~CDiagramTag()
{
}

QVariant CDiagramTag::itemChange(GraphicsItemChange change, const QVariant &value)
{
	return value;
}

void CDiagramTag::focusOutEvent(QFocusEvent *event)
{
	setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void CDiagramTag::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (textInteractionFlags() == Qt::NoTextInteraction)
		setTextInteractionFlags(Qt::TextEditorInteraction);
	QGraphicsTextItem::mouseDoubleClickEvent(event);
}

CDiagramTagLayer::CDiagramTagLayer(CDiagram *parent)
	: CDiagramLayer(parent),
	  m_mode(Cursor),
	  m_scribbling(false),
	  m_currPathItem()
{
	m_currPathItem = new CPathIndicator(this);
	m_currPathItem->setPen(QPen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap));
	setFlag(QGraphicsItem::ItemHasNoContents, false);

    m_cursor = QCursor( QPixmap(":/res/icons/presentation-cursor.png"), 0, 0);
	setCursor(m_cursor);
}

CDiagramTagLayer::~CDiagramTagLayer()
{
}

QRectF	CDiagramTagLayer::boundingRect() const
{
	QRectF			r;
	QGraphicsScene	*sc = NULL;

	sc = scene();
	if (sc && sc->isActive() )
		r = sc->sceneRect();
	return r;
}

QPainterPath CDiagramTagLayer::shape() const
{
	QPainterPath	path;

	path.addRect(boundingRect());
	return path;
}

void CDiagramTagLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	setCursor(m_cursor);
}

void CDiagramTagLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	bool			scribble = true;
	CDiagramTag		*tag = NULL;

	// mouse cursor
	setCursor(m_cursor);
	
	switch(m_mode)
	{
	case Scribble:
		if (event->button() == Qt::LeftButton)
		{
			m_scribbling = true;
			m_currPath.moveTo(event->pos());
		}
		event->accept();
		break;

	default:
		// check if the mouse is clicking tags
		for (int i = 0; i < m_tags.length(); ++i)
		{
			tag = m_tags.at(i);
			if (tag->shape().contains(event->scenePos()))
			{
				// if anyone of the tags is clicked, then
				// it will never be in scribble
				scribble = false;
				break;
			}
		}

		// if it is scribble, then we change the mode
		if (scribble)
		{
			setEditMode(Scribble);
			m_scribbling = true;
			m_currPath.moveTo(event->pos());
		}
		break;
	}; // End of switch
}

void CDiagramTagLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	CDiagramTag		*tag = NULL;

	setCursor(m_cursor);

	switch(m_mode)
	{
	case Scribble:
		// during mouse move event, event->button() does not reflect
		// the pressing button, only by using event->buttons() you can
		// get the pressed button during mouse move event
		if ((event->buttons() & Qt::LeftButton) && m_scribbling)
		{
			m_currPath.lineTo(event->pos());
			m_currPathItem->setPath(m_currPath);
		}
		event->accept();
		break;

	default:
		break;
	}; // End of switch
}

void CDiagramTagLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	switch(m_mode)
	{
	case Scribble:
		if (event->button() == Qt::LeftButton && m_scribbling) 
		{
			m_currPath.lineTo(event->pos());
			m_scribbling = false;
			if (m_currPath.length() > QApplication::startDragDistance())
			{
				m_currPathItem->setPath(m_currPath);
				m_lines.push_back(m_currPathItem);

				// create new path item
				m_currPathItem = new CPathIndicator(this);
				m_currPathItem->setParentItem(this);
				m_currPathItem->setPen(QPen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap));
			}
			else
			{
				m_currPathItem->setPath(QPainterPath());
			}
			
			m_currPath = QPainterPath();
		}
		event->accept();
		break;

	default:
		break;
	}; // End of switch
}

void CDiagramTagLayer::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	CDiagramTag		*tag = NULL;

	if (event->button() == Qt::LeftButton)
	{
		QFont	font("Comic Sans MS", 24);
		font.setBold(true);

		tag = new CDiagramTag(this);
		tag->setParentItem(this);
		tag->setPos(event->scenePos());
		tag->setFont(font);
		tag->setDefaultTextColor( QColor(Qt::red));
		tag->setTextInteractionFlags(Qt::TextEditorInteraction);
		connect(tag, SIGNAL(lostFocus(CDiagramTag*)), this, SLOT(editorLostFocus(CDiagramTag*)));
		scene()->setFocusItem(tag);
	}
	event->accept();
}

void CDiagramTagLayer::setEditMode(CDiagramTagLayer::EditMode mode)
{
	m_mode = mode;
}

CDiagramTagLayer::EditMode CDiagramTagLayer::editMode()
{
	return m_mode;
}

void CDiagramTagLayer::clear()
{
	QGraphicsItem	*item = NULL;
	CDiagramTag		*tag = NULL;

	for (int i = 0; i < m_lines.length(); ++i)
	{
		item = m_lines.at(i);
		item->setParentItem(NULL);
		scene()->removeItem(item);
		SAFE_DELETE(item);
	}
	m_lines.clear();

	for (int i = 0; i < m_tags.length(); ++i)
	{
		tag = m_tags.at(i);
		if (tag)
		{
			tag->setParentItem(NULL);
			scene()->removeItem(tag);
			disconnect(tag, SIGNAL(lostFocus(CDiagramTag*)), this, SLOT(editorLostFocus(CDiagramTag*)));
			tag->deleteLater();
		}
	}
	m_tags.clear();
	update();
}

void CDiagramTagLayer::editorLostFocus(CDiagramTag *tag)
 {
     QTextCursor cursor;
	 
	 if (!tag)
		 return;
	 
	 // reset the cursor shape
	 setCursor(m_cursor);

	 cursor = tag->textCursor();
	 cursor.clearSelection();
     tag->setTextCursor(cursor);

     if (tag->toPlainText().isEmpty()) 
	 {
		 disconnect(tag, SIGNAL(lostFocus(CDiagramTag*)), this, SLOT(editorLostFocus(CDiagramTag*)));
		 if (m_tags.contains(tag))
		 {
			 m_tags.removeAll(tag);
		 }
		 scene()->removeItem(tag);
		 tag->setParentItem(NULL);
         tag->deleteLater();
     }
	 else if (!m_tags.contains(tag))
	 {
		 m_tags.push_back(tag);
	 }
 }

CDiagramGadgetLayer::CDiagramGadgetLayer(CDiagram *parent)
	: CDiagramLayer(parent)
{
}

CDiagramGadgetLayer::~CDiagramGadgetLayer()
{
	m_gadgets.clear();
}

void CDiagramGadgetLayer::addGadget(QGraphicsItem *gadget)
{
	// The gadget is not added to the layer, instead
	// it is added to the scene directly.
	if (gadget && !m_gadgets.contains(gadget) )
	{
		m_p->addItem(gadget);
		gadget->setZValue( zValue() );
		m_gadgets.push_back(gadget);
	}
}

void CDiagramGadgetLayer::removeGadget(QGraphicsItem *gadget)
{
	// The gadget is removed from the scene
	if (gadget && m_gadgets.contains(gadget) )
	{
		m_p->removeItem(gadget);
		m_gadgets.removeAll(gadget);
	}
}

bool CDiagramGadgetLayer::containsGadget(QGraphicsItem *gadget)
{
	bool	r = false;

	if (m_gadgets.contains(gadget))
		r = true;
	return r;
}

void CDiagramGadgetLayer::setGadgetsVisible(bool value)
{
	QGraphicsItem	*gadget = NULL;

	for (int i = 0; i < m_gadgets.length(); ++i)
	{
		gadget = m_gadgets.at(i);
		gadget->setVisible(value);
	}
}

void CDiagramGadgetLayer::enterTemporaryHide()
{
	QGraphicsItem		*item = NULL;

	m_visibleGadgets.clear();
	for (int i = 0; i < m_gadgets.length(); ++i)
	{
		item = m_gadgets.at(i);
		if (item->isVisible())
			m_visibleGadgets.push_back(item);
		item->setVisible(false);
	}
}

void CDiagramGadgetLayer::exitTemporaryHide()
{
	QGraphicsItem		*item = NULL;

	for (int i = 0; i < m_visibleGadgets.length(); ++i)
	{
		item = m_visibleGadgets.at(i);
		item->setVisible(true);
	}
}