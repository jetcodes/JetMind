/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramNode>

#include "CMindMapNode.h"
#include "CMindMapNodeGroup.h"
#include "CMindMapMenuManager.h"

#define GROUP_PADDING	12

CMindMapNodeGroup::CMindMapNodeGroup(CDiagram *d, CMindMapNode *node)
    : CDiagramNode(d),
      m_node(node),
      m_startIndex(-1),
      m_endIndex(-1),
      m_stroke(4),
	  m_relativePos( CMindMapNode::East ),
      m_groupType( CMindMapNode::Boundary )
{
	// setting the zvalue to 0.0 to keep it under all the visual nodes which
	// are all set zvalue to 1.0
	setZValue(0);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setAcceptHoverEvents(true);

    connect(this, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(onPropertyChanged(QString,QVariant)));
}

CMindMapNodeGroup::~CMindMapNodeGroup()
{

}

QRectF CMindMapNodeGroup::boundingRect() const
{
    return QRectF();
}

QPainterPath CMindMapNodeGroup::shape() const
{
    return QPainterPath();
}

void CMindMapNodeGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void CMindMapNodeGroup::relocate()
{

}

void CMindMapNodeGroup::setNode(CMindMapNode *node)
{
    m_node = node;
    relocate();
}

CMindMapNode* CMindMapNodeGroup::node()
{
    return m_node;
}

void CMindMapNodeGroup::setStartIndex(int index)
{
    if (index < 0)
        return;

    m_startIndex = index;
}

int CMindMapNodeGroup::startIndex()
{
    return m_startIndex;
}

void CMindMapNodeGroup::setEndIndex(int index)
{
    if (index < 0)
        return;

    m_endIndex = index;
	m_node->onStructChanged();
}

int CMindMapNodeGroup::endIndex()
{
    return m_endIndex;
}

void CMindMapNodeGroup::setRelativePos(int value)
{
	m_relativePos = value;
}

int CMindMapNodeGroup::relativePos()
{
	return m_relativePos;
}

int CMindMapNodeGroup::groupType() const
{
    return m_groupType;
}

bool CMindMapNodeGroup::isEmpty() const
{
    return (m_endIndex < m_startIndex);
}

void CMindMapNodeGroup::onAddChild(int index, int refIndex)
{
    if (refIndex >= 0)
    {
        if (refIndex >= m_startIndex && refIndex <= m_endIndex)
        {
            ++m_endIndex;
        }
        else
        {
            if (refIndex < m_startIndex)
            {
                ++m_startIndex;
                ++m_endIndex;
            }
            else
            {
                // Nothing
            }
        }
    }
    else
    {
        if (index >= m_startIndex && index <= m_endIndex)
        {
            ++ m_endIndex;
        }
        else if (index < m_startIndex)
        {
            ++m_startIndex;
            ++m_endIndex;
        }
        else
        {
            // Nothing
        }
    }
}

void CMindMapNodeGroup::onPropertyChanged(const QString &name, const QVariant &value)
{
    if (m_label)
    {
        m_label->setFont( font() );
        m_label->setTextColor( textColor() );
        update();
    }

    if (name == "brush")
    {
        int             alpha = 98, threshold = 168;
        qreal           v;
        QColor          c;
        QGradient       *gradient = NULL;
        QGradientStops  stops;

        gradient = const_cast<QGradient*>( m_brush.gradient() );
        if (gradient)
        {
            stops = gradient->stops();
            foreach (QGradientStop stop, stops)
            {
                v = stop.first;
                c = stop.second;
                if (c.alpha() >= threshold )
                    c.setAlpha(alpha);
                gradient->setColorAt(v, c);
            }
        }
        else
        {
            c = m_brush.color();
            if (c.alpha() >= threshold)
                c.setAlpha(alpha);
            m_brush.setColor(c);
        }
    }
}

int CMindMapNodeGroup::onDeleteChild(int index)
{
    int     ref = -1;

    if (index >= m_startIndex && index <= m_endIndex)
    {
        if (index == m_startIndex)
            ref = m_startIndex;
        if (index == m_endIndex)
            ref = m_endIndex - 1;
        --m_endIndex;
    }
    else if (index < m_startIndex)
    {
        --m_startIndex;
        --m_endIndex;
    }
    else
    {
        // Nothing
    }

    return ref;
}

