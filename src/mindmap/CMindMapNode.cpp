/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramNode>

#include "CMindMapNode.h"
#include "CMindMapNodeSprite.h"
#include "CMindMapNodeGroup.h"
#include "CMindMapCommands.h"
#include "CMindMapNodeRenderer.h"
#include "CMindMapPathRenderer.h"
#include "CMindMapRendererManager.h"

QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d)
{
    CMindMapNode            *node = NULL;
    QList<CDiagramItem*>    items;
    QList<CMindMapNode*>    nodes;

    if (!d)
        return nodes;

    items = d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node)
        {
            nodes.push_back(node);
        }
    }

    return nodes;
}

bool spriteOrderLessThan(CMindMapNodeSprite *s1, CMindMapNodeSprite *s2)
{
    return (s1->order() <= s2->order());
}

bool groupOrderLessThan(CMindMapNodeGroup *g1, CMindMapNodeGroup *g2)
{
    bool        r = false;

    r = (g1 == NULL) ? true : false;

    if (g1 && g2)
        r = (g1->startIndex() < g2->startIndex());

    return r;
}

CMindMapText::CMindMapText(CDiagram *parent)
	: CDiagramTextNode(parent)
{
    m_label->setLineEditMode(true);
    m_label->setAutoWidth(true);

    //disconnect(m_label, SIGNAL(sizeAutoChanged()), this, SLOT(onTextSizeChanged()));
	connect(m_label, SIGNAL(sizeAutoChanged()), this, SLOT(onTextSizeAutoChanged()));
}

void CMindMapText::onTextSizeAutoChanged()
{
	QRectF		r1, r2;
	qreal		w, h;

	r1 = geometry();
	r2 = m_label->boundingRect();
	w = qMax(r1.width(), r2.width());
	h = qMax(r1.height(), r2.height());

	setGeometry( QRectF(pos() + QPointF( (w - r1.width()) / 2.0, (h - r1.height()) / 2.0), QSizeF(w, h) ) );
}

CMindMapNodeCollapser::CMindMapNodeCollapser(CMindMapNode *parent)
    : QGraphicsItem(parent),
      m_node(parent),
      m_size(14),
      m_opened(true),
      m_hovered(false)
{
    setAcceptHoverEvents(true);
    setCursor( Qt::PointingHandCursor );
    setCacheMode( QGraphicsItem::NoCache );
    setFlag(QGraphicsItem::ItemStacksBehindParent );
}

CMindMapNodeCollapser::~CMindMapNodeCollapser()
{
}

QRectF CMindMapNodeCollapser::boundingRect() const
{
    qreal			w, h;
    QPainterPath	path;

    w = m_size;
    h = m_size;

    path.setFillRule(Qt::WindingFill);
    path.addRect( QRectF( -w / 2.0, -h / 2.0, w, h) );
    path.addPath( m_edgePath );

    return path.boundingRect();
}

QPainterPath CMindMapNodeCollapser::shape() const
{
    qreal			w, h;
    QPainterPath	path;

    w = m_size;
    h = m_size;

    if (m_node->isCollapsible())
        path.addRect( QRectF( -w / 2.0, -h / 2.0, w, h) );

    return path;
}

void CMindMapNodeCollapser::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen                        pen;
    QRectF                      r;
    qreal                       d = 4;
    CMindMapNodeRenderer        *rd = NULL;
    QLinearGradient				g(0,0,1,1);

    g.setCoordinateMode(QGradient::ObjectBoundingMode);
    g.setColorAt(0.0, Qt::white);
    g.setColorAt(0.48, QColor(246, 246, 246));
    g.setColorAt(1.0, QColor(237, 237, 237));

    r = QRectF(-m_size/2.0, -m_size/2.0, m_size, m_size);

    // edge should aways be draw
    rd = m_node->nodeRenderer();
    if (rd && !m_node->isCollapsed())
    {
        pen = edgePen();
        pen.setJoinStyle( Qt::RoundJoin );

        painter->save();
        painter->setPen( pen );
        painter->translate(-pos());
        painter->setRenderHint(QPainter::Antialiasing);
        m_edgePath = rd->render(m_node, painter, m_node->pathRenderer());
        m_edgePath = m_node->mapToItem(this, m_edgePath );
        painter->restore();
    }

    // only non-root nodes need to draw the collapser
    if (m_node->isCollapsible())
    {
        pen = QPen(Qt::darkGray, 1);

        painter->save();
        painter->setPen(pen);
        painter->setBrush(g);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawEllipse(r);

        painter->setPen( QPen(QColor("#333333"), 2) );
        painter->setRenderHint(QPainter::Antialiasing, false);
        if (m_opened)
        {
            painter->drawLine(QPointF(r.left() + d, 0), QPointF(r.right() - d, 0));
        }
        else
        {
            painter->drawLine(QPointF(r.left() + d, 0), QPointF(r.right() - d, 0));
            painter->drawLine(QPointF(0, r.top() + d), QPointF(0, r.bottom() - d));
        }
        painter->restore();
    }
}

void CMindMapNodeCollapser::setEdgePen(const QPen &value)
{
    m_edgePen = value;
    update();
}

QPen CMindMapNodeCollapser::edgePen()
{
    return m_edgePen;
}

void CMindMapNodeCollapser::hoverEnterEvent (QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;
}

void CMindMapNodeCollapser::hoverLeaveEvent (QGraphicsSceneHoverEvent *event)
{
    m_hovered = false;
}

void CMindMapNodeCollapser::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->button() & Qt::LeftButton) && m_node->isCollapsible() )
    {
        m_opened = !m_opened;
        if (m_node)
            m_node->setCollapsed(!m_opened);
    }
}

CMindMapNodeChildPanel::CMindMapNodeChildPanel(CMindMapNode *node)
    : QGraphicsItem(node),
      m_node(node),
      m_width(32),
      m_height(32)
{
    setAcceptDrops(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(true);
}

CMindMapNodeChildPanel::~CMindMapNodeChildPanel()
{

}

QRectF CMindMapNodeChildPanel::boundingRect() const
{
    QRectF      r;

    r = QRectF(-m_width / 2.0, -m_height / 2.0, m_width, m_height);
    return r;
}

QPainterPath CMindMapNodeChildPanel::shape() const
{
    return m_path;
}

void CMindMapNodeChildPanel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}

