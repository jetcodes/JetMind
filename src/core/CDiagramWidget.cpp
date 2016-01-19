/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramEditor>
#include <core/CDiagramWidget>


bool greaterThan(CDiagramHostedWidget *s1, CDiagramHostedWidget *s2)
{
	bool	r = true;

	if (s1 == NULL && s2 == NULL) 
		r = false;
	else if (s1 == NULL)
		r = false;
	else if (s2 == NULL)
		r = true;
	else
		r = s1->priority() > s2->priority(); 

	return r;
}

CDiagramTool::CDiagramTool(QWidget *parent)
	: QWidget(parent),
	  m_d(NULL),
	  m_p(NULL)
{
	
}

bool CDiagramTool::support(const QList<CDiagramItem*> &items)
{
	return true;
}

void CDiagramTool::init(CDiagram *d)
{
	m_d = d;	
}

void CDiagramTool::uninit(CDiagram *d)
{
	m_d = NULL;
}

void CDiagramTool::reset()
{
}

void CDiagramTool::setHostWidget(CDiagramHostedWidget *w)
{
	m_p = w;
}

CDiagramHostedWidget* CDiagramTool::hostWidget()
{
	return m_p;
}

CDiagramHostedWidget::CDiagramHostedWidget(QWidget *parent)
	: QWidget(parent),
	  m_priority(0),
	  m_parent(NULL),
	  m_d(NULL)
{
}

CDiagramHostedWidget::~CDiagramHostedWidget()
{
}

bool CDiagramHostedWidget::support(CDiagram *d)
{
	return true;
}

void CDiagramHostedWidget::init(CDiagramWidget *parent, CDiagram *d)
{
	m_parent = parent;
	m_d = d;

	foreach (CDiagramTool *t, m_tools)
	{
		t->init(d);
	}
}

void CDiagramHostedWidget::uninit(CDiagramWidget *parent, CDiagram *d)
{
	if (!parent || !d || m_parent != parent || d != m_d)
		return;

	foreach (CDiagramTool *t, m_tools)
	{
		t->uninit(d);
	}

	m_parent = NULL;
	m_d = NULL;
}

void CDiagramHostedWidget::reset()
{
	// TO BE IMPLEMENTED IN SUBCLASSES
}

void CDiagramHostedWidget::setPriority(int value)
{
	m_priority = value;
}

int CDiagramHostedWidget::priority()
{
	return m_priority;
}

bool CDiagramHostedWidget::customLocate()
{
	return false;
}

QPointF CDiagramHostedWidget::preferredPos()
{
	return QPointF();
}

void CDiagramHostedWidget::addTool(CDiagramTool *tool)
{
	if (! m_tools.contains(tool) )
		m_tools.push_back(tool);
}

QList<CDiagramTool*> CDiagramHostedWidget::tools()
{
	return m_tools;
}

void CDiagramHostedWidget::initTools()
{
	// TO BE IMPLEMENTED IN SUBCLASSES
}

CDiagramWidget* CDiagramHostedWidget::diagramWidget() const
{
    return m_parent;
}
/*
 * QGraphicsProxyWidget has to be the top-level item or has the same
 * coordination system with the scene, or the embeded child widgets will not
 * be popuped in the right position. 
 *
 * We make the widget takes the whole scene as the boundingRect, and the shape
 * of the proxy widget as the hit test shape. Because we won't draw anything in
 * a widget, so it will not effect the performance.
 */
CDiagramWidget::CDiagramWidget(CDiagram *d)
	: QGraphicsObject(NULL),
	  m_d(d),
	  m_proxy(NULL),
	  m_activeHostedWidget(NULL),
	  m_moving(false),
	  m_width(0),
	  m_height(0),
	  m_autoHide(false),
	  m_hovered(false),
	  m_boxSize( QSizeF(32, 32) ),
      m_anchorType(CDiagramWidget::AnchorToSceneRight)
{
	QGraphicsDropShadowEffect	*effect = NULL;

	m_proxy = new QGraphicsProxyWidget();
	m_proxy->setParentItem(this);
	m_proxy->setVisible(true);
    m_proxy->setAcceptHoverEvents(true);

	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsMovable);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	setAcceptHoverEvents(true);

	effect = new QGraphicsDropShadowEffect(this);
	effect->setColor( Qt::lightGray );
	effect->setOffset(4, 4);
	effect->setBlurRadius(8);
	//setGraphicsEffect(effect);
}

CDiagramWidget::~CDiagramWidget()
{
	SAFE_DELETE(m_proxy);
}

int CDiagramWidget::widgetType()
{
	return -1;
}

