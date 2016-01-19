/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramContext>
#include <core/CDiagramWidget>
#include <core/CDiagramDemoToolBar>

CDiagramDemoToolBar::CDiagramDemoToolBar()
	: CDiagramToolBar(NULL)
{	
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, false);

	setAutoHide(false);
	setAnchorType(CDiagramWidget::FreeInScene);
	setVisible(false);
}

CDiagramDemoToolBar::~CDiagramDemoToolBar()
{
}

void CDiagramDemoToolBar::init(CDiagram *d)
{
	QPointF			p;
	QRectF			sc, r;
	QGraphicsView	*v = NULL;

	if (!d) 
		return;

	v = d->getMainView();
	if (v)
	{
		sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
		r = m_proxy->geometry();
		p = sc.bottomRight() - QPointF(r.width(), r.height()) - QPointF(5, 5);

		// Mark the position
		m_proxy->setPos(p);
		m_viewPos = v->mapFromScene(p);
	}

	d->addGadget(this, CDiagram::ControlLayer );
	connect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	connect(d, SIGNAL(visibleRectChanged(const QRectF&)), this, SLOT(onDiagramVisibleRectChanged(const QRectF&)));

	CDiagramWidget::init(d);
}

void CDiagramDemoToolBar::uninit(CDiagram *d)
{
	if (!d) return;

	disconnect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	disconnect(d, SIGNAL(visibleRectChanged(const QRectF&)), this, SLOT(onDiagramVisibleRectChanged(const QRectF&)));

	d->removeGadget(this, CDiagram::ControlLayer );

	CDiagramWidget::uninit(d);
}

void CDiagramDemoToolBar::onDiagramModeChanged(CDiagram::DiagramMode mode)
{
	QPointF					p;
	QRectF					r;
	QRectF					sc;
	QGraphicsView			*v = NULL;
	CDiagramHostedWidget	*widget = NULL, *currentWidget = NULL;

	if (!m_d) 
		return;
	
	if (mode == CDiagram::TagMode)
	{
		setHostedWidget( getPreferredHostedWidget() );
		setVisible(true);
		setOpacity(0.5);
		v = m_d->getMainView();
		sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
		r = m_proxy->geometry();
		
		p = sc.bottomRight() - QPointF(r.width(), r.height()) - QPointF(5, 5);

		// Mark the position
		m_proxy->setPos(p);
		m_viewPos = v->mapFromScene(p);
	}
	else
	{
		setVisible(false);
	}
}

void CDiagramDemoToolBar::onDiagramVisibleRectChanged(const QRectF &sc)
{
	QRectF			r;
	QPointF			p;
	QGraphicsView	*v = NULL;

	if (!m_d) return;

	v = m_d->getMainView();
	r = m_proxy->geometry();
	p = sc.bottomRight() - QPointF(r.width(), r.height()) - QPointF(5, 5);
	// Mark the position
	m_proxy->setPos(p);
	m_viewPos = v->mapFromScene(p);
}

void CDiagramDemoToolBar::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	setOpacity(1.0);
}

void CDiagramDemoToolBar::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	setOpacity(1.0);
}