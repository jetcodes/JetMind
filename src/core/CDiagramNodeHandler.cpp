/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <limits>
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramPort>
#include <core/CDiagramCommandBuilder>
#include <core/CDiagramNodeHandler>

Qt::CursorShape CDiagramNodeHandler::CursorShapes[10] =\
{\
	Qt::SizeVerCursor,\
	Qt::SizeFDiagCursor,\
	Qt::SizeHorCursor,\
	Qt::SizeBDiagCursor,\
	Qt::SizeVerCursor,\
	Qt::SizeFDiagCursor,\
	Qt::SizeHorCursor,\
	Qt::SizeBDiagCursor, \
	Qt::SizeAllCursor,\
	Qt::OpenHandCursor
};

CDiagramNodeHandler::CDiagramNodeHandler()
	: CDiagramItemHandler(),
	  m_rotateRadius(30),
	  m_rotatable(true),
	  m_resizable(true),
	  m_anchorXLine(NULL),
	  m_anchorYLine(NULL)
{
	m_anchorXLine = new QGraphicsLineItem();
	m_anchorXLine->setFlag(QGraphicsItem::ItemIsSelectable, false);
	m_anchorXLine->setPen(QPen(Qt::darkGray, 1));

	m_anchorYLine = new QGraphicsLineItem();
	m_anchorYLine->setFlag(QGraphicsItem::ItemIsSelectable, false);
	m_anchorYLine->setPen(QPen(Qt::darkGray, 1));

	initDraggers();
}

void CDiagramNodeHandler::initDraggers()
{
	CDiagramItemHandlerDragger	*d = NULL;
	
	m_draggers.clear();
	for (int i=CDiagramNodeHandler::Top; i<=CDiagramNodeHandler::OutsideBox; ++i)
	{
		if (i == CDiagramNodeHandler::OutsideBox)
			d = new CDiagramItemHandlerDragger(this, i, m_draggerSize * 2, CDiagramItemHandlerDragger::Curve);
		else if (i == CDiagramNodeHandler::Center)
			d = new CDiagramItemHandlerDragger(this, i, m_draggerSize, CDiagramItemHandlerDragger::Circle);
		else
			d = new CDiagramItemHandlerDragger(this, i, m_draggerSize, CDiagramItemHandlerDragger::Square);
		d->setCursor( CDiagramNodeHandler::CursorShapes[i] );
		d->setVisible(true);
		d->setParentItem(this);
		m_draggers.push_back(d);
	}
	// m_draggers[CDiagramNodeHandler::Center]->setVisible(false);
}

void CDiagramNodeHandler::layoutDraggers()
{	
	int							index=0, i=0;	
	qreal						hw, hh;	
	qreal						degree;
	static int					x[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, 0};
	static int					y[10] = {-1, -1, 0, 1, 1, 1, 0, -1, 0, -1};
	CDiagramItemHandlerDragger  *d1 = NULL, *d2 = NULL;

	hw = m_width / 2.0;
	hh = m_height / 2.0;

	for (i=0;i < 10; ++i)
	{
		d1 = m_draggers[i];
		if (!d1->isDragging())
			d1->setPos( hw * x[i], hh * y[i]);
		else
			index = i;
	}

	// If the dragged one is not the rotation control point
	// then we should update the position here.
	if (index != CDiagramNodeHandler::OutsideBox)
	{
		d1 = m_draggers[index];
		d2 = m_draggers[CDiagramNodeHandler::OutsideBox];
		d1->setPos( hw * x[index], hh * y[index] );
		// d2->moveBy(m_rotateRadius, 0);
		d2->moveBy(0, -m_rotateRadius);
	}
	else
	{
		d1 = m_draggers[CDiagramNodeHandler::Center];
		d2 = m_draggers[CDiagramNodeHandler::OutsideBox];
		d2->setPos( d1->pos() + QPointF(0, -hh-m_rotateRadius) );
	}

	degree = qRound(rotation()) % 360;
	degree = (degree < 0 ? degree + 360 : degree);
	index = qFloor( (degree >= 22.5 ? (degree - 22.5) + 45 : degree) / 45 );

	for (int i=CDiagramNodeHandler::Top; i<=CDiagramNodeHandler::RightTop; ++i)
	{
		d1 = m_draggers[i];
		d1->setCursor( CDiagramNodeHandler::CursorShapes[ (CDiagramNodeHandler::RightTop + i - index + 1) % (CDiagramNodeHandler::RightTop + 1)] );
	}

	update();
}

