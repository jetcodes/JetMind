/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramItemHandler>

Qt::CursorShape CDiagramItemHandlerDragger::CursorShapes[3] =\
{\
	Qt::SizeAllCursor,\
	Qt::SizeVerCursor,\
	Qt::SizeHorCursor
};

CDiagramItemHandlerDragger::CDiagramItemHandlerDragger(CDiagramItemHandler* parent, int index, int size, int type)
	: QGraphicsItem(parent),
	  m_p(parent),
	  m_size(size),
	  m_used(true),
	  m_policy(Global::BothDirections),
	  m_index(index),
	  m_dragging(false),
	  m_hovered(false),
	  m_type(type)
{
	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);

	m_pen = QPen( QColor(0, 0, 0, 255), 1, Qt::SolidLine );
	m_brush = QBrush( QColor( 255, 153, 0 ) );

	setCursor(CDiagramItemHandlerDragger::CursorShapes[m_policy]);
}

QRectF CDiagramItemHandlerDragger::boundingRect() const
{
	qreal			hs;
	
	hs = (m_size + m_pen.widthF() ) / 2.0;
	return QRectF( -hs, -hs, m_size + m_pen.widthF(), m_size + m_pen.widthF());
}

QPainterPath CDiagramItemHandlerDragger::shape() const
{
	QPainterPath	path;
	qreal			hs;
	
	if (m_path.isEmpty())
	{
		hs = m_size / 2.0;	
		path.addRect(-hs, -hs, m_size, m_size);
	}
	else
	{
		path.addRect( m_path.boundingRect() );
	}
	return path;
}

void CDiagramItemHandlerDragger::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	qreal				hs;
	QLinearGradient		lineGradient;

	hs = m_size / 2.0;
	m_path = computePath();

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	switch(m_type)
	{
	case Square:
		lineGradient = QLinearGradient(0, -hs, 0, m_size);
		lineGradient.setColorAt(0.0, m_brush.color() );
		lineGradient.setColorAt(1.0, QColor( 255, 255, 255, 255));

		m_path = computePath();
		painter->translate(0.5, 0.5);
		m_pen.setWidth(1);
		m_pen.setColor(Qt::gray);

		painter->setPen( m_pen );
		painter->setBrush( lineGradient );
		break;

	case Curve:
		painter->setPen( QColor(22, 110, 200, 255) );
		painter->setBrush( Qt::NoBrush );
		break;

	default:
		painter->setPen( QColor(22, 110, 200, 255) );
		painter->setBrush( QColor(22, 110, 200, 128) );
		break;
	};

	if (isDragging() || isHovered() )
	{
		painter->setPen( QPen(Qt::black, 1) );
		painter->setBrush( QBrush(QColor(22, 110, 200, 98)) );
	}

	painter->drawPath( m_path );
	painter->restore();	
}

void CDiagramItemHandlerDragger::hoverEnterEvent(QGraphicsSceneHoverEvent* event )
{
	m_hovered = true;
	update();
}

void CDiagramItemHandlerDragger::hoverLeaveEvent(QGraphicsSceneHoverEvent* event )
{
	m_hovered = false;
	update();
}

void CDiagramItemHandlerDragger::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		event->ignore();
	}
	else
	{
		setDragging(true);
		update();
		m_p->grabFocus();
		QGraphicsItem::mousePressEvent(event);
		m_p->startDragging(this);
	}
}

void CDiagramItemHandlerDragger::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{	
	if (isDragging())
	{
		setPos( mapToParent(event->pos()) );
		m_p->doDragging(this);
	}
	else
	{
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void CDiagramItemHandlerDragger::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() != Qt::LeftButton || !isDragging() )
	{
		event->ignore();
		return;
	}
	else
	{
		m_p->finishDragging(this);
		setDragging(false);
		update();
		QGraphicsItem::mouseReleaseEvent(event);	
	}
}