void CMindMapNodeChildPanel::updateSize()
{
    QRectF          r;
    QPainterPath    path;

    if (!m_node || !scene())
        return;

    path = m_node->connectArea();
    r = path.boundingRect();

    m_width = r.width();
    m_height = r.height();
    setPos(r.center());
    m_path = mapFromParent(path);

    update();
}

CMindMapNode* CMindMapNodeChildPanel::node() const
{
    return m_node;
}

void CMindMapNodeChildPanel::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
}

void CMindMapNodeChildPanel::dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
{
    event->accept();
}

void CMindMapNodeChildPanel::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void CMindMapNodeChildPanel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::RightButton)
    {

    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void CMindMapNodeChildPanel::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

CMindMapNode::CMindMapNode(CDiagram *d)
	: CDiagramNode(d),
	  m_upLevelNode(NULL),
	  m_shapeType( CMindMapNode::DefaultType ),
	  m_layoutType( CMindMapNode::DefaultLayout ),
	  m_pathType( CMindMapNode::DefaultLine ),
	  m_hovered(false),
	  m_customPen(false),
	  m_customFontSize(false),
      m_textWidth(0),
      m_outlineStroke(4),
	  m_level(0),
      m_index(-1),
	  m_collapser(NULL),
      m_childPanel(NULL),
	  m_collapsed(false),
	  m_relativePos(CMindMapNode::DefaultPosition),
      m_maxNodeId(1),
      m_nextSpriteId(0)
{
    QLinearGradient             g(0,0,1,1);
    QGraphicsDropShadowEffect	*effect = NULL;

	g.setCoordinateMode(QGradient::ObjectBoundingMode);
	g.setColorAt(0.0, QColor("#ffffff"));
    g.setColorAt(1.0, QColor("#9ABA58"));

    effect = new QGraphicsDropShadowEffect();
    effect->setColor( QColor(0, 0, 0, 32) );
    effect->setOffset(2, 3);

    setGraphicsEffect(effect);
    setEffectEnabled(false);
    setName( NODE_NAME );
    setCategory( NODE_CATEGORY );
	// setBrush( QBrush(g) );
	setBrush( QBrush(QColor("#FF99CC")) );
    setPen( QPen(QColor("#FF99CC"), 2) );
    setConnectable(true);
	setAcceptHoverEvents(true);
	setFontFamily("Tahoma");
    setMovable(false);
	setFontBold(false);
	setFontItalic(false);
	setFontUnderline(false);
	setFontSize(-1);
	setZValue(1.0);

    showLabel(false);

    m_collapser = new CMindMapNodeCollapser(this);
	m_collapser->setVisible(false);
	m_collapser->setZValue(1.0);
	m_collapser->setEdgePen(activePen());

    m_childPanel = new CMindMapNodeChildPanel(this);

	m_label->setStyleable(false);
	m_text = tr("Main Topic");

	m_editor = new CMindMapText(NULL);
	m_editor->setParentItem(this);
	m_editor->setPen(QPen(QColor("#4E80BC"), 1));
	m_editor->setBrush(QBrush(Qt::white));
	m_editor->setFlag(QGraphicsItem::ItemIsSelectable, false);
	m_editor->setFlag(QGraphicsItem::ItemIsMovable, false);
	m_editor->setVisible(false);
	m_editor->setTextAlign(Qt::AlignLeft);
	m_editor->setTextStyleable(false);
    m_editor->setFont( QFont("Arial", 16) );

    m_sprites.clear();
	// used to load sub items when loaded from file
	if (d)
		connect(d, SIGNAL(itemsLoadFinished()), this, SLOT(onRebuildChilds()));
	connect(this, SIGNAL(propertyChanged(const QString&, const QVariant&)), this, SLOT(onPropertyChanged(const QString&, const QVariant&)));
}

CMindMapNode::~CMindMapNode()
{
	SAFE_DELETE(m_collapser);
}

QSizeF CMindMapNode::preferredSize() const
{
    int                                 countX = 0, countY = 0;
    qreal                               hWidth = 0, vWidth = 0, hHeight = 0, vHeight = 0;
    QString                             tabString("\t"), target = m_text;
    QString                             repString(" ");
    QString                             tmpText = target.replace(tabString, repString);
    QFontMetrics                        metrics( QFont(fontFamily(), fontSize(), fontBold(), fontItalic() ) );
    QList<CMindMapNodeSprite*>          sprites;
    QList<QList<CMindMapNodeSprite*> >  allSprites;

    // calculate all sprites's geometry affects
    allSprites = m_sprites.values();
    for (int i = 0; i < allSprites.length(); ++i)
    {
        sprites.append( allSprites.at(i) );
    }

    foreach (CMindMapNodeSprite *sprite, sprites)
    {
        QRectF      r;

        switch(sprite->spriteType())
        {
        case CMindMapNode::Icon:
        case CMindMapNode::File:
        case CMindMapNode::Link:
        case CMindMapNode::Note:
		case CMindMapNode::TaskPriority:
		case CMindMapNode::TaskProgress:
            r = sprite->boundingRect();
            hWidth += r.width();
            hHeight = qMax(r.height(), hHeight);
            ++countX;
            break;

        case CMindMapNode::Pixmap:
        case CMindMapNode::Task:
            r = sprite->boundingRect();
            vHeight += r.height();
            vWidth = qMax(vWidth, r.width());
            ++countY;
            break;

        default:
            break;
        }
    }

    // calculate all text geometry affects
	if (!m_text.isEmpty())
    {
        hWidth += metrics.width(tmpText);
        hHeight = qMax(hHeight, metrics.height() + 0.0);
    }

	// calculate the inter items padding affect to the geometry
    hWidth += (hSpacing() * countX);
    vHeight += (vSpacing() * countY);

	vWidth = qMax(hWidth, vWidth) + 2 * xpadding();
	vHeight += (hHeight + 2 * ypadding());

    // adjust a little according to the node type
    switch (nodeType() )
    {
    case Ellipse:
        {
            qreal delta = 1.2;
            qreal t1 = delta * vHeight;
            qreal t2 = vWidth / qSqrt(1 - 1/(delta * delta));
            vHeight = t1;
            vWidth = t2;
            break;
        }

    case Diamond:
        {
            qreal delta = vHeight / vWidth;
            qreal t1 = vHeight + delta * vWidth;
            qreal t2 = t1 / delta;
            vHeight = t1;
            vWidth = t2;
            break;
        }

    default:
        break;
    }; // End of switch

    return QSizeF(vWidth, vHeight);
}

QRectF CMindMapNode::contentRect() const
{
	QSizeF		s;

	s = preferredSize();
	return QRectF(QPointF(-s.width() / 2.0, -s.height()/ 2.0), s);
}

QRectF CMindMapNode::outlineRect() const
{
	QSizeF		s;
	qreal		w, h, padding = 2;

	s = preferredSize();
	w = s.width() + 2 * (padding + activePen().widthF());
	h = s.height() + 2 * (padding + activePen().widthF());

	return QRectF( -w / 2.0, -h / 2.0, w, h);
}

QRectF CMindMapNode::boundingRect() const
{
	qreal		w, h, pw = 0;
	QSizeF		s;
	
	s = preferredSize();
	w = s.width();
	h = s.height();
    pw = 2 * m_outlineStroke;

	return QRectF(-w / 2.0 - pw, -h / 2.0 - pw, w + 2.0 * pw , h + 2.0 * pw);
}

QPainterPath CMindMapNode::shape() const
{
	qreal			w, h, pw;
	QRectF			r;
	QPainterPath	path;

	r = contentRect();
	w = r.width();
	h = r.height();
	pw = m_outlineStroke;

	path.addRect( QRectF(-w / 2.0 - pw, -h / 2.0 - pw, w + 2.0 * pw , h + 2.0 * pw) );

	return path;
}

void CMindMapNode::paint(QPainter *painter,  const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

    QSizeF					size;
    QRectF					rect;
    qreal					w, h;
    qreal                   defaultRadius = 4;
	qreal					xradius = 8, yradius = 8;
	QPainterPath			path;
	QFont					font(fontFamily(), fontSize(), fontBold(), fontItalic());

    size = preferredSize();
    layoutContent();
    w = size.width();
	h = size.height();
	rect = QRectF(  QPointF( -w / 2.0, -h / 2.0), size );

    painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen( activePen() );
	painter->setBrush( brush() );

    m_shape = QPainterPath();
	switch(nodeType())
	{
	case Rect:
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawRoundedRect(rect, defaultRadius, defaultRadius);
		m_shape.addRect(rect);
		break;

	case RoundedRect:
        xradius = yradius = qMin(rect.width() / 2.0, rect.height() / 2.0);
        painter->drawRoundedRect(rect, xradius, yradius);
		m_shape.addRoundedRect(rect, xradius, yradius);
		break;

	case Ellipse:
		painter->drawEllipse( rect );
		m_shape.addEllipse(rect);
		break;

	case Diamond:
		path.moveTo( rect.topLeft() + QPointF(0, h / 2.0) );
		path.lineTo( rect.topLeft() + QPointF(w / 2.0, 0) );
		path.lineTo( rect.topRight() + QPointF(0, h / 2.0) );
		path.lineTo( rect.center() + QPointF(0, h / 2.0) );
		path.closeSubpath();
		painter->drawPath(path);
		m_shape.addPath(path);
		break;

    case Underline:
        painter->drawLine( rect.bottomLeft(), rect.bottomRight() );
		m_shape.addRect(rect);
		break;

	case FishHeadLeft:
		path.moveTo ( rect.topLeft() + QPointF(0, h / 2.0) );
		path.cubicTo( rect.topLeft() + QPointF(w / 3.0, 0), rect.topRight() + QPointF(-w / 3.0, 0), rect.topRight() );
		path.lineTo ( rect.bottomRight() );
		path.cubicTo( rect.bottomRight() + QPointF(-w / 3.0, 0), rect.bottomLeft() + QPointF(w / 3.0, 0), rect.topLeft() + QPointF(0, h / 2.0) );
	
		painter->drawPath(path);
		m_shape.addPath(path);
		break;

	case FishHeadRight:
		path.moveTo ( rect.topLeft() );
		path.cubicTo( rect.topLeft() + QPointF(w / 3.0, 0), rect.topRight() + QPointF(-w / 3.0, 0), rect.topRight() + QPointF(0, h / 2.0) );
		path.cubicTo( rect.bottomRight() + QPointF(-w / 3.0, 0), rect.bottomLeft() + QPointF(w / 3.0, 0), rect.bottomLeft() );
		path.lineTo ( rect.topLeft() );

		painter->drawPath(path);
		m_shape.addPath(path);
		break;

	case NoneType:
		m_shape.addRect(rect);
		break;

	default:
		break;

	}; // End of switch

    // draw the text
	font.setUnderline( fontUnderline() );
	if (fontBold())
		painter->setPen( QPen(m_textColor, 2) );
	else
		painter->setPen( QPen(m_textColor, 1) );
	painter->setFont( font );
    painter->drawText(m_textRect, Qt::AlignHCenter | Qt::AlignVCenter, m_text );

	// draw the outline
	rect = outlineRect();
	if (isSelected())
	{
		painter->setPen( QPen(Qt::blue, m_outlineStroke, Qt::SolidLine ) );
		painter->setBrush( Qt::NoBrush );
		painter->drawRoundedRect( rect, 8, 8 );
	}
	else if (m_hovered)
	{
		painter->setPen( Qt::NoPen );
		painter->setBrush( QColor(0, 0, 0, 48) );
		painter->drawRoundedRect( rect, 8, 8 );
	}

	painter->restore();
}

void CMindMapNode::addChild(CMindMapNode *node, CMindMapNode *ref, bool beforeRef)
{
	int		index = -1;
	
	if (!node)
		return;

	if (ref)
	{
		index = m_childs.indexOf(ref);
		if (!beforeRef)
			index += 1;
	}
	else
	{
		index = m_childs.length();
	}

	addChild(node, index);
}

void CMindMapNode::addChild(CMindMapNode *node, int index)
{
	// make sure the index value is valid
    if (index > m_childs.length())
		return;

	// simply insert the node
    if (index < 0)
        m_childs.append(node);
    else
        m_childs.insert(index, node);

	node->setUpLevelNode(this);
	node->setLevel(level() + 1);

	if (m_collapser && !m_collapser->isVisible())
	{
		m_collapser->setVisible(true);
		m_collapser->setEdgePen(activePen());
	}

    setCollapsed(false);
	onStructChanged();
}

void CMindMapNode::dropChild(CMindMapNode *node)
{
	if (!node)
		return;

	node->setUpLevelNode(NULL);
	m_childs.removeAll(node);

	if (m_childs.length() == 0 && m_collapser)
		m_collapser->setVisible(false);

	onStructChanged();
}

QList<CMindMapNode*> CMindMapNode::childs()
{
	return m_childs;
}

QList<CMindMapNode*> CMindMapNode::childs(int startIndex, int endIndex)
{
    QList<CMindMapNode*>    nodes;

    if (startIndex > m_childs.length() - 1 || endIndex > m_childs.length() - 1)
        return nodes;

    nodes = m_childs.mid(startIndex, endIndex + 1 - startIndex);
    return nodes;
}

int CMindMapNode::childCount()
{
	return m_childs.length();
}

int CMindMapNode::indexOf(CMindMapNode *node)
{
	return m_childs.indexOf(node);
}

CMindMapNode* CMindMapNode::childAt(int index)
{
	if (index < 0 || index > m_childs.length() - 1)
		return NULL;

	return m_childs.at(index);
}

void CMindMapNode::reorder(CMindMapNode *node, int index)
{
    if (!node || index < 0 || index > m_childs.length() )
        return;

    m_childs.removeAll(node);
    m_childs.insert(index, node);

    onStructChanged();
}

void CMindMapNode::setCustomPen(bool value)
{
	m_customPen = value;
}

bool CMindMapNode::customPen()
{
	return m_customPen;
}

void CMindMapNode::setCustomFontSize(bool value)
{
	m_customFontSize = value;
}

bool CMindMapNode::customFontSize()
{
	return m_customFontSize;
}

QPen CMindMapNode::activePen() const
{
	if (m_customPen)
		return m_pen;
	else if (m_upLevelNode)
		return m_upLevelNode->activePen();
	else
		return m_pen;
}

void CMindMapNode::setEdgePen(const QPen &value)
{
	if (m_collapser)
		m_collapser->setEdgePen(value);

	foreach (CMindMapNode *n, m_childs)
	{
		if (n && !n->customPen())
			n->setEdgePen(value);
	}

	update();
}

QPen CMindMapNode::edgePen() const
{
	QPen	p;

	if (m_collapser)
		p = m_collapser->edgePen();
	return p;
}

QPainterPath CMindMapNode::nodeShape() const
{
    QPainterPath        path;

    if (m_shape.isEmpty())
        path.addRect(contentRect());
    else
        path = m_shape;

    return path;
}

void CMindMapNode::setNodeType(int value)
{
	m_shapeType = value;
	onStructChanged();
}

int CMindMapNode::nodeType() const
{
	int		r;

	r = m_shapeType;
	if (m_shapeType == DefaultType)
	{
		switch(level())
		{
		case 0:
			r = CMindMapNode::RoundedRect;
			break;

		case 1:
			r = CMindMapNode::Rect;
			break;

		default:
			r = CMindMapNode::Underline;
			break;

		};  // End of switch
	}
	
	return r;
}

void CMindMapNode::setCollapsed(bool value)
{
	CMindMapNode	*node = NULL;

	for (int i = 0; i < m_childs.length(); ++i)
	{
		node = m_childs.at(i);
		node->toggleVisible(!value);
	}

	m_collapsed = value;
	onStructChanged();
}

bool CMindMapNode::isCollapsed() const
{
	return m_collapsed;
}

bool CMindMapNode::isCollapsible() const
{
	bool					r = true;
	CMindMapNodeRenderer	*renderer = NULL;

	if (level() == 0)
		return false;

	renderer = nodeRenderer();
	if (renderer)
		r = renderer->collapsible();
	return r;
}

void CMindMapNode::setUpLevelNode(CMindMapNode *node)
{
	m_upLevelNode = node;
}

CMindMapNode* CMindMapNode::upLevelNode()
{
	return m_upLevelNode;
}

bool CMindMapNode::isDescendantOf(CMindMapNode *node)
{
	bool	r = (m_upLevelNode == node);
	
	if (!r && m_upLevelNode)
		r = m_upLevelNode->isDescendantOf(node);

	return r;
}

void CMindMapNode::setRelativePosition(int pos)
{
	m_relativePos = pos;
}

int CMindMapNode::relativePosition() const
{
	return m_relativePos;
}

CMindMapNodeRenderer* CMindMapNode::nodeRenderer() const
{
	CMindMapNodeRenderer	*r = NULL;
	CMindMapRendererManager	*manager = NULL;

	manager = CMindMapRendererManager::instance(); 
	if (manager)
	{
		if (m_layoutType == CMindMapNode::DefaultLayout)
		{
			if (isRoot())
			{
				r = manager->getNodeRenderer( CMindMapNode::ClockwiseMindMap );
			}
			else
			{
				if (level() == 1)
				{
					switch(m_upLevelNode->layoutType())
					{
					case DefaultLayout:
					case MindMap:
					case ClockwiseMindMap:
					case AntiClockwiseMindMap:
						if (m_relativePos == CMindMapNode::East)
							r = manager->getNodeRenderer( CMindMapNode::RightLogic ); 
						else
							r = manager->getNodeRenderer( CMindMapNode::LeftLogic );
						break;

					case DownsideOrganization:
					case UpsideOrganization:
					case LeftTree:
					case RightTree:
					case LeftLogic:
					case RightLogic:
						r = m_upLevelNode->nodeRenderer();
						break;

					case LeftFishBone:
					case RightFishBone:
					default:
						r = manager->getNodeRenderer( CMindMapNode::RightLogic );
						break;
					};
				}
				else
				{
					r = m_upLevelNode->nodeRenderer();
				}
			}
		}
		else
		{
			r = manager->getNodeRenderer(m_layoutType);
		}
	}

	return r;
}

CMindMapPathRenderer* CMindMapNode::pathRenderer() const
{
	CMindMapPathRenderer	*r = NULL;
	CMindMapRendererManager	*manager = NULL;

	manager = CMindMapRendererManager::instance(); 
	if (manager)
	{
		if (m_pathType == CMindMapNode::DefaultLine)
		{
			if (isRoot())
				r = manager->getPathRenderer( CMindMapNode::CurveLine );
			else
				r = manager->getPathRenderer( CMindMapNode::PolygonLine );
		}
		else
		{
			r = manager->getPathRenderer(m_pathType);
		}
	}

	return r;
}

void CMindMapNode::setLayoutType(int layout)
{
	m_layoutType = layout;
	onStructChanged();
}

int CMindMapNode::layoutType() const
{
	return m_layoutType;
}

void CMindMapNode::setPathType(int path)
{
	m_pathType = path;
	onStructChanged();
}

int CMindMapNode::pathType() const
{
	return m_pathType;
}

void CMindMapNode::setAreaRect(const QRectF &value)
{
    m_areaRect = value;
}

QRectF CMindMapNode::areaRect()
{
    return m_areaRect;
}

void CMindMapNode::setAreaSize(const QSizeF &value)
{
	m_areaSize = value;
}

QSizeF CMindMapNode::areaSize()
{
	if (m_areaSize.isNull() || isCollapsed() )
		setAreaSize( recalc() );

	return m_areaSize;
}

QSizeF CMindMapNode::contentSize() const
{
    return m_contentSize;
}

QRectF CMindMapNode::printArea()
{
	return mapToScene( boundingRect() ).boundingRect();
}

QPainterPath CMindMapNode::connectArea()
{
    QPainterPath            p;
    CMindMapNodeRenderer    *r = NULL;

    r = nodeRenderer();
    if (r)
    {
        p = r->connectArea(this);
    }

    return p;
}

int CMindMapNode::connectIndex(const QPointF &p)
{
    int                     index = -1;
    CMindMapNodeRenderer    *r = NULL;

    r = nodeRenderer();
    if (r)
    {
        index = r->connectIndex(this, p);
    }

    return index;
}

QGraphicsItem* CMindMapNode::collapser()
{
	return m_collapser;
}

QSizeF CMindMapNode::recalc()
{
	QSizeF					s;
	CMindMapNode			*node = NULL;
	CMindMapNodeRenderer	*rd = NULL;

	// if is collapsed,we only return the size of
	// the node itself
	if (isCollapsed())
	{
		s = contentRect().size();
	}
	else
	{
		// Algorithm: first, we let the child nodes calculate
		// their size recursively, and then we use the renderer to
		// calculate this node's size according to the childs's
		// sizes, so the renderer don't have to perform recursively
		// calculation, it only have to handle one level childs.
		for (int i = 0; i < m_childs.length(); ++i)
		{
			node = m_childs.at(i);
			if (node)
				node->recalc();
		}

		rd = nodeRenderer();
		if (rd)
			s = rd->recalc(this);
	}

	setAreaSize(s);
	return s;
}

void CMindMapNode::layout()
{
	CMindMapNode			*node = NULL;
	CMindMapNodeRenderer	*rd = NULL;

	if (isCollapsed())
		return;

	// Layout is different from size recalculation, the parent node
	// should first relocate all the first level of its childs according
	// to the size, then position the grandchilds recusively.
	rd = nodeRenderer();
	if (rd)
		rd->layout(this);

	// after the parent node is positioned, then the childs are
	// placed according to the parent's position.
	for (int i = 0; i < m_childs.length(); ++i)
	{
		node = m_childs.at(i);
		if (node)
			node->layout();
	}

    if (m_collapser)
    {
        m_collapser->setEdgePen(edgePen());
    }

    // trigger the update of child panel
    if (m_childPanel)
    {
        m_childPanel->updateSize();
    }

    for (int i = 0; i < m_groups.length(); ++i)
        m_groups.at(i)->relocate();
}

void CMindMapNode::setLevel(int value)
{
	if (m_level == value)
		return;

	m_level = value;

	foreach (CMindMapNode *node, m_childs)
	{
		node->setLevel(value + 1);
	}
	update();
}

int CMindMapNode::level() const
{
	return m_level;
}

void CMindMapNode::setIndex(int index)
{
    m_index = index;
}

int CMindMapNode::index() const
{
    return m_index;
}

bool CMindMapNode::isRoot() const
{
	return (m_upLevelNode == NULL || m_level == 0);
}

void CMindMapNode::setFontFamily(const QString &value)
{
	if (value.isEmpty())
		return;

	m_font.setFamily(value);
	update();
}

QString CMindMapNode::fontFamily() const
{
	return m_font.family();
}

void CMindMapNode::setFontSize(int value)
{
	if (value > 0)
	{
		m_customFontSize = true;
		m_font.setPointSize(value);
	}
	else
	{
		value = fontSize();
		m_customFontSize = false;
		m_font.setPointSize(value);
	}

	update();
	onStructChanged();
}

int CMindMapNode::fontSize() const
{
	int		r;

	if (!m_customFontSize)
	{
		if (m_level == 0)
            r = 18;
		else if (m_level == 1)
            r = 14;
		else
            r = 12;
	}
	else
	{
		r = m_font.pointSize();
	}
	return r;
}

void CMindMapNode::setFontItalic(bool value)
{
	m_font.setItalic(value);
	update();
}

bool CMindMapNode::fontItalic() const
{
	return m_font.italic();
}

void CMindMapNode::setFontBold(bool value)
{
	m_font.setBold(value);
	update();
}

bool CMindMapNode::fontBold() const
{
	return m_font.bold();
}

void CMindMapNode::setFontUnderline(bool value)
{
	m_font.setUnderline(value);
	update();
}

bool CMindMapNode::fontUnderline() const
{
	return m_font.underline();
}

void CMindMapNode::toggleVisible(bool value)
{
	CMindMapNode	*node = NULL;

	setVisible(value);
	if (!isCollapsed())
	{
		for (int i = 0; i < m_childs.length(); ++i)
		{
			node = m_childs.at(i);
			node->toggleVisible(value);
		}
	}
}

QVariant CMindMapNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch(change)
	{
	case QGraphicsItem::ItemPositionHasChanged:
		if (isRoot())
		{
			layout();
		}
		break;

	case QGraphicsItem::ItemSelectedHasChanged:
		break;

	default:
		break;
	};

	return CDiagramNode::itemChange(change, value);
}