void CDiagramWidget::init(CDiagram *d)
{
	m_activeHostedWidget = NULL;
	m_d = d;
}

void CDiagramWidget::uninit(CDiagram *d)
{
	if (m_activeHostedWidget)
		m_activeHostedWidget->uninit(this, d);

	m_d = NULL;
}

void CDiagramWidget::setAnchorType(CDiagramWidget::AnchorType value)
{
	QGraphicsView		*v = NULL;
	QRectF				sc, r;
	qreal				x, y, paddingx = 6, paddingy = 6;
	
	m_anchorType = value;
	
	if (!m_d) return;
	v = m_d->getMainView();

	if (!v) return;
	sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
	
	if (!m_proxy) return;
	r = m_proxy->geometry();

	switch( m_anchorType )
	{
	case AnchorToSceneLeft:
		x = sc.left() + paddingx;
		y = sc.top() + paddingy;
        // m_proxy->setPos( QPointF(qRound(x), qRound(y)) );
		m_viewPos = QPointF(qRound(x), qRound(y));
		break;

	case AnchorToSceneRight:
		x = sc.right() - r.width() - paddingx;
		y = sc.top() + paddingy;
        // m_proxy->setPos( QPointF(qRound(x), qRound(y)) );
		m_viewPos = QPointF(qRound(x), qRound(y));
		break;

	default:
		break;
	}; // End of switch
}

CDiagramWidget::AnchorType CDiagramWidget::anchorType()
{
	return m_anchorType;
}

void CDiagramWidget::updateVisibility(bool value)
{
	if (autoHide())
	{
		setVisible(value);
	}
	else
	{
		if (!isVisible() && value)
			setVisible(true);
	}

	// if the anchor type is anchor left/right, we should update the position
	if (m_anchorType == CDiagramWidget::AnchorToSceneLeft ||
		m_anchorType == CDiagramWidget::AnchorToSceneRight)
		updatePosViaGeometryChange(QRectF());

	if (m_activeHostedWidget)
		m_activeHostedWidget->resize(m_activeHostedWidget->layout()->sizeHint());
}

void CDiagramWidget::setAutoHide(bool value)
{
	m_autoHide = value;
}

bool CDiagramWidget::autoHide()
{
	return m_autoHide;
}

QRectF CDiagramWidget::boundingRect() const
{
	QRectF		r;
	
	r = shape().boundingRect().adjusted(-6,-6,6,6);
	return r;
}

QPainterPath CDiagramWidget::shape() const
{
	QRectF					r;
	QPainterPath			path;
	QGraphicsItem			*item = NULL;
	QGraphicsProxyWidget	*proxy = NULL;
	QList<QGraphicsItem*>	items;

	r = m_proxy->geometry();
	r.adjust(-2, -2, 2, 2);
	path.setFillRule(Qt::WindingFill);
	path.addRect( r );
	items = m_proxy->childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (item->isVisible() && item->type() == QGraphicsProxyWidget::Type)
		{
			r = item->mapRectToParent(item->boundingRect());
			path.addRect(r);
		}
	}

	return path;
}

void CDiagramWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF			r;

	r = m_proxy->geometry();
	
	painter->save();
	//painter->setPen( QPen(Qt::lightGray, 1, Qt::SolidLine) );
	//painter->setBrush( QBrush(QColor(255,255,255) ) );
	//if (r.height() > 68)
	//{
	//	painter->setRenderHint(QPainter::Antialiasing, true);
	//	painter->drawRoundedRect(r, 8, 8);
	//}
	//else
	//{
	//	painter->setRenderHint(QPainter::Antialiasing, true);
	//	painter->drawRoundedRect(r, 4, 4);
	//}

	painter->restore();
}

void CDiagramWidget::addHostedWidget(CDiagramHostedWidget *widget)
{
	if (!widget)
		return;

	if (!m_hostedWidgets.contains(widget))
		m_hostedWidgets.push_back(widget);
	qSort(m_hostedWidgets.begin(), m_hostedWidgets.end(), greaterThan);
}

void CDiagramWidget::setHostedWidget(CDiagramHostedWidget *widget)
{
	if (m_proxy && widget && m_activeHostedWidget != widget)
	{
		if (m_activeHostedWidget)
			m_activeHostedWidget->uninit(this, m_d);

		if (widget)
		{
			widget->init(this, m_d);
			widget->reset();
			m_activeHostedWidget = widget;
			m_proxy->setWidget(m_activeHostedWidget);
			m_proxy->setVisible(true);
			m_proxy->resize( m_activeHostedWidget->width(), m_activeHostedWidget->height() );
		}
	}
	else if (widget && m_activeHostedWidget == widget)
	{
		widget->reset();
	}
}

