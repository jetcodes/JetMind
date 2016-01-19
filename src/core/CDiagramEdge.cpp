/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <limits>
#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramText>
#include <core/CDiagramNode>
#include <core/CDiagramPort>
#include <core/CDiagramEdge>
#include <core/CDiagramArrow>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramArrowCategory>

class CDiagramEdgeLabelInfo {
public:
	qreal			labelPosition;
	int				labelAnchor;
	CDiagramText*	label;
};

static CDiagramNode* getNodeAtPoint(CDiagram *d, const QPointF &p)
{
	CDiagramNode			*node = NULL;
	CDiagramItem			*item = NULL;
	QList<QGraphicsItem*>	items;

	if (d)
	{
		items = d->items(p, Qt::IntersectsItemShape, Qt::AscendingOrder);
		for (int i = 0; i < items.length(); ++i)
		{
			item = dynamic_cast<CDiagramItem*>(items.at(i));
			if (item)
			{
				if (item->interType() == Global::Edge)
					continue;
				else if (item->interType() == Global::Node)
					node = dynamic_cast<CDiagramNode*>( item );
				else
					break;
			}
		}
	}
	return node;
}

static CDiagramPort* getPortAtPoint(CDiagram *d, const QPointF &p)
{
	CDiagramPort	*port = NULL;

	if (d) 
		port = dynamic_cast<CDiagramPort*>( d->itemAt(p) );
	return port;
}

static QDomElement createPortElement(QDomDocument &doc, CDiagramPort *port, const QString &name)
{
	int				exited = 0;
	QDomElement		e;
	CDiagramNode	*p = NULL;

	e = doc.createElement(name);
	if (port)
	{
		exited = 1;
		p = port->getParentNode();	
		e.setAttribute( QString::fromUtf8("node"), QString::number(p->id()) );
		e.setAttribute( QString::fromUtf8("port"), QString::number(port->index()));
	}
	e.appendChild( doc.createTextNode( QString::number(exited) ) );
	return e;
}

static QDomElement createArrowElement(QDomDocument &doc, CDiagramArrow *arrow, const QString &name)
{
	int				exited = 0;
	QDomElement		e;

	e = doc.createElement(name);
	if (arrow)
	{
		exited = 1;
		e.setAttribute( QString::fromUtf8("category"), arrow->getCategory() );
		e.setAttribute( QString::fromUtf8("type"), arrow->getName() );
	}
	e.appendChild( doc.createTextNode( QString::number(exited) ) );
	return e;
}

static CDiagramPort* readPort(CDiagram *d, const QDomElement &e)
{
	int				itemId;
	int				portId;
	CDiagramNode	*s = NULL;
	CDiagramPort	*c = NULL;

	if (!e.isNull() && e.text().toInt() == 1)
	{
		itemId = e.attribute( QString::fromUtf8("node") ).toInt();
		portId = e.attribute( QString::fromUtf8("port") ).toInt();
		s = dynamic_cast<CDiagramNode*>( d->getItemById(itemId) );
		if (s && s->interType() == Global::Node)
		{
			c = s->getPortByIndex(portId);
		}
	}
	return c;
}

static CDiagramArrow* readArrow(const QDomElement &e)
{
	QString				  name;
	QString					category;
	CDiagramArrow			*style = NULL;
	CDiagramArrowDatabase	*db = NULL;
	CDiagramArrowCategory	*ctg = NULL;


	if (!e.isNull() && e.text().toInt() == 1)
	{
		name = e.attribute( QString::fromUtf8("type") );
		category = e.attribute( QString::fromUtf8("category") );
		db = CDiagramArrowDatabase::instance();
		if (db)
		{
			ctg = db->getCategory(category);
			if (ctg)
				style = ctg->getArrow(name);
		}
	}
	return style;
}