void CMindMapNode::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Enter:
	case Qt::Key_Return:
		{
			if (isRoot())
			{
                m_p->addUndoCommand(new CMindMapNewNodeCommand(m_p,
                                                               this,
                                                               NULL,
                                                               false,
                                                               (event->modifiers() & Qt::Key_Alt)));
			}
			else
			{
                m_p->addUndoCommand(new CMindMapNewNodeCommand(m_p,
                                                               m_upLevelNode,
                                                               this,
                                                               false,
                                                               (event->modifiers() & Qt::Key_Alt)));
			}
			event->accept();
		}
		break;

    case Qt::Key_Escape:
        break;

	default:
		if (!event->text().isEmpty() && !(event->modifiers() & Qt::ControlModifier))
		{
            startEditingContent();

			// resend the keyevent, so the input will be reflected
			// in the node immediatly.
			QApplication::sendEvent(scene(), event);
		}
		else
		{
			QGraphicsItem::keyPressEvent(event);
		}
		break;
	}; // End of switch
}

void CMindMapNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF     p;

    p = event->scenePos();
    if (mapToScene(shape()).contains(p))
    {
        m_hovered = true;
        update();
    }

    CDiagramNode::hoverEnterEvent(event);
}

void CMindMapNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	m_hovered = false;
	update();

    CDiagramNode::hoverLeaveEvent(event);
}