QRectF CDiagramNodeHandler::boundingRect() const
{
	qreal			hs, hw, hh;	

	hs = m_pen.widthF() / 2.0;
	hw = m_width / 2.0;
	hh = m_height / 2.0;	

	return QRectF(-hw-hs, -hh-hs, m_width + m_pen.widthF() + m_rotateRadius, m_height + m_pen.widthF() );
}

QPainterPath CDiagramNodeHandler::shape() const
{
	QRectF							rect;
	QPainterPath					path;
	CDiagramItemHandlerDragger			*d = NULL;

	if (m_rotatable)
	{
		path.moveTo(m_draggers[CDiagramNodeHandler::Right]->pos());
		path.lineTo(m_draggers[CDiagramNodeHandler::OutsideBox]->pos());
	}

	foreach (d, m_draggers )
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

void CDiagramNodeHandler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	
	QRectF						r;
	Qt::PenStyle				old = m_pen.style();
	CDiagramItemHandlerDragger  *d1 = NULL, *d2 = NULL;

	r = QRectF(-m_width/2.0, -m_height/2.0, m_width, m_height);

	painter->save();
	painter->setPen( Qt::NoPen );
	painter->setBrush( QColor(0, 0, 0, 48) );
	painter->drawRect( r );

	if (m_rotatable)
	{
		d1 = m_draggers[CDiagramNodeHandler::Top];
		d2 = m_draggers[CDiagramNodeHandler::OutsideBox];
		r = d2->boundingRect();
		m_pen.setStyle( Qt::DotLine );
		painter->setPen( m_pen );
		painter->drawLine( d1->pos(), d2->mapToItem(this, QPointF(0, r.bottom()) ));
		m_pen.setStyle( old );
	}
	painter->restore();
}

void CDiagramNodeHandler::startDragging(CDiagramItemHandlerDragger *d)
{
	switch(d->index())
	{
	case CDiagramNodeHandler::OutsideBox:
		m_draggers[CDiagramNodeHandler::OutsideBox]->setCursor( Qt::ClosedHandCursor );
		break;

	default:
		break;
	};
}

void CDiagramNodeHandler::doDragging(CDiagramItemHandlerDragger *d)
{
	switch (d->index())
	{
	case CDiagramNodeHandler::Center:
		doMove(d);
		break;

	case CDiagramNodeHandler::OutsideBox:
		
		doRotate(d);
		break;

	default:
		doResize(d);
		break;
	};
}

void CDiagramNodeHandler::finishDragging(CDiagramItemHandlerDragger *d)
{
	if (!m_d)
		return;

	switch (d->index())
	{
	case CDiagramNodeHandler::Center:
		m_d->emitItemsDraggingStop();
		break;

	case CDiagramNodeHandler::OutsideBox:
		m_d->emitItemsRotatingStop();
		m_draggers[CDiagramNodeHandler::OutsideBox]->setCursor( CDiagramNodeHandler::CursorShapes[CDiagramNodeHandler::OutsideBox] );
		break;

	default:
		m_d->emitItemsResizingStop();
		if (m_anchorYLine->scene() == m_d)
			m_d->removeItem(m_anchorYLine);
		if (m_anchorXLine->scene() == m_d)
			m_d->removeItem(m_anchorXLine);
		break;
	};
}