static void readAnchor(const QDomElement &e, CDiagramEdgeControlPoint *anchor)
{
	if (!e.isNull() && anchor != NULL)
	{
		anchor->setUsed( Utils::stringToBool( e.text() ) );
		anchor->setX(e.attribute( QString::fromUtf8("x")).toFloat(), Utils::stringToBool(e.attribute( QString::fromUtf8("anchorX") )) );
		anchor->setY(e.attribute( QString::fromUtf8("y")).toFloat(), Utils::stringToBool(e.attribute( QString::fromUtf8("anchorY") )) );
	}
}

/////////////////////////////////////////////////////////////////////////
// Begin of Class's implementation Zone
/////////////////////////////////////////////////////////////////////////
CDiagramEdgeControlPoint::CDiagramEdgeControlPoint()
	: m_anchorX(false),
	  m_anchorY(false),
	  m_used(false)
{
}

void CDiagramEdgeControlPoint::setX(qreal x, bool anchor)
{
	m_pos.setX(x);
	m_anchorX = anchor;
}

void CDiagramEdgeControlPoint::setY(qreal y, bool anchor)
{
	m_pos.setY(y);
	m_anchorY = anchor;
}

void CDiagramEdgeControlPoint::setDirection(const QVector2D &dir)
{
	m_dir = dir;
	m_dir.normalize();
}

void CDiagramEdgeControlPoint::setPos(const QPointF &pos)
{
	m_pos = pos;
}

void CDiagramEdgeControlPoint::clearAnchors()
{
	m_anchorX = false;
	m_anchorY = false;
}

CDiagramEdge::CDiagramEdge(CDiagram *diagram)
	: CDiagramItem(diagram),
	  m_start(NULL),
	  m_end(NULL),
	  m_startArrow(NULL),
	  m_endArrow(NULL),
	  m_edgeStyle(NULL),
	  m_activePoints(0),
	  m_hoverRect(NULL),
	  m_lazyTrack(false),
	  m_autoTrack(true),
	  m_startPercent(0.5),
	  m_endPercent(0.5)
{
	CDiagramEdgeControlPoint		*p = NULL;

	m_category = QString::fromUtf8("Core");
	m_name = QString::fromUtf8("Edge");

	m_resizable = false;
	m_rotatable = false;
	m_keepSizeRatio = false;
	setLockPosition(false);

	m_label->setBackground( QBrush(Qt::white) );
	m_label->setAutoWidth(true);

	m_d1 = Global::East;
	m_d2 = Global::West;

	m_edgeStyle = CDiagramEdgeStyleDatabase::instance()->getDefaultRouter();
	m_endArrow = CDiagramArrowDatabase::instance()->getDefaultArrow();
	
	updateEdgeDirection();

	m_pen.setCapStyle(Qt::RoundCap);
	m_pen.setJoinStyle(Qt::RoundJoin);
	m_pen.setWidth(1);
	m_pen.setColor( QColor("#000000") );
	m_brush.setColor(Qt::black);

	for (int i=0; i<6; ++i)
	{
		p = new CDiagramEdgeControlPoint();
		if (p)
		{
			m_ctrlPoints.append( p );
		}
	}

	defaultInit();
}

CDiagramEdge::~CDiagramEdge()
{
	SAFE_DELETE(m_hoverRect);

	foreach(CDiagramEdgeControlPoint *p, m_ctrlPoints)
	{
		SAFE_DELETE(p);
	}
}

QRectF CDiagramEdge::boundingRect() const
{
	qreal			x1, y1, x2, y2, d = 10;
	QRectF			rect;
	QPointF			point;
	CDiagramText	*label = NULL;

	if (m_activePoints > 0) 
	{
		point = m_ctrlPoints.at(0)->pos();
		x1 = x2 = point.x();
		y1 = y2 = point.y();
	
		for (int i = 1; i < m_activePoints; ++i)
		{
			point = m_ctrlPoints.at(i)->pos();
			x1 = qMin(point.x(), x1);
			y1 = qMin(point.y(), y1);
			x2 = qMax(point.x(), x2);
			y2 = qMax(point.y(), y2);
		}
		rect = QRectF(x1, y1, x2-x1, y2-y1).adjusted(-20, -20, 20, 20);
	}

	for (int i = 0; i < m_labels.length(); ++i)
	{
		label = m_labels[i].label;
		if (!label->toPlainText().trimmed().isEmpty())
		{
			rect = rect.united( label->mapRectToParent(label->boundingRect()) );
		}
	}
	return rect;
}

