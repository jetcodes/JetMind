/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramNode>
#include <core/CDiagramSprite>
#include <core/CDiagramItemGeometryNotifier>

#ifndef SPACING
#define SPACING 24
#endif // SPACING

CDiagramItemGeometryNotifier::CDiagramItemGeometryNotifier()
	: CDiagramSprite(NULL),
	  m_triggered(false)
{
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);

	m_text = new QGraphicsTextItem();
	m_text->setParentItem(this);
	m_text->setVisible(true);
	m_text->setFont(QFont("Arial", 12, QFont::Bold));
	m_text->setDefaultTextColor( Qt::white );
}

CDiagramItemGeometryNotifier::~CDiagramItemGeometryNotifier()
{
	if (m_text)
	{
		delete m_text;
		m_text = NULL;
	}
}


QRectF CDiagramItemGeometryNotifier::boundingRect() const
{
	QRectF		r;

	r = m_text->boundingRect();
	return QRectF( -r.width() / 2.0, -r.height() / 2.0, r.width(), r.height() );
}

QPainterPath CDiagramItemGeometryNotifier::shape() const
{
	QPainterPath	path;

	path.addRect(boundingRect());
	return path;
}

void CDiagramItemGeometryNotifier::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush( QBrush( QColor(0, 0, 0, 128) ) ); 
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->drawRoundedRect( boundingRect(), 4, 4 );
	painter->restore();
}

void CDiagramItemGeometryNotifier::init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect)
{
	if (!d) return;

	d->addGadget(this, CDiagram::HandlerLayer);
	//connect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*> &, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF&)));
	connect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onBoundedItemsMoving(qreal, qreal)));
	connect(d, SIGNAL(itemsDragging()), this, SLOT(onBoundedItemsDragging()));
	connect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onBoundedItemsDraggingStop(const QRectF&)));
	connect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onBoundedItemsGeometryChanged(const QRectF&)));
	connect(d, SIGNAL(itemsRotationChanged(const qreal)),this, SLOT(onBoundedItemsRotationChanged(const qreal)));
	connect(d, SIGNAL(itemsResizingStop(const QRectF&)), this, SLOT(onBoundedItemsResizingStop(const QRectF &)));
	connect(d, SIGNAL(itemsRotatingStop(qreal)), this, SLOT(onBoundedItemsRotatingStop(qreal)));
	setVisible(false);

	m_d = d;
    reset(items, rect);
}

void CDiagramItemGeometryNotifier::uninit(CDiagram *d)
{
	if (!d) return;
	
	//disconnect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*> &, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF&)));
	disconnect(d, SIGNAL(itemsMoving(qreal, qreal)), this, SLOT(onBoundedItemsMoving(qreal, qreal)));
	disconnect(d, SIGNAL(itemsDragging()), this, SLOT(onBoundedItemsDragging()));
	disconnect(d, SIGNAL(itemsDraggingStop(const QRectF&)), this, SLOT(onBoundedItemsDraggingStop(const QRectF&)));
	disconnect(d, SIGNAL(itemsGeometryChanged(const QRectF&)),this, SLOT(onBoundedItemsGeometryChanged(const QRectF&)));
	disconnect(d, SIGNAL(itemsRotationChanged(const qreal)),this, SLOT(onBoundedItemsRotationChanged(const qreal)));
	disconnect(d, SIGNAL(itemsResizingStop(const QRectF&)), this, SLOT(onBoundedItemsResizingStop(const QRectF &)));
	disconnect(d, SIGNAL(itemsRotatingStop(qreal)), this, SLOT(onBoundedItemsRotatingStop(qreal)));
	d->removeGadget(this, CDiagram::HandlerLayer);
	m_d = NULL;
}

void CDiagramItemGeometryNotifier::reset(QList<CDiagramItem*> &items, const QRectF &rect)
{
	bool				r = false;
	CDiagramItem		*item = NULL;
	CDiagramNode		*shape = NULL;
	QString				s("W:%1 H:%2");

	m_triggered = false;
	if (items.length() == 0)
	{
		setVisible(false);
	}
	else
	{
		foreach (item, items)
		{
			shape = dynamic_cast<CDiagramNode*>(item);
			if (shape != NULL)
			{
				r = true;
				break;
			}
		}

		if (r)
		{
			setVisible(true);
			m_triggered = true;
			m_text->setPlainText( s.arg( qRound(rect.width()) ).arg( qRound(rect.height()) ) );
			m_text->setPos( boundingRect().topLeft() );
			setPos( rect.center() + QPointF(0, rect.height() / 2.0 + SPACING) );
		}
		else
		{
			setVisible(false);
		}
	}
	setVisible(false);
}

void CDiagramItemGeometryNotifier::onBoundedItemsDragging()
{
	QRectF		rect;
	QString		s("X:%1 Y:%2");

	if (!m_triggered) return;
	rect = m_d->getItemsBoundingRect(true);

	setVisible(true);
	m_text->setPlainText( s.arg( qRound(rect.center().x()) ).arg( qRound(rect.center().y()) ) );
	m_text->setPos( boundingRect().topLeft() );
}

void CDiagramItemGeometryNotifier::onBoundedItemsDraggingStop(const QRectF& rect)
{
	if (!m_triggered) return;
	
	setVisible(false);
	m_text->setPos( boundingRect().topLeft() );
	setPos( rect.center() + QPointF(0, rect.height() / 2.0 + SPACING) );
}

void CDiagramItemGeometryNotifier::onBoundedItemsMoving(qreal dx, qreal dy)
{
	if (!m_triggered) return;
	moveBy(dx, dy);
}

void CDiagramItemGeometryNotifier::onBoundedItemsResizingStop(const QRectF &rect)
{
	setVisible(false);
}

void CDiagramItemGeometryNotifier::onBoundedItemsRotatingStop(qreal value)
{
	setVisible(false);
}

void CDiagramItemGeometryNotifier::onBoundedItemsGeometryChanged(const QRectF &rect)
{
	QString		s("W:%1 H:%2");
	
	if (!m_triggered) return;
	setVisible(true);
	m_text->setPlainText( s.arg( qRound(rect.width()) ).arg( qRound(rect.height()) ) );
	m_text->setPos( boundingRect().topLeft() );
	setPos( rect.center() + QPointF(0, rect.height() / 2.0 + SPACING) );
}

void CDiagramItemGeometryNotifier::onBoundedItemsRotationChanged(const qreal value)
{
	QRectF			rect;
	QChar			degree = 0xB0;
	QString			s("%1");
	CDiagramNode	*node = NULL;

	if (!m_triggered) return;
	rect = m_d->getItemsBoundingRect(true);

	node = dynamic_cast<CDiagramNode*>( m_d->selectedTopLevelItem() );
	if (node && m_d->selectedTopLevelItems().length() == 1)
	{
		setVisible(true);
		m_text->setPlainText( s.arg( qRound(node->rotation()) % 360 ).append(degree) );
		m_text->setPos( boundingRect().topLeft() );
		setPos( rect.center() );
	}
	else
	{
		setVisible(false);
	}
}