const QPainterPath CDiagramItemHandlerDragger::computePath()
{
	qreal			w;
	QPainterPath	path;

	w = m_size / 2.0;
	switch (m_type)
	{
	default:
	case Diamond:
		path.moveTo(0, -w);
		path.lineTo(-w, 0);
		path.lineTo(0, w);
		path.lineTo(w, 0);
		path.closeSubpath();
		break;

	case Circle:
		path.addEllipse(-w, -w, m_size, m_size);
		break;

	case Square:
		path.addRect(-w, -w, m_size, m_size);
		break;

	case Curve:
		QPainterPathStroker	stroker;
		path.moveTo(-w, w);
		path.quadTo(QPointF(0, -w/2.0), QPointF(w, w));
		path.moveTo(QPointF(w/2.0, w));
		path.lineTo(QPointF(w, w));
		path.moveTo(QPointF(w, w/2));
		path.lineTo(QPointF(w, w));
		path.moveTo(QPointF(-w/2.0, w));
		path.lineTo(QPointF(-w, w));
		path.moveTo(QPointF(-w, w/2));
		path.lineTo(QPointF(-w, w));
		path = stroker.createStroke(path);
		break;

	}; // End of switch
	return path;
}

void CDiagramItemHandlerDragger::setDragPolicy(Global::DragPolicy p)
{
	m_policy = p;
	setCursor( CDiagramItemHandlerDragger::CursorShapes[p] );
}

void CDiagramItemHandlerDragger::setUsed(bool value)
{
	m_used = value;
	setVisible(value);
}

void CDiagramItemHandlerDragger::resetPen()
{
	m_pen = QPen( QColor(0, 0, 0, 255), 1, Qt::SolidLine );
}

void CDiagramItemHandlerDragger::setPen(const QPen &pen)
{
	m_pen = pen;
}

QPen CDiagramItemHandlerDragger::pen()
{
	return m_pen;
}

void CDiagramItemHandlerDragger::resetBrush()
{
	// m_brush = QBrush( QColor(161, 173, 215) );
	m_brush = QBrush( QColor( 255, 153, 0 ) );
}

void CDiagramItemHandlerDragger::setBrush(const QBrush &brush)
{
	m_brush = brush;
}

QBrush CDiagramItemHandlerDragger::brush()
{
	return m_brush;
}

CDiagramItemHandler::CDiagramItemHandler()
	: m_d(NULL),
	  m_width(100),
	  m_height(100),
	  m_draggerSize(6),
	  m_dragger(NULL)
{
	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	
	// m_pen = QPen( QColor( QString::fromUtf8("#00A8FF") ), 1, Qt::SolidLine );
	m_pen = QPen(Qt::gray, 1, Qt::SolidLine);
	initDraggers();
}

CDiagramItemHandler::~CDiagramItemHandler()
{
}

void CDiagramItemHandler::initDraggers()
{
	// TO BE OVERRIEDED IN SUBCLASSES
}

QRectF CDiagramItemHandler::boundingRect() const
{
	qreal			hs, hw, hh;	

	hs = m_pen.widthF() / 2.0;
	hw = m_width / 2.0;
	hh = m_height / 2.0;	

	return QRectF(-hw-hs, -hh-hs, m_width + m_pen.widthF(), m_height + m_pen.widthF() );
}

QPainterPath CDiagramItemHandler::shape() const
{
	QRectF					rect;
	QPainterPath			path;
	CDiagramItemHandlerDragger	*d = NULL;

	foreach (d, m_draggers)
	{
		if (d->isVisible())
		{
			rect = d->boundingRect();
			rect.moveCenter(d->pos());
			path.addRect(rect);
		}
	}
	return path;
}

void CDiagramItemHandler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->save();
	painter->setPen( m_pen );
	painter->drawRect(-m_width/2.0, -m_height/2.0, m_width, m_height);
	painter->restore();
}

void CDiagramItemHandler::layoutDraggers()
{
	// TO BE OVERRIDED IN SUBCLASSES
}

