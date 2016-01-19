/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramGroup>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramLine>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>

CDiagramGroup::CDiagramGroup(CDiagram *parent)
	: CDiagramItem(parent)
{
	 setHandlesChildEvents(true);
	 m_pen.setColor(Qt::gray);
	
	 setEffectEnabled(false);
	
	 setCategory(QString("Core"));
	 setName(QString("Group"));

	 if (parent)
		 connect(parent, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	 connect(this, SIGNAL(propertyChanged(const QString&, const QVariant&)), this, SLOT(onPropertyChanged(const QString&, const QVariant&)));
	 connect(this, SIGNAL(geometryChanged()), this, SLOT(onGeometryChanged()));
}

CDiagramGroup::~CDiagramGroup()
{
	QList<CDiagramItem*>	allItems;

	allItems = items();
	foreach (CDiagramItem *item, allItems)
	{
		item->deleteLater();
	}
}

void CDiagramGroup::initGroup(const QList<CDiagramItem*> &items)
{
	QRectF			itemRect, rect;
	CDiagramItem	*item = NULL;
	QGraphicsItem	*p = NULL;
	bool			resizable = true;
	bool			rotatable = true;
	bool			keepSizeRatio = false;

	foreach (item, items)
	{
		if (item->interType() == Global::Edge) continue;
		itemRect = item->boundingRect();
		itemRect.moveCenter( item->pos() );
		rect |= itemRect;
		if (resizable  && !item->resizable()) resizable = false;
		if (rotatable  && !item->rotatable()) rotatable = false;
		if (!keepSizeRatio && item->keepSizeRatio()) keepSizeRatio = true;
	}
	
	setResizable(resizable);
	setRotatable(rotatable);
	setKeepSizeRatio(keepSizeRatio);

	setGeometry( QRectF(rect.center(), rect.size()) );
	
	foreach (item, items)
	{
		p = item->parentItem();
		item->setParentItem(this);
		if (p)
			item->setPos( mapFromItem(p, item->pos()) );
		else
			item->setPos( mapFromScene(item->pos()) );
	}
}

void CDiagramGroup::destroyGroup()
{
	QPointF			point;
	CDiagramItem	*item = NULL;
	QGraphicsItem	*p = NULL;

	p = parentItem();
	foreach (item, items())
	{
		point = item->pos();
		item->setParentItem( p );
		if (p)
			item->setPos(mapToParent(point));
		else
			item->setPos(mapToScene(point));
	}
}

QList<CDiagramItem*> CDiagramGroup::items()
{
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	items;

	foreach (QGraphicsItem *c, childItems())
	{
		item = dynamic_cast<CDiagramItem*>(c);
		if (item && item->parentItem() == this)
			items.append(item);
	}
	return items;
}

QRectF CDiagramGroup::boundingRect() const
{
	return QRectF(-width() / 2.0, -height() / 2.0, width(), height());
}

QPainterPath CDiagramGroup::shape() const
{
	CDiagramItem	*item = NULL;
	QPainterPath	path;

	path.setFillRule(Qt::WindingFill);
	foreach (QGraphicsItem *c, childItems())
	{
		item = dynamic_cast<CDiagramItem*>(c);
		if (item && item->parentItem() == this)
		{
			path.addPath( mapFromItem(item, item->shape()) );
		}
	}
	return path;
}

void CDiagramGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);
    if (option && (option->state & QStyle::State_Selected) )
	{
		painter->save();
		painter->setPen( m_pen );
        painter->setBrush(Qt::NoBrush);
		// painter->drawRect( boundingRect() );
		painter->restore();
    }
}

bool CDiagramGroup::isObscuredBy(const QGraphicsItem *item) const
{
    return CDiagramItem::isObscuredBy( item );
}

QPainterPath CDiagramGroup::opaqueArea() const
{
    return CDiagramItem::opaqueArea();
}

void CDiagramGroup::layoutContent()
{
	QPointF		p;
	QRectF		rect;
	qreal		xr, yr;

	rect = shape().boundingRect();
	xr = rect.width() > 0 ? width() / rect.width() : 1.0;
	yr = rect.height() > 0 ? height() / rect.height() : 1.0;
	
	foreach (CDiagramItem *item, items())
	{
		p = item->pos();
		item->setGeometry( QRectF( p.x() * xr, p.y() * yr, item->width() * xr, item->height() * yr) );
	}
}

void CDiagramGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	foreach (CDiagramItem *item, items())
	{
		if (item->isUnderMouse())
		{
			if (scene())
			{
				setHandlesChildEvents(false);
				scene()->sendEvent(item, event);
				event->accept();
				break;
			}
		}
	}
}

void CDiagramGroup::onDiagramModeChanged(CDiagram::DiagramMode mode)
{
	// Need to verify the codes here
	if (mode != CDiagram::EditTextMode)
	{
		setHandlesChildEvents(true);
	}
}

void CDiagramGroup::onPropertyChanged(const QString &name, const QVariant &value)
{
	CDiagramItem			*item = NULL;
	QList<CDiagramItem*>	list;

	list = items();
	for (int i = 0; i < list.length(); ++i)
	{
		item = list.at(i);
		item->setProperty(name.toAscii().constData(), value);
	}
}

void CDiagramGroup::onGeometryChanged()
{
	CDiagramNode		*node = NULL;
	CDiagramItem		*item = NULL;

	layoutContent();
	foreach (item, items())
	{
		node = dynamic_cast<CDiagramNode*>(item);
		if (node)
			node->retrack();
	}
}

void CDiagramGroup::fromXml(const QDomElement &n)
{
	int						id;
	QDomElement				e;
	QString					category, name;
	CDiagramItem			*item = NULL;
	CDiagramItemTypeCategory	*factory = NULL;
	QList<CDiagramItem*>	items;

	if (!m_p) return;

	setCategory(n.attribute( QString("category") ) );
	setName(n.attribute( QString("name") ) );
	setId(n.attribute( QString("id") ).toInt() );
	
	e = n.firstChildElement("geometry");
	if (!e.isNull())
	{
		QRectF		r;
		QByteArray	b = QByteArray::fromBase64( e.text().toAscii() );
		QDataStream s(&b, QIODevice::ReadOnly);
		
		s >> r;
		setGeometry(r);
	}

	e = n.firstChildElement( QString("groupedItems") );
	if (!e.isNull())
	{
		e = e.firstChildElement( QString("groupedItem") );
		while (!e.isNull())
		{
			category = e.attribute( QString("category" ));
			name = e.attribute( QString("name") );
			id = e.attribute( QString("id") ).toInt();
			factory = CDiagramItemTypeDatabase::instance()->getCategory(category);
			if (factory)
			{
				item = factory->createItem(name, Q_ARG(CDiagram*, m_p));
				if (item)
				{
					item->setId(id);
					m_p->insertItem(item, item->pos(), false, true);
					item->fromXml(e);
					item->setParentItem(this);
					items.push_back(item);
				}
			}
			e = e.nextSiblingElement(QString("groupedItem"));
		}
	}
	//initGroup(items);
}

void CDiagramGroup::toXml(QDomElement &n)
{
	QByteArray				b;
	QDataStream				s(&b, QIODevice::WriteOnly);
	QDomDocument			doc;
	QDomElement				childNode, e;
	QGraphicsItem			*item = NULL;
	CDiagramSerializable	*inst = NULL;
	QList<QGraphicsItem*>	items;
	
	doc = n.ownerDocument();
	childNode = doc.createElement( QString("groupedItems") );

	n.setAttribute( QString("type"), interType());
	n.setAttribute( QString("category"), category() );
	n.setAttribute( QString("name"), name() );
	n.setAttribute( QString("id"), QString::number(id()) );
	
	s << geometry();
	e = doc.createElement(QString("geometry"));
	e.appendChild( doc.createTextNode( QString(b.toBase64() ) ) );
	n.appendChild(e);

	items = childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		inst = dynamic_cast<CDiagramSerializable*>(item);
		if (inst && inst != m_label)
		{
			QDomElement		c = doc.createElement(QString("groupedItem"));
			inst->toXml(c);
			childNode.appendChild(c);
		}
	}
	n.appendChild(childNode);
}