QPainterPath CDiagramEdge::shape() const
{
	CDiagramText			*label = NULL;
	QPainterPath			p;
	QPainterPathStroker		ps;
	
	if (m_path == QPainterPath())
		return m_path;
	
	ps.setCapStyle(m_pen.capStyle());
	ps.setWidth( m_stroke.widthF() );
	ps.setJoinStyle( m_stroke.joinStyle() );
	ps.setMiterLimit( m_stroke.miterLimit() );

	p = ps.createStroke(m_path);
	for (int i = 0; i < m_labels.length(); ++i)
	{
		label = m_labels[i].label;
		if (!label->toPlainText().trimmed().isEmpty())
		{
			p.addRect( label->mapRectToParent(label->boundingRect()) );
		}
	}
	return p;
}

void CDiagramEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QRectF				r;
	QPainterPath		path;
	QList<QPointF>		points;
	QPainterPathStroker	ps;
	
	if (m_p1.isNull() || m_p2.isNull() || m_activePoints < 2) return;
	
	for (int i = 0; i < m_activePoints; ++i)
	{
		points.push_back(m_ctrlPoints[i]->pos());
	}
	
	painter->save();

	m_pen.setCapStyle(Qt::FlatCap);
	m_pen.setJoinStyle(Qt::MiterJoin);
	painter->setPen(m_pen);
	painter->setBrush(m_brush);
	m_edgeStyle->draw(painter, points, m_startArrow, m_endArrow, path);
    m_path = path;

	if (option->state & QStyle::State_Selected) 
	{
		m_stroke.setCapStyle(Qt::FlatCap);
		m_stroke.setJoinStyle(Qt::MiterJoin);
		m_stroke.setWidthF( m_pen.widthF() + 8);
		painter->strokePath(m_path, m_stroke);
	}

	painter->restore();

	layoutContent(path);
}

QVariant CDiagramEdge::itemChange(GraphicsItemChange change, const QVariant &value)
{
	QPointF		p;

	switch (change)
	{
	case ItemSelectedChange:
		break;

	case ItemPositionChange:
		break;

	default:
		break;
	};
	return QGraphicsItem::itemChange(change, value);
}

void CDiagramEdge::init(const QVariant &value)
{
	QPen		  p;
	QDomElement	  e;
	CDiagramArrow *a = NULL;

	// start arrow
	a = readArrow(e);
	if (a)
		setStartArrow(a);

	// end arrow
	a = readArrow(e);
	if (a)
		setEndArrow(a);

	// pen style
	//p = readPenStyle();
	setPen(p);
}

void CDiagramEdge::defaultInit()
{
	CDiagramEdgeLabelInfo			info;
	
	m_labels.clear();

	// Label info
	info.labelPosition = 0.5;
	info.labelAnchor = CDiagramEdge::Inline;
	info.label = m_label;
	m_labels.push_back(info);
}