CMindMapNode* CMindMapNode::createChildNode()
{
	QString			t1("Topic %1"), t2("Branch %1");
	CMindMapNode	*r = NULL;

	r = new CMindMapNode(m_p);
	r->setLibraryCategory( libraryCategory() );
	r->setLibraryName( libraryName() );
	r->setCategory( category() );
	r->setName( name() );
	r->setLevel(level() + 1);
    r->setMovable(true);
	
	if (level() == 0)
	{
		r->setBrush( Qt::white );
		r->setText( t2.arg( nextNodeId() ) );
	}
	else
	{
		r->setBrush( Qt::NoBrush );
		r->setText( t1.arg( nextNodeId() ) );
	}
	return r;
}

qreal CMindMapNode::xpadding() const
{
    qreal	r;

	if (m_level == 0)
        r = 16;
	else if (m_level == 1)
        r = 16;
	else
        r = 16;
	return r;
}

qreal CMindMapNode::ypadding() const
{
	qreal	r;

	if (m_level == 0)
        r = 12;
	else if (m_level == 1)
        r = 6;
	else
        r = 4;
	return r;
}

qreal CMindMapNode::hSpacing() const
{
    return 4;
}

qreal CMindMapNode::vSpacing() const
{
    return 4;
}

int CMindMapNode::nextNodeId()
{
	return m_maxNodeId ++;
}