CDiagramHostedWidget* CDiagramWidget::activeHostedWidget()
{
	CDiagramHostedWidget		*w = NULL;

	if (m_proxy)
		w = dynamic_cast<CDiagramHostedWidget*>( m_proxy->widget() );
	return w;
}

CDiagramHostedWidget* CDiagramWidget::getPreferredHostedWidget()
{
	CDiagramHostedWidget	*preferredWidget = NULL;

	foreach (CDiagramHostedWidget *widget, m_hostedWidgets)
	{
		if (widget->support(m_d))
		{
			preferredWidget = widget;
			break;
		}
	}

	return preferredWidget;
}

void CDiagramWidget::updatePosViaGeometryChange(const QRectF &rect)
{
    QPointF					p0, p1, p2, p3, p4;
    QRectF					sc;
    QRect                   r;
	qreal					delta = 10, x1, y1;
	qreal					paddingx = 6, paddingy = 6;
	QGraphicsView			*v = NULL;
    QWidget                 *w = NULL;
	CDiagramHostedWidget	*widget = NULL;

	widget = activeHostedWidget();
	if (widget && widget->customLocate())
	{
		m_proxy->setPos( mapFromScene(widget->preferredPos()) );
		return;
	}
	
	v = m_d->getMainView();
	sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
    w = m_proxy->widget();
    if (w)
        r = w->geometry();
    else
        r = m_proxy->geometry().toRect();
	
	switch( m_anchorType )
	{
	case AnchorToSelectedItem:
		p1 = QPointF(rect.left() - r.width() - delta, rect.top() );
		p2 = QPointF(rect.right() + delta, rect.top() );
		p3 = QPointF(rect.center().x() - r.width() / 2.0, rect.top() - r.height() - delta);
		p4 = QPointF(rect.center().x() - r.width() / 2.0, rect.bottom() + delta);

		if (p2.x() + r.width() < sc.right() ) // right side of the item
		{
			if (p2.y() <= sc.top())
            {
				p0 = QPointF(p2.x(), sc.top() + delta);
            }
			else
            {
                p0 = QPointF(p2.x(), qMin(p2.y(), sc.bottom() - r.height() ));
            }
		}
        else if (p1.x() - r.width() > sc.left()) // left side of the item
		{
			if (p1.y() <= sc.top())
            {
				p0 = QPointF(p1.x(), sc.top() + delta);
            }
			else
            {
                p0 = QPointF(p1.x(), qMin(p1.y(), sc.bottom() - r.height()));
            }
		}
		else if (p3.y() > sc.top())
		{
			p0 = p3;
		}
		else if (p4.y() + r.height() < sc.bottom())
		{
			p0 = p4;
		}
		else
		{
			p0 = sc.center();
		}
        p0 = mapFromScene(p0);
        m_proxy->setPos( QPointF(qRound(p0.x()), qRound(p0.y())) );
		m_viewPos = v->mapFromScene(p0);
		break;

	case AnchorToSceneLeft:
		p1 = v->mapToScene( m_viewPos.toPoint() );
		x1 = sc.left() + paddingx;
		y1 = sc.bottom() - r.height() - paddingy;
        p0 = QPointF(x1, qMax(sc.top() + paddingy, qMin(y1, p1.y()) ) );
        m_proxy->setPos( QPointF(qRound(p0.x()), qRound(p0.y())) );
        m_viewPos = v->mapFromScene(p0);
		break;

	case AnchorToSceneRight:
		p1 = v->mapToScene( m_viewPos.toPoint() );
		x1 = sc.right() - r.width() - paddingx;
		y1 = sc.bottom() - r.height() - paddingy;
        p0 = QPointF(x1, qMax(sc.top() + paddingy, qMin(y1, p1.y()) ) );
        m_proxy->setPos( QPointF(qRound(p0.x()), qRound(p0.y())) );
        m_viewPos = v->mapFromScene(p0);
		break;

	case FreeInScene:
	default:
		p1 = v->mapToScene( m_viewPos.toPoint() );
		x1 = sc.right() - r.width();
		y1 = sc.bottom() - r.height();
        p0 = QPointF(qMax(sc.left(), qMin(p1.x(), x1)), qMax(sc.top(), qMin(p1.y(), y1)));
        m_proxy->setPos( QPointF(qRound(p0.x()), qRound(p0.y())) );
		break;

	}; // End of switch

	// the update is critical, because it make the sprite to refresh itself
	update();
}

void CDiagramWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		m_moving = true;
		m_prev = event->scenePos();
	}
	QGraphicsObject::mousePressEvent(event);
}

void CDiagramWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF			p;
	QGraphicsView	*v = NULL;

	if ((event->buttons() & Qt::LeftButton) && m_moving)
	{
		v = m_d->getMainView();
		p = event->scenePos();
		m_proxy->moveBy(p.x() - m_prev.x(), p.y() - m_prev.y());
		m_prev = p;
		m_viewPos = v->mapFromScene(m_proxy->pos());
	}
	else
	{
		event->accept();
	}
}

void CDiagramWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	m_moving = false;
	QGraphicsObject::mouseReleaseEvent(event);

	// for anchor left/right, we should update the position right now
	if (m_anchorType == CDiagramWidget::AnchorToSceneLeft ||
		m_anchorType == CDiagramWidget::AnchorToSceneRight)
		updatePosViaGeometryChange(QRectF());
}

void CDiagramWidget::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	m_hovered = true;

	update();
}

void CDiagramWidget::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	m_hovered = false;

	update();
}

// CDiagramToolBar
CDiagramToolBar::CDiagramToolBar(CDiagram *d)
	: CDiagramWidget(d)
{
}

CDiagramToolBar::~CDiagramToolBar()
{
}

void CDiagramToolBar::onSelectionChanged(QList<CDiagramItem*> &, const QRectF &)
{
	// TO BE IMPLEMENTED
}

void CDiagramToolBar::onSelectedItemsMoving(qreal dx, qreal dy)
{
    QPointF     p;

	if (m_anchorType == CDiagramWidget::AnchorToSceneLeft ||
		m_anchorType == CDiagramWidget::AnchorToSceneRight)
    {
		updatePosViaGeometryChange(QRectF());
    }
	else
    {
        p = pos() + QPointF(dx, dy);
        setPos( QPointF(qRound(p.x()), qRound(p.y())) );
    }
}

void CDiagramToolBar::onSelectedItemsDragging()
{
	updateVisibility(false);
}

void CDiagramToolBar::onSelectedItemsDraggingStop(const QRectF &rect)
{
    updateVisibility(true);
	updatePosViaGeometryChange(rect);
}

void CDiagramToolBar::onSelectedItemsGeometryChanged(const QRectF &rect)
{
	updatePosViaGeometryChange(rect);
}

void CDiagramToolBar::onDiagramModeChanged(CDiagram::DiagramMode mode)
{
	if (!m_d) return;

	switch(mode)
	{
	case CDiagram::UserMode:
		if (m_d->getNewItemMeta() != NULL || m_d->getNewItem() != NULL)
			updateVisibility(false);
		break;

	case CDiagram::EditTextMode:
		updateVisibility(false);
		break;

	case CDiagram::TagMode:
		// Force to be unvisible
		setVisible(false);
		break;

	default:
		if (m_d->getItemsHasFocus().length() > 0)
			updateVisibility(true);
		break;
	};
}

void CDiagramToolBar::onDiagramVisibleRectChanged(const QRectF &rect)
{
	QRectF		r;

	if (!m_d) return;
	r = m_d->getItemsBoundingRect(true);
	updatePosViaGeometryChange(r);
}

void CDiagramToolBar::init(CDiagram *d)
{
	if (!d) return;

	d->addGadget(this, CDiagram::ControlLayer);
	connect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	connect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onSelectedItemsMoving(qreal, qreal)));
	connect(d, SIGNAL(itemsDragging()), this, SLOT(onSelectedItemsDragging()));
	connect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onSelectedItemsDraggingStop(const QRectF&)));
	connect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onSelectedItemsGeometryChanged(const QRectF&)));
	connect(d, SIGNAL(visibleRectChanged(const QRectF&)), this, SLOT(onDiagramVisibleRectChanged(const QRectF&)));
	connect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	
	CDiagramWidget::init(d);

	updateVisibility(false);
}

void CDiagramToolBar::uninit(CDiagram *d)
{
	if (!d || m_d != d) return;
	disconnect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	disconnect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onSelectedItemsMoving(qreal, qreal)));
	disconnect(d, SIGNAL(itemsDragging()), this, SLOT(onSelectedItemsDragging()));
	disconnect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onSelectedItemsDraggingStop(const QRectF&)));
	disconnect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onSelectedItemsGeometryChanged(const QRectF&)));
	disconnect(d, SIGNAL(visibleRectChanged(const QRectF&)), this, SLOT(onDiagramVisibleRectChanged(const QRectF&)));
	disconnect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	d->removeGadget(this, CDiagram::ControlLayer);

	CDiagramWidget::uninit(d);
}