void CDiagramEdge::retrack()
{
	bool			reconnect = false;
	int				i = 0, d1 = m_d1, d2 = m_d2;
	qreal			pw;
	QPointF			p, p1 = m_p1, p2 = m_p2;
	QList<QPointF>	ps;
	CDiagramNode	*n1 = NULL, *n2 = NULL;
	CDiagramPort	*t1 = NULL, *t2 = NULL;

	prepareGeometryChange();
	
	if (m_autoTrack && m_start && m_end)
	{
		n1 = m_start->getParentNode();
		n2 = m_end->getParentNode();
		if (n1 && n2 && n1 != n2)
		{
			t1 = n1->getClosestPort(n2);
			t2 = n2->getClosestPort(n1);
			if (t1 != m_start && t1)
			{
				m_start->removeEdge(this);
				setStartPort(t1);
				t1->addEdge(this);
				reconnect = true;
			}
			if (t2 != m_end && t2)
			{
				m_end->removeEdge(this);
				setEndPort(t2);
				t2->addEdge(this);
				reconnect = true;
			}
			if (reconnect)
			{
				for (int i = 0; i < m_ctrlPoints.length(); ++i) m_ctrlPoints[i]->clearAnchors();
			}
		}
	}

	if (!m_lazyTrack)
	{
		d1 = (m_start != NULL) ? m_start->getConnectPosition(this,p1) : m_d1;
		d2 = (m_end   != NULL) ? m_end->getConnectPosition(this,p2)   : m_d2;
	}

	d1 = (d1 < 0) ? m_d1 : d1;
	d2 = (d2 < 0) ? m_d2 : d2;
	// p1, p2 is in scene coordination
	// after the routing, it will be transferred to
	// the local coordintion for control points.

	/*
	// adjust the p1 and p2 a little bit according to the pen's width
	pw = m_pen.widthF();
	if (m_startArrow)
	{
		switch(d1)
		{
		case Global::West:
			p1 = p1 + QPointF(-pw, 0);
			break;

		case Global::East:
			p1 = p1 + QPointF(pw, 0);
			break;

		case Global::South:
			p1 = p1 + QPointF(0, pw);
			break;

		case Global::North:
			p1 = p1 + QPointF(0, -pw);
			break;

		default:
			break;
		};
	}

	if (m_endArrow)
	{
		switch(d2)
		{
		case Global::West:
			p2 = p2 + QPointF(-pw, 0);
			break;

		case Global::East:
			p2 = p2 + QPointF(pw, 0);
			break;

		case Global::South:
			p2 = p2 + QPointF(0, pw);
			break;

		case Global::North:
			p2 = p2 + QPointF(0, -pw);
			break;

		default:
			break;
		};
	}
	*/

	m_edgeStyle->route(p1, d1, p2, d2, ps);

	layoutControlPoints(ps);

	m_lazyTrack = false;
	m_d1 = d1;
	m_d2 = d2;
	m_p1 = p1;
	m_p2 = p2;
}

void CDiagramEdge::layoutControlPoints(const QList<QPointF> &points)
{
	int			i = 0;
	QPointF		p;

	// If the count does not match, that means
	// the path has to be recalculated.
	if ( m_activePoints != points.length())
	{
		for (i=0; i<points.length(); ++i)
		{
			m_ctrlPoints[i]->clearAnchors();
			m_ctrlPoints[i]->setUsed(true);
			m_ctrlPoints[i]->setPos( mapFromScene(points[i]) );
		}
		for (; i<m_ctrlPoints.length(); ++i)
		{
			m_ctrlPoints[i]->clearAnchors();
			m_ctrlPoints[i]->setUsed(false);
		}
		m_activePoints = points.length();
	}
	else
	{
		for (i = 0; i<m_activePoints; ++i)
		{
			p = mapFromScene( points[i] );
			if (!m_ctrlPoints[i]->anchorX()) m_ctrlPoints[i]->setX(p.x());
			if (!m_ctrlPoints[i]->anchorY()) m_ctrlPoints[i]->setY(p.y());
		}
	}

	// The direction of the anchor is updated here
	for (i=0; i<m_activePoints; ++i)
	{
		if (i < m_activePoints - 1)
			m_ctrlPoints[i]->setDirection( QVector2D(m_ctrlPoints[i+1]->pos() - m_ctrlPoints[i]->pos()) );
		else
			m_ctrlPoints[i]->setDirection( QVector2D(m_ctrlPoints[i-1]->pos() - m_ctrlPoints[i]->pos()) );
	}
}