void CDiagramItemHandler::startDragging(CDiagramItemHandlerDragger *dragger)
{
	// TO BE OVERRIDED IN SUBCLASSES
	m_dragger = dragger;
}

void CDiagramItemHandler::doDragging(CDiagramItemHandlerDragger *dragger)
{
	// TO BE OVERRIDED IN SUBCLASSES
}

void CDiagramItemHandler::finishDragging(CDiagramItemHandlerDragger *dragger)
{
	// TO BE OVERRIDED IN SUBCLASSES
	m_dragger = NULL;
}

void CDiagramItemHandler::onBoundedItemsDragging()
{
	if (isChanging())
		return;
	
	setVisible(false);
}

void CDiagramItemHandler::onBoundedItemsDraggingStop(const QRectF& rect)
{
	foreach (CDiagramItemHandlerDragger *d, m_draggers)
	{
		d->setDragging(false);
	}

	setVisible(true);
	setGeometry( QRectF(rect.center(), rect.size()) );
}

void CDiagramItemHandler::onBoundedItemsMoving(qreal dx, qreal dy)
{
	moveBy(dx, dy);
}

void CDiagramItemHandler::onBoundedItemsGeometryChanged(const QRectF &rect)
{
	setGeometry( QRectF( rect.center(), rect.size() ) );
}

void CDiagramItemHandler::onBoundedItemsRotationChanged(const qreal value)
{
	prepareGeometryChange();

	setRotation(rotation() + value);
	layout();
}

void CDiagramItemHandler::setGeometry( const QRectF &rect)
{
	prepareGeometryChange();

	setPos( rect.topLeft() );
	m_width = rect.width();
	m_height = rect.height();
	layout();
}

bool CDiagramItemHandler::isChanging()
{
	bool		r = false;

	foreach (CDiagramItemHandlerDragger *d, m_draggers) 
	{
		if (d->isDragging())
		{
			r = true;
			break;
		}
	}
	return r;
}

void CDiagramItemHandler::grabFocus()
{
	Q_ASSERT(m_d);
	// m_d->itemLostFocus();
}

void CDiagramItemHandler::init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect)
{
	if (!d || m_d == d || items.length() < 1) 
		return;

	if (scene() != d)
	{
		d->addGadget(this, CDiagram::HandlerLayer);
		connect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onBoundedItemsMoving(qreal, qreal)));
		connect(d, SIGNAL(itemsDragging()), this, SLOT(onBoundedItemsDragging()));
		connect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onBoundedItemsDraggingStop(const QRectF&)));
		connect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onBoundedItemsGeometryChanged(const QRectF&)));
		connect(d, SIGNAL(itemsRotationChanged(const qreal)),this, SLOT(onBoundedItemsRotationChanged(const qreal)));
	}

	m_items = items;
	setVisible(true);
	setGeometry( QRectF(rect.center(), QSizeF(rect.width(), rect.height()) ) );
	m_d = d;
}

void CDiagramItemHandler::uninit(CDiagram *d)
{
	if (!d || m_d != d)
		return;

	if (scene() == d)
	{
		disconnect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onBoundedItemsMoving(qreal, qreal)));
		disconnect(d, SIGNAL(itemsDragging()), this, SLOT(onBoundedItemsDragging()));
		disconnect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onBoundedItemsDraggingStop(const QRectF&)));
		disconnect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onBoundedItemsGeometryChanged(const QRectF&)));
		disconnect(d, SIGNAL(itemsRotationChanged(const qreal)),this, SLOT(onBoundedItemsRotationChanged(const qreal)));
		d->removeGadget(this, CDiagram::HandlerLayer);
	}

	m_items.clear();
	setVisible(false);
	m_d = NULL;
}

void CDiagramItemHandler::layout()
{
	prepareGeometryChange();

	layoutDraggers();
}

bool CDiagramItemHandler::support(const QList<CDiagramItem *> &items)
{
	// TO BE Overrieded.
	return false;
}
