/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramProxyWidget>

CDiagramProxyWidget::CDiagramProxyWidget(CDiagram *d)
	: CDiagramNode(d),
	  m_widget(NULL),
	  m_proxy(NULL)
{
	m_proxy = new QGraphicsProxyWidget(this);
	m_proxy->setParentItem(this);
	m_proxy->setVisible(true);
	
	connect(this, SIGNAL(geometryChanged()), this, SLOT(onGeometryChanged()));
}

CDiagramProxyWidget::~CDiagramProxyWidget()
{
	SAFE_DELETE(m_proxy);	
}


void CDiagramProxyWidget::onGeometryChanged()
{
	QRectF	r;
	QSize	size;
	
	r = geometry();
	size = r.size().toSize();
	m_widget->resize(size);
	layout();
}

QRectF CDiagramProxyWidget::boundingRect() const
{
	QSize	s;
	qreal	w, h;

	if (m_widget)
		s = m_widget->size();
	w = s.width();
	h = s.height();

	return QRectF(-w/2.0, -h/2.0, w, h);
}

QPainterPath CDiagramProxyWidget::shape() const
{
	QPainterPath	path;

	path.addRect( boundingRect() );
	return path;
}

void CDiagramProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	Q_UNUSED(painter);
}

void CDiagramProxyWidget::setWidget(QWidget *value)
{
	if (m_proxy)
	{
		m_widget = value;
		m_proxy->setWidget(m_widget);
		m_proxy->setVisible(true);
		resize(QSizeF(m_widget->width(), m_widget->height()));
		layout();
		update();
	}
}

QWidget* CDiagramProxyWidget::widget()
{
	QWidget		*w = NULL;

	if (m_proxy)
		w = m_proxy->widget();
	return w;
}

void CDiagramProxyWidget::layout()
{
	QRectF	rect;

	rect = boundingRect();
	m_proxy->setGeometry(QRectF(rect.topLeft(), rect.size() ) );
}

bool CDiagramProxyWidget::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
	QGraphicsSceneMouseEvent	*mouseEvent = NULL;

	mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
	if (mouseEvent && mouseEvent->buttons() & Qt::LeftButton)
		setSelected(true);
	return scene()->sendEvent(m_proxy, event);
}