void CDiagramNodeHandler::doResize(CDiagramItemHandlerDragger* d)
{
	int						index;
	qreal					w, h, xratio, yratio, bs = 2 * m_draggerSize;
	QPointF					op, c;
	CDiagramCommandBuilder	*b = NULL;
	QList<QRectF>			oldGeometry;
	QList<QRectF>			newGeometry;

	checkSizeAnchor(d);

	index = d->index();
	op = m_draggers[(index + 4) % 8]->pos();
	
	if (d->isDragging() && m_resizable)
	{
		prepareGeometryChange();
		switch (index)
		{
		case CDiagramNodeHandler::Top:
			w = qMax(bs, m_width);
			h = qMax(bs, op.y() - d->y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x(), op.y() - h / 2.0 );
			break;

		case CDiagramNodeHandler::Bottom:
			w = qMax(bs, m_width);
			h = qMax(bs, d->y() - op.y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x(), op.y() + h / 2.0);
			break;

		case CDiagramNodeHandler::Left:
			w = qMax(bs, op.x() - d->x());
			h = qMax(bs, m_height);
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() - w / 2.0, op.y() );
			break;

		case CDiagramNodeHandler::Right:
			w = qMax(bs, d->x() - op.x());
			h = qMax(bs, m_height);
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() + w / 2.0, op.y() );
			break;

		case CDiagramNodeHandler::LeftBottom:
			w = qMax(bs, op.x() - d->x());
			h = qMax(bs, d->y() - op.y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() - w / 2.0, op.y() + h / 2.0);
			break;

		case CDiagramNodeHandler::LeftTop:
			w = qMax(bs, op.x() - d->x());
			h = qMax(bs, op.y() - d->y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() - w / 2.0, op.y() - h / 2.0);
			break;

		case CDiagramNodeHandler::RightBottom:
			w = qMax(bs, d->x() - op.x());
			h = qMax(bs, d->y() - op.y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() + w / 2.0, op.y() + h / 2.0);
			break;

		case CDiagramNodeHandler::RightTop:
			w = qMax(bs, d->x() - op.x());
			h = qMax(bs, op.y() - d->y());
			checkRatio(w, h, xratio, yratio);
			c = QPointF( op.x() + w / 2.0, op.y() - h / 2.0);
			break;

		default:
			break;
		};

		op = m_draggers[(index + 4) % 8]->pos();
		m_tmpGeometry = QRectF( mapToScene(c), QSizeF(w,h) );
		updateItemsGeometry(xratio, yratio, op, oldGeometry, newGeometry);
		b = CDiagramCommandBuilder::instance();
		m_d->addUndoCommand(b->createCommand(Global::CMD_RESIZE, 
											 Q_ARG(CDiagram*, m_d),
											 Q_ARG(QList<CDiagramItem*>, m_items),
											 Q_ARG(QList<QRectF>, oldGeometry),
											 Q_ARG(QList<QRectF>, newGeometry),
											 Q_ARG(QUndoCommand*, NULL) ));
	}
	layout();
}

void CDiagramNodeHandler::doRotate(CDiagramItemHandlerDragger* d)
{
	QLineF					line;
	bool					mark = false;
	qreal					delta, radius = 0;
	CDiagramNode			*node = NULL;
	CDiagramCommandBuilder	*b = NULL;

	if (d->isDragging() && m_rotatable)
	{
		prepareGeometryChange();

		line.setLine( 0, 0, d->pos().x(), d->pos().y() );
		delta = qRound( line.angleTo( QLineF(0, 0, 0, -10) ) );
		if (delta > 180.0)
			delta = delta - 360.0;

		if (m_d->selectedTopLevelItems().length() == 1)
		{
			node = dynamic_cast<CDiagramNode*>(m_d->selectedTopLevelItems().first());
			if (node)
			{
				radius = node->rotation() + delta;
				radius = qRound(radius / 45.0) * 45.0;
				mark = true;
			}
		}

		if (m_d->rotateStep() > 0 && (QApplication::keyboardModifiers() & Qt::ControlModifier))
		{
			if (mark)
			{
				delta = radius - node->rotation();
			}
			else
			{
				delta = qRound(delta / m_d->rotateStep() ) * m_d->rotateStep();
			}
		}
		else
		{
			if (mark && qAbs(delta - (radius - node->rotation())) < 10)
				delta = radius - node->rotation();
		}

		b = CDiagramCommandBuilder::instance();
		m_d->addUndoCommand(b->createCommand(Global::CMD_ROTATE, 
											 Q_ARG(CDiagram*, m_d),
											 Q_ARG(QPointF, mapToScene(0,0)),
											 Q_ARG(qreal, delta),
											 Q_ARG(QUndoCommand*, NULL)));
		layout();
	}
}