void CMindMapNodeGroup::onReorderChild(int from, int to)
{

    if (from >= m_startIndex && from <= m_endIndex)
    {
        if (to >= m_startIndex && to <= m_endIndex)
        {
            // Nothing
        }
        else if (to < m_startIndex)
        {
            ++m_startIndex;
        }
        else
        {
            --m_endIndex;
        }
    }
    else if (from < m_startIndex)
    {
        if (to >= m_startIndex && to <= m_endIndex)
        {
            --m_startIndex;
            if (to == m_endIndex)
                --m_endIndex;
        }
        else if (to < m_startIndex)
        {
            // Nothing
        }
        else
        {
            --m_startIndex;
            --m_endIndex;
        }
    }
    else
    {
        if (to >= m_startIndex && to <= m_endIndex)
        {
            ++m_endIndex;
            if (to == m_startIndex)
                ++m_startIndex;
        }
        else if (to < m_startIndex)
        {
            ++m_startIndex;
            ++m_endIndex;
        }
        else
        {
            // Nothing
        }
    }
}

void CMindMapNodeGroup::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu       *menu = NULL;

    if (!event->isAccepted())
    {
        menu = contextMenu();
        if (menu)
        {
            menu->exec(event->screenPos());
        }
        event->accept();
    }
}

void CMindMapNodeGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CDiagramNode::mousePressEvent(event);
}

void CMindMapNodeGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    CDiagramNode::mouseReleaseEvent(event);
}

QMenu* CMindMapNodeGroup::contextMenu()
{
    return NULL;
}

CMindMapNodeBoundary::CMindMapNodeBoundary(CDiagram *d, CMindMapNode *node)
    : CMindMapNodeGroup(d, node),
      m_boundaryType( CMindMapNodeBoundary::RounededRect )
{
    m_groupType = CMindMapNode::Boundary;

    setPen( QPen(Qt::gray, 2, Qt::DashLine) );
    setBrush( QBrush(QColor(221, 221, 221, 64)) );

    m_label->setFont( QFont("Arial", 9) );
    m_label->setTextColor( Qt::darkGray );
    m_label->setLineEditMode(true);
}

CMindMapNodeBoundary::~CMindMapNodeBoundary()
{

}

QRectF CMindMapNodeBoundary::boundingRect() const
{
    qreal       w, h;

    w = m_areaRect.width() + 2 * m_stroke;
    h = m_areaRect.height() + 2 * m_stroke;

    return QRectF(-w / 2.0, -h / 2.0, w, h);
}

QPainterPath CMindMapNodeBoundary::shape() const
{
    QPainterPath        path;

    path.addRect( boundingRect() );
    return path;
}

void CMindMapNodeBoundary::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal		radius = 8, padding = 8;
    QPen        p;
    QRectF      r, textRect;

    r = QRectF(-m_areaRect.width() / 2.0, -m_areaRect.height() / 2.0, m_areaRect.width(), m_areaRect.height() );
    textRect = m_label->boundingRect();

    p = pen();
    painter->save();
    painter->setPen( p );
    painter->setBrush( brush() );
    painter->drawRoundedRect(r, radius, radius);

    if (isSelected())
    {
        painter->setPen( QPen(Qt::blue, m_stroke, Qt::DashLine) );
        painter->setBrush( Qt::NoBrush );
        painter->drawRoundedRect( boundingRect(),  radius, radius );
    }

    if (!m_label->toPlainText().isEmpty())
    {
        r = QRectF( r.topLeft() - QPointF(-padding, textRect.height() / 2.0), textRect.size());
        painter->setPen( pen() );
        painter->setBrush( QBrush(Qt::yellow) );
        painter->drawRect(r);
    }

    painter->restore();
}

void CMindMapNodeBoundary::relocate()
{
    QRectF                      areaRect, textRect;
	CMindMapNode				*node = NULL;
    QList<CMindMapNode*>        nodes;

    if (!m_node || m_startIndex < 0 || !m_label)
        return;

    textRect = m_label->boundingRect();
    nodes = m_node->childs(m_startIndex, m_endIndex);
    foreach (node, nodes)
    {
        QRectF  r;
        r = node->areaRect();
        areaRect = areaRect.unite( r );
    }
	
    areaRect = areaRect.adjusted(-GROUP_PADDING * 2,
                                 -GROUP_PADDING - textRect.height() / 2.0,
                                 GROUP_PADDING * 2,
                                 GROUP_PADDING);

    setGeometry( QRectF(areaRect.center(), areaRect.size()) );
	if (node)
		setRelativePos( node->relativePosition() );
    m_areaRect = areaRect;

	layoutContent();
}

void CMindMapNodeBoundary::setBoundaryType(int value)
{
    m_boundaryType = value;
    update();
}