void CDiagramEdge::layoutContent(const QPainterPath &path)
{
	int						a = CDiagramEdge::Inline;
	qreal					p = 0.5;
	QRectF					r;
	CDiagramEdgeLabelInfo	info;

	for (int i = 0; i < m_labels.length(); ++i)
	{
		info = m_labels.at(i);
		r = info.label->boundingRect();
		switch(a)
		{
		default:
		case Inline:
			info.label->setPos( path.pointAtPercent(info.labelPosition) + QPointF(-r.width() / 2.0, -r.height() / 2.0) );
			break;

		case Below:
			// TODO:
			break;

		case Above:
			// TODO:
			break;
		};
	}
}

void CDiagramEdge::refresh()
{
	update();
}

void CDiagramEdge::setStartPort(CDiagramPort* port)
{
	if (m_start == port) return;

	// Should we check whether the original port is still
	// connect with this edge?
	m_start = port;
	if (port)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		m_d1 = port->getConnectPosition(this,m_p1);
	}
	else
	{
		if (endPort() == NULL)
			setFlag(QGraphicsItem::ItemIsMovable, true);
	}
}

void CDiagramEdge::setEndPort(CDiagramPort* port)
{
	if (m_end == port) return;
	
	// Should we check whether the original port is still
	// connect with this edge?
	m_end = port;
	if (port)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		m_d2 = port->getConnectPosition(this,m_p2);
	}
	else
	{
		if (startPort() == NULL)
			setFlag(QGraphicsItem::ItemIsMovable, true);
	}
}

void CDiagramEdge::setStartArrow(const CDiagramArrow *value)
{
	m_startArrow = const_cast<CDiagramArrow*>(value);
	CDiagramArrowDatabase::instance()->setDefaultArrow(const_cast<CDiagramArrow*>(value));
	updateEdgeDirection();
}

void CDiagramEdge::setEndArrow(const CDiagramArrow *value)
{
	m_endArrow = const_cast<CDiagramArrow*>(value);
	CDiagramArrowDatabase::instance()->setDefaultArrow(const_cast<CDiagramArrow*>(value));
	updateEdgeDirection();
}

void CDiagramEdge::setEdgeStyle(const CDiagramEdgeStyle *value)
{
	CDiagramEdgeStyleDatabase::instance()->setDefaultRouter(const_cast<CDiagramEdgeStyle*>(value));
	m_edgeStyle = const_cast<CDiagramEdgeStyle*>(value);
}

void CDiagramEdge::setStartPoint(const QPointF &p)
{
	m_p1 = p;
	m_lazyTrack = true;
}

void CDiagramEdge::setEndPoint(const QPointF &p)
{
	m_p2 = p;
	m_lazyTrack = true;
}

void CDiagramEdge::setStartDirection(int d)
{
	m_d1 = d;
}

void CDiagramEdge::setEndDirection(int d)
{
	m_d2 = d;
}

void CDiagramEdge::addLabel(const QString &label, qreal pos)
{
	CDiagramText			*text = NULL;
	CDiagramEdgeLabelInfo	info;

	text = new CDiagramText(this);
	text->setHtml(label);
	text->setAutoWidth(true);
	//text->setBrush( QBrush(Qt::white) );
	//text->setEditMode(CDiagramText::LineEditMode);

	info.labelAnchor = Inline;
	info.labelPosition = pos;
	info.label = text;

	m_labels.push_back(info);
}

void CDiagramEdge::removeLabel(int index)
{
	if (index >= 0 && index < m_labels.length())
	{
		SAFE_DELETE(m_labels[index].label);
		m_labels.removeAt(index);
	}
}

QList<CDiagramText*> CDiagramEdge::labels()
{
	QList<CDiagramText*>	list;

	for (int i = 0; i < m_labels.length(); ++i)
	{
		list.push_back(m_labels[i].label);
	}
	return list;
}

void CDiagramEdge::setLabelPosition(int index, qreal p)
{
	if (index >= 0 && index < m_labels.length())
	{
		if (p >= 0.0 && p <= 1.0)
		{
			m_labels[index].labelPosition = p;
			layoutContent(m_path);
		}
	}
}

qreal CDiagramEdge::labelPosition(int index)
{
	qreal			p = 0.5;
	
	if (index >= 0 && index < m_labels.length())
	{
		p = m_labels[index].labelPosition;
	}
	return p;
}