void CMindMapNode::onStructChanged()
{
    QRectF      r;

    if (!m_p)
        return;

	if (m_upLevelNode)
	{
		m_upLevelNode->onStructChanged();
	}
	else
	{
		recalc();
		layout();

        foreach (CMindMapNode *node, m_childs)
        {
            r = r.unite( node->areaRect() );
        }

        m_areaRect = r;
        emit areaChanged( r );
	}

    for (int i = 0; i < m_childs.length(); ++i)
        m_childs.at(i)->setIndex(i);

    update();
}

void CMindMapNode::setText(const QString &value)
{
	m_text = value;

	update();
}

QString CMindMapNode::text()
{
	return m_text;
}

void CMindMapNode::startEditingContent()
{
	QRectF		r;

	r = contentRect();
	//stackBefore(m_editor);

	m_editor->setPlainText( m_text );
	m_editor->setGeometry( QRectF(r.center(), r.size()) );

	m_editor->layoutContent();
	m_editor->setEditing(true);
	m_editor->setVisible(true);
	m_editor->setZValue(1.0);
	connect((QObject*)m_p, SIGNAL(textChanged(CDiagramText*)), this, SLOT(onContentChanged(CDiagramText*)));
}

void CMindMapNode::finishEditingContent()
{
	m_editor->setVisible(false);
	disconnect((QObject*)m_p, SIGNAL(textChanged(CDiagramText*)), this, SLOT(onContentChanged(CDiagramText*)));
}