int CMindMapNodeBoundary::boundaryType() const
{
    return m_boundaryType;
}

void CMindMapNodeBoundary::layoutContent()
{
    qreal           padding = 8;
	QRectF			r;
    QRectF			rect;

    r = m_label->boundingRect();
    rect = QRectF(-m_areaRect.width() / 2.0, -m_areaRect.height() / 2.0, m_areaRect.width(), m_areaRect.height() );

    m_label->setGeometry( QRectF(rect.topLeft() - QPointF(-padding, r.height() / 2.0), r.size() ) );
}

QMenu* CMindMapNodeBoundary::contextMenu()
{
    CMindMapNodeGroupMenu	*menu = NULL;

    menu = CMindMapMenuManager::instance()->nodeGroupMenu( CMindMapNode::Boundary );
    if (menu)
        menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

QStringList CMindMapNodeBoundary::filterProperties()
{
    QStringList         filters;

    filters << "startIndex"
            << "endIndex"
            << "groupType"
            << "boundaryType"
            << "id";

    return filters;
}

CMindMapNodeSummary::CMindMapNodeSummary(CDiagram *d, CMindMapNode *node)
    : CMindMapNodeGroup(d, node),
	  m_braceWidth(32)
{
    m_groupType = CMindMapNode::Summary;
}

CMindMapNodeSummary::~CMindMapNodeSummary()
{

}

QRectF CMindMapNodeSummary::boundingRect() const
{
	qreal		w, h;

    w = m_areaRect.width();
    h = m_areaRect.height();

    return QRectF(-w / 2.0, -h / 2.0, w, h);
}

QPainterPath CMindMapNodeSummary::shape() const
{
	QPainterPath		path;

	path.addPath(m_path);
	path.addRect(m_label->geometry());

    return path;
}

void CMindMapNodeSummary::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->save();
	painter->setPen( QPen(Qt::gray, 2, Qt::SolidLine) );
	painter->drawPath(m_path);
	painter->restore();
}

void CMindMapNodeSummary::relocate()
{
	qreal						w, h;
	QRectF						rect, textRect;
	QRectF                      areaRect;
	CMindMapNode				*node = NULL;
    QList<CMindMapNode*>        nodes;

    if (!m_node || m_startIndex < 0)
        return;

    nodes = m_node->childs(m_startIndex, m_endIndex);
    foreach (node, nodes)
    {
        QRectF  r;
        r = node->areaRect();
        areaRect = areaRect.unite( r );
    }

	if (node)
		setRelativePos( node->relativePosition() );

	textRect = m_label->boundingRect();
    switch(m_relativePos)
	{
	case CMindMapNode::North:
		w = areaRect.width();
		h = m_braceWidth;
		m_areaRect = QRectF(areaRect.topLeft() + QPointF(0, -h - GROUP_PADDING), QSizeF( w, h ) );
		break;

	case CMindMapNode::East:
		w = m_braceWidth;
		h = areaRect.height();
		m_areaRect = QRectF(areaRect.topLeft() + QPointF(GROUP_PADDING, 0), QSizeF(w, h));
		break;

	case CMindMapNode::South:
		w = areaRect.width();
		h = m_braceWidth;
		m_areaRect = QRectF(areaRect.bottomLeft() + QPointF(0, GROUP_PADDING), QSizeF(w, h));
		break;

	case CMindMapNode::West:
		w = m_braceWidth;
		h = areaRect.height();
		m_areaRect = QRectF(areaRect.topLeft() + QPointF(-w - GROUP_PADDING , 0), QSizeF(w, h));
		break;

	default:
		break;

	}; // End of switch
}

void CMindMapNodeSummary::layoutContent()
{
	QRectF			r;
	QRectF			rect;

	r = m_label->boundingRect();
	switch(m_relativePos)
	{
	case CMindMapNode::North:
		
		break;

	case CMindMapNode::East:

		break;

	case CMindMapNode::South:

		break;

	case CMindMapNode::West:

		break;

	default:
		break;

	}; // End of switch
}

void CMindMapNodeSummary::setBraceWidth(qreal value)
{
    m_braceWidth = value;
    update();
}

qreal CMindMapNodeSummary::braceWidth() const
{
    return m_braceWidth;
}

void CMindMapNodeSummary::setSize(qreal value)
{
    m_size = value;
    update();
}

qreal CMindMapNodeSummary::size() const
{
    return m_size;
}

void CMindMapNodeSummary::setPath(QPainterPath &value)
{
    m_path = value;
    update();
}

QPainterPath CMindMapNodeSummary::path() const
{
    return m_path;
}