void CDiagramEdge::setLabelText(int index, const QString &value)
{
	if (index >= 0 && index < m_labels.length())
	{
		m_labels[index].label->setHtml(value);
	}
}

QString CDiagramEdge::labelText(int index)
{
	QString			r;
	
	if (index >= 0 && index < m_labels.length())
	{
		r = m_labels[index].label->toPlainText();
	}
	return r;
}

void CDiagramEdge::setLabelAnchor(int index, CDiagramEdge::LabelAnchor value)
{
	// NOT IMPLEMENT
}

CDiagramEdge::LabelAnchor CDiagramEdge::labelAnchor(int index)
{
	// NOT IMPLEMENT
	return Inline;
}

void CDiagramEdge::setAutoTrack(bool value)
{
	m_autoTrack = value;
}

bool CDiagramEdge::autoTrack()
{
	return m_autoTrack;
}

void CDiagramEdge::setStartPercent(qreal value)
{
	m_startPercent = value;
}

qreal CDiagramEdge::startPercent()
{
	return m_startPercent;
}

void CDiagramEdge::setEndPercent(qreal value)
{
	m_endPercent = value;
}

qreal CDiagramEdge::endPercent()
{
	return m_endPercent;
}

void CDiagramEdge::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	bool					accepted = false;
	qreal					dist = -1, temp = 0;
	QRectF					rect;
	QGraphicsItem			*item = NULL, *closedItem = NULL;
	QGraphicsLayoutItem		*layout = NULL;
	QList<QGraphicsItem*>	items;

	items = childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		layout = dynamic_cast<QGraphicsLayoutItem*>(item);
		if (layout)
		{
			rect = item->mapRectToScene( item->boundingRect() );
			if (rect.contains(event->scenePos()))
			{
				scene()->sendEvent(item, event);
				event->accept();
				accepted = true;
				break;
			}
			else
			{
				temp = QLineF(event->scenePos(), rect.center()).length();
				if (dist > temp || dist < 0)
				{
					dist = temp;
					closedItem = item;
				}
			}
		}
	}
	
	if (!accepted && closedItem)
	{
		scene()->sendEvent(closedItem, event);
		event->accept();
	}
}

QPainterPath CDiagramEdge::path()
{
	return m_path;
}

void CDiagramEdge::startGrowing(const QPointF &p)
{
	QRectF				rect;
	QGraphicsRectItem	*item = NULL;
	CDiagramNode		*node = NULL;

	node = getNodeAtPoint(m_p, p);
	if (node)
	{
		rect = node->boundingRect();
		m_hoverRect = new QGraphicsRectItem();
		m_hoverRect->setPen(QPen(Qt::red, 2));
		m_hoverRect->setBrush(Qt::NoBrush);
		m_p->addItem(m_hoverRect);
		m_hoverRect->setRect( node->mapRectToScene(rect) );
	}

	setStartPoint( mapToParent( mapFromScene(p) ) );
}

bool CDiagramEdge::onGrowing(const QPointF &p)
{
	bool				r = false;
	QPointF				t;
	QRectF				rect;
	QGraphicsRectItem	*item = NULL;
	CDiagramNode		*node = NULL;

	node = getNodeAtPoint(m_p, p);
	if (node && m_hoverRect)
	{
		rect = node->boundingRect();
		m_hoverRect->setRect( node->mapRectToScene( rect ) );
	}

	t = mapToParent( mapFromScene(p) );
	if (qAbs(t.x() - m_p1.x()) >= 20 || qAbs(t.y() - m_p1.y()) >= 20.0)
	{
		r = true;
		setEndPoint(t);
	}
	retrack();
	return r;
}