void CMindMapNode::onContentChanged(CDiagramText *text)
{
    QString                 plainText;
    QList<CDiagramItem*>    nodes;

    if (!text)
		return;

	finishEditingContent();

    plainText = text->toPlainText();
    if (m_p)
    {
        nodes << this;
        m_p->addUndoCommand(new CMindMapChangePropertyCommand(m_p,
                                                              nodes,
                                                              "text",
                                                              QVariant(plainText),
                                                              NULL) );
    }
    else
    {
        setText( plainText );
    }

	onStructChanged();

	setSelected(true);
}

void CMindMapNode::onPropertyChanged(const QString &name, const QVariant &value)
{
	CMindMapNode	*node = NULL;

	if (name == "pen")
	{
		setCustomPen(true);
		for (int i = 0; i < m_childs.length(); ++i)
		{
			node = m_childs.at(i);
			if (node && !node->customPen())
				node->setEdgePen( value.value<QPen>() );
		}

		if (m_collapser)
			m_collapser->setEdgePen(activePen());
	}
}

void CMindMapNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        startEditingContent();
    else
        CDiagramNode::mouseDoubleClickEvent(event);
}

void CMindMapNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton)
    {
        // Nothing
    }
    else
    {
        CDiagramNode::mousePressEvent(event);
    }
}

QStringList CMindMapNode::childsId()
{
	QStringList		r;

	m_childsId.clear();
	foreach (CMindMapNode *node, m_childs)
	{
		m_childsId.push_back( node->id() );
		r.push_back( QString::number( node->id() ) );
	}

	return r;
}

void CMindMapNode::setChildsId(const QStringList &value)
{
	m_childsId.clear();
	for (int i = 0; i < value.length(); ++i)
	{
		m_childsId.push_back( value.at(i).toInt() );
	}
}

QList<int> CMindMapNode::childsIdArray()
{
    return m_childsId;
}

void CMindMapNode::onRebuildChilds()
{
	QList<int>			ids;
	CMindMapNode		*node = NULL;

	ids = m_childsId;
    m_childs.clear();
	foreach (int id, ids)
	{
		node = dynamic_cast<CMindMapNode*>( m_p->getItemById(id) );
		if (node)
		{
			m_childs.push_back(node);
			node->setUpLevelNode(this);
		}
	}

	// collapser controller may be seen
	m_collapser->setVisible( m_childs.length() > 0 );
	
	// clear subitem's ids
	m_childsId.clear();

    onStructChanged();
}

bool CMindMapNode::isAlignable() const
{
	return false;
}

bool CMindMapNode::isAnchorable() const
{
	return false;
}

void CMindMapNode::setMovable(bool value)
{
    setFlag(QGraphicsItem::ItemIsMovable, value);
}

bool CMindMapNode::isMovable() const
{
    return (flags() & QGraphicsItem::ItemIsMovable);
}

void CMindMapNode::setCollapserPos(const QPointF &value)
{
	if (m_collapser)
		m_collapser->setPos(value);
}

QPointF CMindMapNode::collapserPos() const
{
	QPointF		r;

	if (m_collapser)
		r = m_collapser->pos();
	return r;
}

