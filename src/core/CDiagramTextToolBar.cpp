/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramText>
#include <core/CDiagramWidget>
#include <core/CDiagramTextToolBar>

CDiagramTextToolBar::CDiagramTextToolBar()
	: CDiagramWidget(NULL),
	  m_text(NULL)
{
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, false);

	setAutoHide(true);
	setAnchorType(CDiagramWidget::AnchorToSelectedItem);
	setVisible(false);
}

CDiagramTextToolBar::~CDiagramTextToolBar()
{
}

void CDiagramTextToolBar::init(CDiagram *d)
{
	if (!d) return;

	d->addGadget(this, CDiagram::ControlLayer );
	connect(d, SIGNAL(editingText(CDiagramText*)), this, SLOT(onTextEditRequest(CDiagramText*)));
	connect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	
	CDiagramWidget::init(d);
}

void CDiagramTextToolBar::uninit(CDiagram *d)
{
	if (!d) return;

	disconnect(d, SIGNAL(editingText(CDiagramText*)), this, SLOT(onTextEditRequest(CDiagramText*)));
	disconnect(d, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	d->removeGadget(this, CDiagram::ControlLayer );

	CDiagramWidget::uninit(d);
}

void CDiagramTextToolBar::onTextEditRequest(CDiagramText *text)
{
	if (!text || !text->styleable() )
		return;

	setHostedWidget( getPreferredHostedWidget() );

	// connect the signal
	connect(text, SIGNAL(sizeAutoChanged()), this, SLOT(onTextSizeChanged()));

	setVisible(true);
	m_text = text;

	updatePosition();
}

void CDiagramTextToolBar::onTextSizeChanged()
{
	updatePosition();
}

void CDiagramTextToolBar::onDiagramModeChanged(CDiagram::DiagramMode mode)
{
	QTextDocument	*doc = NULL;

	if (mode != CDiagram::EditTextMode)
	{
		if (m_text)
		{
			setHostedWidget( getPreferredHostedWidget() );
			disconnect(m_text, SIGNAL(sizeAutoChanged()), this, SLOT(onTextSizeChanged()));
		}
		setVisible(false);
		m_text = NULL;
	}
}

void CDiagramTextToolBar::updatePosition()
{
	QRectF		r;
	QPointF		p;

	if (!m_text)
		return;

	r = m_text->mapToScene(m_text->boundingRect()).boundingRect();
	// the rect passed contains the center of the CDiagramText as the topleft
	// of the rect, and the size of the CDiagramText as the size
	p = r.topLeft();
	if (m_proxy)
	{
		QPointF			p0, p1, p2, p3, p4;
		QRectF			r1, sc;
		qreal			delta = 10;
		QGraphicsView	*v = NULL;
		QRectF			rect = r;

		v = m_d->getMainView();
		sc = v->mapToScene(v->viewport()->geometry()).boundingRect();

		// MUST use proxy's geometry, instead of control's boundingRect
		r1 = m_proxy->geometry();

		p1 = QPointF(rect.left() - r1.width() - delta, rect.top() );
		p2 = QPointF(rect.right() + delta, rect.top() );
		if (p2.x() + r1.width() < sc.right() )
			p0 = p2;
		else
			p0 = p1;
        p0 = mapFromScene(p0);
        m_proxy->setPos(QPointF(qRound(p0.x()), qRound(p0.y())));
	}
}