void CDiagramNodeHandler::doMove(CDiagramItemHandlerDragger *d)
{
	QPointF					p1, p2;
	qreal					dx, dy;
	CDiagramCommandBuilder	*b = NULL;

	p1 = mapToScene(0, 0);
	p2 = mapToScene(d->pos());

	dx = p2.x() - p1.x();
	dy = p2.y() - p1.y();

	if (d->isDragging())
	{
		prepareGeometryChange();
		b = CDiagramCommandBuilder::instance();
		m_d->addUndoCommand(b->createCommand(Global::CMD_MOVEITEMS, 
											 Q_ARG(CDiagram*, m_d),
                                             Q_ARG(QList<CDiagramItem*>, m_d->selectedTopLevelItems() ),
											 Q_ARG(qreal, dx),
											 Q_ARG(qreal, dy),
											 Q_ARG(QUndoCommand*, NULL) ) );
		layout();
	}
}

void CDiagramNodeHandler::onBoundedItemsDraggingStop(const QRectF& rect)
{
	foreach (CDiagramItemHandlerDragger *d, m_draggers)
	{
		d->setDragging(false);
	}

	setVisible(true);
	updateGeometry(rect);
}

void CDiagramNodeHandler::onBoundedItemsGeometryChanged(const QRectF &rect)
{
	updateGeometry(rect);
}

void CDiagramNodeHandler::onBoundedItemsPropertyChanged(const QString &name, const QVariant &value)
{
	if (name == "resizable")
	{
		setResizable(value.toBool());
	}
	update();
}

void CDiagramNodeHandler::checkRatio(qreal &w, qreal &h, qreal &xratio, qreal &yratio)
{
	qreal	delta = 0.001;
	qreal	dx, dy, dd;

	xratio = w / m_width;
	yratio = h / m_height;

	if (m_d->snapToGrid() || (QApplication::keyboardModifiers() & Qt::ControlModifier))
	{
		xratio = qRound( m_width * xratio / m_d->gridSize())  * m_d->gridSize() / m_width;
		yratio = qRound( m_height * yratio / m_d->gridSize()) * m_d->gridSize() / m_height;
	}
	else
	{
		m_width = qRound(m_width);
		m_height = qRound(m_height);
	}

	if (keepSizeRatio())
	{
		dx = qAbs(xratio - 1.0);
		dy = qAbs(yratio - 1.0);
		if (dx > dy)
			yratio = xratio;
		else
			xratio = yratio;
	}

	w = m_width * xratio;
	h = m_height * yratio;
}

bool CDiagramNodeHandler::support(const QList<CDiagramItem *> &items)
{
	bool			r = false;
	CDiagramNode	*item = NULL;

	if (items.length() == 1)
	{
		if (dynamic_cast<CDiagramNode*>(items.first()))
			r = true;
	}
	return r;
}