void CMindMapNode::layoutContent()
{
    qreal                               w = 0, h = 0, hDist = 0, wDist = 0;
    qreal                               leftWidth, rightWidth;
    qreal                               topHeight, bottomHeight, midHeight = 0;
    QRectF                              r;
    QString                             tabString("\t"), target = m_text;
    QString                             repString(" ");
    QString                             tmpText = target.replace(tabString, repString);
    QFontMetrics                        metrics( QFont(fontFamily(), fontSize(), fontBold(), fontItalic() ) );
    QList<CMindMapNodeSprite*>          sprites;
    QList<CMindMapNodeSprite*>          left, right, top, bottom;
    QList<QList<CMindMapNodeSprite*> >  allSprites;

    r = contentRect();
    leftWidth = xpadding();
    rightWidth = xpadding();
    topHeight = ypadding();
    bottomHeight = ypadding();

    // size allocate
    allSprites = m_sprites.values();
    for (int i = 0; i < allSprites.length(); ++i)
    {
        sprites.append( allSprites.at(i) );
    }

    foreach (CMindMapNodeSprite *sprite, sprites)
    {
        switch(sprite->spriteType())
        {
        case CMindMapNode::Icon:
		case CMindMapNode::TaskPriority:
		case CMindMapNode::TaskProgress:
            left.push_back(sprite);
            leftWidth += sprite->boundingRect().width() + hSpacing();
            midHeight = qMax(midHeight, sprite->boundingRect().height() + vSpacing());
            break;

        case CMindMapNode::File:
        case CMindMapNode::Link:
        case CMindMapNode::Note:
            right.push_back(sprite);
            rightWidth += sprite->boundingRect().width() + hSpacing();
            midHeight = qMax(midHeight, sprite->boundingRect().height() + vSpacing());
            break;

        case CMindMapNode::Pixmap:
            top.push_back(sprite);
            topHeight += sprite->boundingRect().height() + vSpacing();
            break;

        case CMindMapNode::Task:
            bottom.push_back(sprite);
            bottomHeight += sprite->boundingRect().height() + vSpacing();
            break;

        default:
            break;
        }
    }

    //qSort(left.begin(), left.end(), spriteOrderLessThan);
    qSort(right.begin(), right.end(), spriteOrderLessThan);

    // calculate all text geometry affects
    m_textWidth= 0;
    if (!m_text.isEmpty())
    {
        midHeight = qMax(midHeight, metrics.height() + 0.0);
        m_textWidth = metrics.width(tmpText);
    }

    m_contentSize = QSizeF(leftWidth + rightWidth + m_textWidth + hSpacing(),
                           topHeight + bottomHeight + midHeight + vSpacing());

    hDist = r.height() - m_contentSize.height();
    hDist = (hDist > 0) ? hDist / 2.0 : 0;

    wDist = r.width() - m_contentSize.width();
    wDist = (wDist > 0) ? wDist / 2.0 : 0;

    // calculate the text height
    m_textRect = QRectF(r.left() + wDist + leftWidth,
                        r.top() + hDist + topHeight,
                        m_textWidth,
                        midHeight );

    // place the sprites on tops
    h = ypadding() + hDist;
    foreach (CMindMapNodeSprite *sprite, top)
    {
        QRectF  br;

        br = sprite->boundingRect();
        sprite->setPos( QPointF(0, r.top() + h + br.height() / 2.0) );
        h += (br.height() + vSpacing());
    }

    // place the sprites on the left
    w = xpadding() + wDist;
    foreach (CMindMapNodeSprite *sprite, left)
    {
        QRectF  br;

        br = sprite->boundingRect();
        sprite->setPos( QPointF(r.left() + w, r.top() + h + midHeight / 2.0) );
        w += (br.width() + hSpacing());
    }

    // place the sprites on the right
    w =  xpadding() + wDist;
    foreach (CMindMapNodeSprite *sprite, right)
    {
        QRectF  br;

        br = sprite->boundingRect();
        sprite->setPos( QPointF(r.right() - w, r.top() + h + midHeight / 2.0 ) );
        w += (br.width() + hSpacing());
    }

    // place the sprites on bottom
    h = ypadding() + hDist;
    foreach (CMindMapNodeSprite *sprite, bottom)
    {
        QRectF  br;

        br = sprite->boundingRect();
        sprite->setPos( QPointF(0, r.bottom() - h - br.height() / 2.0) );
        h += (br.height() + vSpacing());
    }
}

void CMindMapNode::addSprite(CMindMapNodeSprite *sprite)
{
    int    type = 0;

    if (!sprite)
        return;

    if (sprite->spriteId() < 1)
        sprite->setSpriteId(nextSpriteId());

    type = sprite->spriteType();
    if (!m_sprites.contains(type))
    {
        m_sprites.insert(type, QList<CMindMapNodeSprite*>() );
    }
    m_sprites[type].append(sprite);
    sprite->setParentItem(this);

    layoutContent();
    onStructChanged();
}

void CMindMapNode::dropSprite(CMindMapNodeSprite *sprite)
{
    int     type = 0;

    type = sprite->spriteType();
    if (m_sprites.contains(type))
    {
        m_sprites[type].removeAll( sprite );
    }

    onStructChanged();
    layoutContent();
}

void CMindMapNode::dropSprites(int type)
{
    if (m_sprites.contains(type))
    {
        m_sprites.remove(type);
    }

    onStructChanged();
    layoutContent();
}

QList<CMindMapNodeSprite*> CMindMapNode::sprites(int type) const
{
    QList<CMindMapNodeSprite*>      r;

    if (m_sprites.contains(type))
    {
        r = m_sprites[type];
    }
    return r;
}

CMindMapNodeSprite* CMindMapNode::getSpriteById(int value)
{
    QList<int>                  keys;
    CMindMapNodeSprite          *r = NULL;
    CMindMapNodeSprite          *sprite = NULL;
    QList<CMindMapNodeSprite*>  allSprites;

    keys = m_sprites.keys();
    foreach (int key, keys)
    {
        allSprites = m_sprites[key];
        foreach (sprite, allSprites)
        {
            if (sprite->spriteId() == value)
            {
                r = sprite;
                break;
            }
        }

        if (r)
            break;
    }

    return r;
}

void CMindMapNode::setNextSpriteId(int value)
{
    if (value >= 0)
        m_nextSpriteId = value;
}

int CMindMapNode::nextSpriteId()
{
    int         r = 0;

    r = m_nextSpriteId;
    ++ m_nextSpriteId;

    return r;
}