bool CDiagramEdge::stopGrowing(const QPointF &p)
{
	bool				r = false;
	QPointF				t;
	CDiagram			*d = NULL;
	CDiagramPort		*port1 = NULL, *port2 = NULL;
	CDiagramNode		*node1 = NULL, *node2 = NULL;

	d = dynamic_cast<CDiagram*>(scene());
	node1 = getNodeAtPoint(m_p, m_p1);
	node2 = getNodeAtPoint(m_p, p);
	
	if (m_hoverRect)
	{
		if (!d)
			d = dynamic_cast<CDiagram*>(m_hoverRect->scene());
		if (d)
			d->removeItem(m_hoverRect);
		SAFE_DELETE(m_hoverRect);
	}

	if (node1 && node2 && node1 != node2)
	{
		port1 = node1->getClosestPort(node2);
		port2 = node2->getClosestPort(node1);
		if (port1 && port2)
		{
			setStartPort(port1);
			port1->addEdge(this);

			setEndPort(port2);
			port2->addEdge(this);

			r = true;
		}
	}
	else
	{
		t = mapToParent( mapFromScene(p) );
		if (qAbs(t.x() - m_p1.x()) >= 20 || qAbs(t.y() - m_p1.y()) >= 20.0)
		{
			r = true;
			setEndPoint(t);
		}
	}
	retrack();
	return r;
}

int CDiagramEdge::edgeDirection(CDiagramPort *port)
{
	int		r = 0;

	r = m_direction;
	if (port)
	{
		if (port == m_end)
		{
			if (m_direction == Forward)
				r = Backward;
			else if (m_direction == Backward)
				r = Forward;
			else
				r = BothDirections;
		}
	}
	return r;
}

void CDiagramEdge::setEdgeDirection(int value)
{
	m_direction = value;
}

void CDiagramEdge::updateEdgeDirection()
{
	int		s;
	int		startLevel = 0, endLevel = 0;

	if (m_startArrow)
	{
		s = m_startArrow->direction();
		if (s == CDiagramArrow::Inward) startLevel -= 1;
		if (s == CDiagramArrow::Outward)startLevel += 1;
	}

	if (m_endArrow)
	{
		s = m_endArrow->direction();
		if (s == CDiagramArrow::Inward) endLevel -= 1;
		if (s == CDiagramArrow::Outward)endLevel += 1;
	}

	s = startLevel - endLevel;
	if (s > 0)
		m_direction = Forward;
	else if (s < 0)
		m_direction = Backward;
	else
		m_direction = BothDirections;
}

void CDiagramEdge::extFromXml(const QDomElement &n)
{
	int							index = 1;
	bool						prim = false;
	qreal						m;
	QDomElement					e, p, a;
	QDomNodeList				ns;
	CDiagram					*d = NULL;
	CDiagramEdgeStyle			*r = NULL;
	CDiagramEdgeStyleCategory	*ctg = NULL;
	CDiagramEdgeStyleDatabase	*db = NULL;
	CDiagramPort				*port = NULL;
	CDiagramText				*text = NULL;

	d = dynamic_cast<CDiagram*>(scene());
	if (!d) return;

	p = n.firstChildElement( QString::fromUtf8("ports") );
	a = n.firstChildElement( QString::fromUtf8("arrows") );
	setStartPort( readPort(d, p.firstChildElement( QString::fromUtf8("start") ) ) );
	setEndPort( readPort(d, p.firstChildElement( QString::fromUtf8("end") ) ) );
	setStartArrow( readArrow( a.firstChildElement( QString::fromUtf8("startArrow") ) ) );
	setEndArrow( readArrow( a.firstChildElement( QString::fromUtf8("endArrow") ) ) );

	if (m_start) 
	{
		m_start->addEdge(this);
		m_start->setConnectPosition(this, startPercent());
	}
	if (m_end) 
	{
		m_end->addEdge(this);
		m_end->setConnectPosition(this, endPercent());
	}

	// extract labels
	a = n.firstChildElement("labels");
	e = a.firstChildElement("label");
	while (!e.isNull())
	{
		CDiagramEdgeLabelInfo	info;

		info.labelPosition = e.attribute("position").toFloat();
		info.labelAnchor = e.attribute("anchor").toInt();
		prim = Utils::stringToBool( e.attribute("primary") );
		if (!prim)
		{
			text = new CDiagramText(this);
			info.label = text;
			m_labels.push_back(info);
			setLabelPosition(index, info.labelPosition);
			++index;
		}
		else
		{
			text = m_label;
			info.label = text;
			setLabelPosition(0, info.labelPosition);
		}
		
		text->setHtml( QString::fromUtf8( QByteArray::fromBase64( e.text().toAscii() ) ) );
		e = e.nextSiblingElement();
	}


	//extract the edge style from the xml
	e = n.firstChildElement( QString::fromUtf8("edgeStyle"));
	db = CDiagramEdgeStyleDatabase::instance();
	if (db)
	{
		ctg = db->getCategory(e.attribute( QString::fromUtf8("category")) );
		if (ctg)
			r = ctg->getStyle( e.attribute( QString::fromUtf8("name") ) );
		if (!r)
			r = db->getDefaultRouter();
		setEdgeStyle(r);
	}

	// extract the anchors
	m_activePoints = 0;
	e = n.firstChildElement( QString::fromUtf8("anchors"));
	ns = e.elementsByTagName( QString::fromUtf8("anchor") );
	for (int i = 0; i < ns.count() && i < m_ctrlPoints.length(); ++i)
	{
		e = ns.at(i).toElement();
		readAnchor(e, m_ctrlPoints[i]);
		if (m_ctrlPoints.at(i)->used())
			++m_activePoints;
	}
	
	updateEdgeDirection();
	refresh();
}