void CDiagramNodeHandler::init(CDiagram *d, QList<CDiagramItem *> &items, const QRectF &rect)
{
	bool						fixed = false;
	CDiagramItem				*item = NULL;
	CDiagramItemHandlerDragger	*dragger = NULL;

	// first call the parent to initialize the environment
	CDiagramItemHandler::init(d, items, rect);

	// setup the connections with items
	for (int i = 0; i < m_items.length(); ++i)
		connect(m_items.at(i), SIGNAL(propertyChanged(const QString &, const QVariant &)), this, SLOT(onBoundedItemsPropertyChanged(const QString &, const QVariant &) ));

	// clear up the draggers
	for (int i = 0; i < m_draggers.length(); ++i)
	{
		dragger = m_draggers.at(i);
		if (dragger && dragger->index() < CDiagramNodeHandler::Center)
		{
			dragger->setVisible(true);
		}
	}

	if (items.length() == 1)
	{
		item = items.at(0);
		if (item->sizePolicy().verticalPolicy() == QSizePolicy::Fixed)
		{
			m_draggers[Top]->setVisible(false);
			m_draggers[Bottom]->setVisible(false);
			fixed = true;
		}

		if (item->sizePolicy().horizontalPolicy() == QSizePolicy::Fixed)
		{
			m_draggers[Left]->setVisible(false);
			m_draggers[Right]->setVisible(false);
			fixed = true;
		}

		if (fixed || item->keepSizeRatio() )
		{
			m_draggers[LeftTop]->setVisible(false);
			m_draggers[LeftBottom]->setVisible(false);
			m_draggers[RightTop]->setVisible(false);
			m_draggers[RightBottom]->setVisible(false);
		}
		setResizable(item->resizable());
		setRotatable(item->rotatable());
	}

	m_draggers[Center]->setVisible(false);
	setVisible(true);
	updateGeometry(rect);
}

void CDiagramNodeHandler::uninit(CDiagram *d)
{
	for (int i = 0; i < m_items.length(); ++i)
		disconnect(m_items.at(i), SIGNAL(propertyChanged(const QString &, const QVariant &)), this, SLOT(onBoundedItemsPropertyChanged(const QString &, const QVariant &) ));
	
	// call the parent to uninitialize
	CDiagramItemHandler::uninit(d);
}

void CDiagramNodeHandler::updateGeometry(const QRectF &rect)
{
	CDiagramItem	*item = NULL;
	QRectF			r;

	if (m_items.length() != 1)
		return;

	item = m_items.at(0);
	r = item->contentsRect();

	setRotation( item->rotation() );
	setGeometry( QRectF( item->mapToScene(r.center()), r.size() * m_d->getScale() ) );
}

void CDiagramNodeHandler::updateItemsGeometry(qreal xratio, qreal yratio, QPointF &op, QList<QRectF> &oldGeometry, QList<QRectF> &newGeometry)
{
	QSizeF			minSize, newSize;
	QPointF			c;
	CDiagramItem	*item = NULL;

	item = m_items.first();
	if (!item)
		return;

	minSize = item->minimumSize();
	minSize = QSizeF( qMax(minSize.width(), 10.0), qMax(minSize.height(), 10.0) );
	newSize = QSizeF( qMax(item->width() * xratio, minSize.width()), qMax(item->height() * yratio, minSize.height()));
	
	if (item->width() > 0.1)
		xratio = newSize.width() / item->width();
	if (item->height() > 0.1)
		yratio = newSize.height() / item->height();
	
	c = item->mapToItem(this, QPointF(0,0) );
	c = QPointF(op.x() + (c.x() - op.x()) * xratio, op.y() + (c.y() - op.y() ) * yratio) ;
	newGeometry.push_back( QRectF(mapToItem(item->parentItem(), c), newSize) );
	oldGeometry.push_back( item->geometry() );
}

void CDiagramNodeHandler::applyLimits()
{
}

bool CDiagramNodeHandler::keepSizeRatio()
{
	bool	r = false;

	foreach (CDiagramItem *t, m_items)
	{
		if (t->keepSizeRatio())
		{
			r = true;
			break;
		}
	}
	return r;
}

void CDiagramNodeHandler::setResizable(bool value)
{
	CDiagramItemHandlerDragger	*dragger = NULL;

	for (int i = 0; i < m_draggers.length(); ++i)
	{
		dragger = m_draggers.at(i);
		if (dragger && dragger->index() < CDiagramNodeHandler::Center)
		{
			if (!value)
			{
				dragger->setPen( QPen(Qt::red) );
				dragger->setBrush( QBrush(Qt::red) );
			}
			else
			{
				dragger->resetPen();
				dragger->resetBrush();
			}
		}
	}
	m_resizable = value;
	layout();
}