bool CMindMapNode::checkExistedGroup(int startIndex, int endIndex, int groupType)
{
    bool    r = false;

    foreach (CMindMapNodeGroup *g, m_groups)
    {
        if (g->startIndex() == startIndex &&
            g->endIndex() == endIndex &&
            g->groupType() == groupType)
        {
            r = true;
            break;
        }
    }

    return r;
}

bool CMindMapNode::addNodeGroup(CMindMapNodeGroup *group)
{
    bool    r = true;
    int     startIndex, endIndex;

    if (!group)
        return false;

    startIndex = group->startIndex();
    endIndex = group->endIndex();
    r = checkExistedGroup(startIndex, endIndex, group->groupType());

    if (!r)
    {
        group->setNode(this);
        m_groups.push_back(group);
        qStableSort(m_groups.begin(), m_groups.end(), groupOrderLessThan);
        update();
    }

	onStructChanged();
    return r;
}

void CMindMapNode::dropNodeGroup(CMindMapNodeGroup *group)
{
    m_groups.removeAll(group);
    update();
}

void CMindMapNode::dropAllNodeGroups()
{
    m_groups.clear();
    update();
}

QList<QList<int> > CMindMapNode::nodeRegions() const
{
    int                 prevIndex = -1;
	int					startIndex = -1;
	int					endIndex = -1;
	QList<int>			region;
    QList<QList<int> >  regions;

    foreach (CMindMapNodeGroup *group, m_groups)
    {
		endIndex = group->endIndex();
        startIndex = group->startIndex();
		if (prevIndex >= startIndex)
        {
			for (int i = prevIndex + 1; i <= endIndex; ++i)
			{
				region.push_back(i);
			}
			prevIndex = qMax(prevIndex, endIndex);
        }
		else
		{
			if (prevIndex != -1)
				regions.push_back(region);

			for (int i = prevIndex + 1; i < startIndex; ++i)
			{
				region.clear();
				region.push_back(i);
				regions.push_back( region );
			}

			// add the current group into the new region
			region.clear();
			for (int i = startIndex; i <= endIndex; ++i)
			{
				region.push_back(i);
			}

			// update the previous index to be the newest group's end index
			prevIndex = endIndex;
		}
    }

	if (prevIndex != -1)
		regions.push_back(region);

	for (int i = prevIndex + 1; i < m_childs.length(); ++i)
	{
		region.clear();
		region.push_back(i);
		regions.push_back(region);
	}

    return regions;
}

QList<CMindMapNodeGroup*> CMindMapNode::groups() const
{
    return m_groups;
}

void CMindMapNode::extFromXml(const QDomElement &n)
{
    int                 groupType = -1;
    int                 spriteType = -1;
    CMindMapNodeGroup   *group = NULL;
    CMindMapNodeSprite  *sprite = NULL;
    QDomElement         t;

    if (!m_p)
        return;

    // groups
    t = n.firstChildElement("groups");
    t = t.firstChildElement("group");
    while (!t.isNull())
    {
        groupType = t.attribute("groupType").toInt();
        switch(groupType)
        {
        case CMindMapNode::Boundary:
            group = new CMindMapNodeBoundary(m_p, this);
            group->fromXml(t);
            break;

        case CMindMapNode::Summary:
            group = new CMindMapNodeSummary(m_p, this);
            group->fromXml(t);
            break;

        default:
            break;
        }; // End of switch

        m_p->insertItem(group, QPointF(0, 0));
        addNodeGroup(group);
        t = t.nextSiblingElement("group");
    }

    // sprites
    t = n.firstChildElement("sprites");
    t = t.firstChildElement("sprite");
    while (!t.isNull())
    {
        sprite = NULL;
        spriteType = t.attribute("spriteType").toInt();
        switch(spriteType)
        {
        case CMindMapNode::Icon:
            sprite = new CMindMapIconSprite(this);
            sprite->fromXml(t);
            break;

        case CMindMapNode::Pixmap:
            sprite = new CMindMapPixmapSprite(this);
            sprite->fromXml(t);
            break;

        case CMindMapNode::File:
            sprite = new CMindMapFileSprite(this);
            sprite->fromXml(t);
            break;

        case CMindMapNode::Link:
            sprite = new CMindMapLinkSprite(this);
            sprite->fromXml(t);
            break;

        case CMindMapNode::Task:
            sprite = new CMindMapTaskSprite(this);
            sprite->fromXml(t);
            break;

        case CMindMapNode::Note:
            sprite = new CMindMapNoteSprite(this);
            sprite->fromXml(t);
            break;

		case CMindMapNode::TaskPriority:
			sprite = new CMindMapTaskPrioritySprite(this);
            sprite->fromXml(t);
			break;

		case CMindMapNode::TaskProgress:
			sprite = new CMindMapTaskProgressSprite(this);
            sprite->fromXml(t);
			break;

        default:
            break;
        }; // End of switch

        if (sprite)
        {
            m_p->addItem(sprite);
            addSprite(sprite);
        }
        t = t.nextSiblingElement("sprite");
    }
}

void CMindMapNode::extToXml(QDomElement &n)
{
    /*
     * Format as follows
     * <ext>
     *      <sprites>
     *          <sprite type="1">
     *              <...>
     *          </sprite>
     *          <sprite type="2">
     *              <...>
     *          </sprite>
     *      </sprites>
     *      <groups>
     *          <group type="">
     *              <...>
     *          </group>
     *      </groups>
     * </ext>
     */
    QDomElement                 element, t;
    QDomDocument                doc;
    CMindMapNodeGroup           *group = NULL;
    CMindMapNodeSprite          *sprite = NULL;
    QList<CMindMapNodeSprite*>  sprites;

    if (!m_p)
        return;

    doc = n.ownerDocument();

    // groups
    element = doc.createElement("groups");
    foreach (group, m_groups)
    {
        t = doc.createElement( "group");
        t.setAttribute("groupType", group->groupType());
        group->toXml(t);
        element.appendChild( t );
    }
    n.appendChild(element);

    // sprites
    element = doc.createElement("sprites");
    foreach (int key, m_sprites.keys())
    {
        sprites = m_sprites[key];
        foreach (sprite, sprites)
        {
            t = doc.createElement("sprite");
            t.setAttribute("spriteType", sprite->spriteType());
            sprite->toXml(t);
            element.appendChild( t );
        }
    }
    n.appendChild(element);
}