void CDiagramEdge::extToXml(QDomElement &n)
{
	QDomElement					e, t;
	QDomDocument				doc;
	CDiagramEdgeControlPoint	*p = NULL;

	doc = n.ownerDocument();
	e = doc.createElement( QString::fromUtf8("ports") );
	e.appendChild( createPortElement(doc, m_start, QString::fromUtf8("start") ) );
	e.appendChild( createPortElement(doc, m_end, QString::fromUtf8("end") ) );
	n.appendChild(e);

	e = doc.createElement( QString::fromUtf8("arrows") );
	e.appendChild( createArrowElement(doc, m_startArrow, QString::fromUtf8("startArrow") ) );
	e.appendChild( createArrowElement(doc, m_endArrow, QString::fromUtf8("endArrow") ) );
	n.appendChild(e);

	e = doc.createElement("labels");
	for (int i = 0; i < m_labels.length(); ++i)
	{
		CDiagramEdgeLabelInfo	info;
		info = m_labels.at(i);

		t = doc.createElement("label");
		t.setAttribute("position", QString::number(info.labelPosition));
		t.setAttribute("anchor", QString::number(info.labelAnchor));
		t.setAttribute("primary", Utils::boolToString(info.label == m_label));
		t.appendChild(doc.createTextNode( QString( info.label->toHtml().toUtf8().toBase64()) ));
		e.appendChild(t);
	}
	n.appendChild(e);

	// save the edge style
	e = doc.createElement( QString::fromUtf8("edgeStyle") );
	e.setAttribute( QString::fromUtf8("category"), m_edgeStyle->getCategory() );
	e.setAttribute( QString::fromUtf8("name"), m_edgeStyle->getName());
	n.appendChild(e);

	// save the anchors
	e = doc.createElement( QString::fromUtf8("anchors"));
	for (int i = 0;i < m_ctrlPoints.length(); ++i)
	{
		p = m_ctrlPoints.at(i);
		t = Utils::genXmlElement(doc, QString::fromUtf8("anchor"), Utils::boolToString(p->used()));
		t.setAttribute( QString::fromUtf8("x"), QString::number(p->x()) );
		t.setAttribute( QString::fromUtf8("y"), QString::number(p->y()) );
		t.setAttribute( QString::fromUtf8("anchorX"),Utils::boolToString(p->anchorX()) );
		t.setAttribute( QString::fromUtf8("anchorY"),Utils::boolToString(p->anchorY()) );
		e.appendChild(t);
	}
	n.appendChild(e);
}