void CDiagramNodeHandler::setRotatable(bool value)
{
	m_rotatable = value;
	m_draggers[CDiagramNodeHandler::OutsideBox]->setVisible(value);
	m_draggers[CDiagramNodeHandler::Center]->setVisible(value);
	layout();
}

void CDiagramNodeHandler::checkSizeAnchor(CDiagramItemHandlerDragger *dragger)
{
	qreal						x, rx, dx, mx = -1;
	qreal						y, ry, dy, my = -1;
	QRectF						r1, r2, rect;
	QPointF						p;
	CDiagramItem				*item = NULL;
	QList<CDiagramItem*>		topItems;
	QGraphicsView				*v = NULL;

	topItems = m_d->topLevelItems();
	r1 = boundingRect();
	rx = x = dragger->x();
	ry = y = dragger->y();

	// calculate the related visual rect of scene in the view
	v = m_d->getMainView();
	rect = v->mapToScene(v->viewport()->geometry()).boundingRect();
	for (int i = 0; i < topItems.length(); ++i)
	{
		item = topItems.at(i);

		// skip checking the selected items
		// only check unselected nodes
		if (item->isSelected() || item->interType() != Global::Node)
			continue;
		r2 = item->mapRectToItem(this, item->contentsRect() );
	/*
		if ( (qAbs( r2.left() - r1.right() ) > r2.width() + r1.width() && qAbs( r2.top() - r1.bottom() ) > r2.height() + r1.height() )||
			 (qAbs( r2.right() - r1.left() ) > r2.width() + r1.width() && qAbs( r2.bottom() - r1.top() ) > r2.height() + r1.height()) )
			continue;
    */
		switch(dragger->index())
		{
		case CDiagramNodeHandler::Top:
			dy = qAbs(y - r2.top());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.top();
			}
			break;

		case CDiagramNodeHandler::Bottom:
			dy = qAbs(y - r2.bottom());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.bottom();
			}
			break;

		case CDiagramNodeHandler::Left:
			dx = qAbs(x - r2.left());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.left();
			}
			break;

		case CDiagramNodeHandler::Right:
			dx = qAbs(x - r2.right());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.right();
			}
			break;

		case CDiagramNodeHandler::LeftBottom:
			dx = qAbs(x - r2.left());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.left();
			}
			dy = qAbs(y - r2.bottom());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.bottom();
			}
			break;

		case CDiagramNodeHandler::LeftTop:
			dx = qAbs(x - r2.left());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.left();
			}
			dy = qAbs(y - r2.top());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.top();
			}
			break;

		case CDiagramNodeHandler::RightBottom:
			dx = qAbs(x - r2.right());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.right();
			}
			dy = qAbs(y - r2.bottom());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.bottom();
			}
			break;

		case CDiagramNodeHandler::RightTop:
			dx = qAbs(x - r2.right());
			if (dx < mx || mx < 0)
			{
				mx = dx;
				rx = r2.right();
			}
			dy = qAbs(y - r2.top());
			if (dy < my || my < 0)
			{
				my = dy;
				ry = r2.top();
			}
			break;

		default:
			break;
		};
	}

	if (0 <= mx && mx <= 3)
	{
		if (m_anchorXLine->scene() != m_d)
		{
			m_d->addItem(m_anchorXLine);
		}
		p = mapToScene(rx, ry);
		m_anchorXLine->setLine(QLineF(p.x(), rect.top(), p.x(), rect.bottom()));
		dragger->setX(rx);
	}
	else if (m_anchorXLine->scene() == m_d)
	{
		m_d->removeItem(m_anchorXLine);
	}

	if (0 <= my && my <= 3)
	{
		if (m_anchorYLine->scene() != m_d)
		{
			m_d->addItem(m_anchorYLine);
		}
		p = mapToScene(rx, ry);
		m_anchorYLine->setLine(QLineF(rect.left(), p.y(), rect.right(), p.y()));
		dragger->setY(ry);
	}
	else if (m_anchorYLine->scene() == m_d)
	{
		m_d->removeItem(m_anchorYLine);
	}
}
