/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CUtils>

#include "CMindMapNode.h"
#include "CMindMapUiItem.h"
#include "CMindMapCommands.h"
#include "CMindMapNodeSprite.h"
#include "CMindMapMenuManager.h"
#include "CMindMapIconProvider.h"

CMindMapNodeSpriteSizer::CMindMapNodeSpriteSizer(CMindMapNodeSprite *sprite)
	: QGraphicsItem(sprite),
	  m_sprite(sprite),
      m_size(8),
	  m_pressed(false)
{
	setCursor( Qt::SizeFDiagCursor );

	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);
}

CMindMapNodeSpriteSizer::~CMindMapNodeSpriteSizer()
{
}

QRectF CMindMapNodeSpriteSizer::boundingRect() const
{
	return QRectF(-m_size / 2.0, -m_size/2.0, m_size, m_size);
}

QPainterPath CMindMapNodeSpriteSizer::shape() const
{
	QPainterPath		path;

	path.addRect(boundingRect());
	return path;
}

void CMindMapNodeSpriteSizer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QRectF			r;

	r = boundingRect();

	painter->save();
	if (m_pressed)
	{
        painter->setPen( QPen(Qt::blue, 3, Qt::SolidLine) );
        // painter->setBrush( QBrush(Qt::yellow) );
	}
	else
	{
        painter->setPen( QPen(Qt::darkGray, 3, Qt::SolidLine) );
        // painter->setBrush( QBrush(Qt::blue) );
	}
    painter->drawLine( r.bottomLeft(), r.bottomRight());
    painter->drawLine( r.bottomRight(), r.topRight());
    // painter->drawRect( r );
	painter->restore();
}

void CMindMapNodeSpriteSizer::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		event->ignore();
	}
	else
	{
		m_pressed = true;
		update();
		QGraphicsItem::mousePressEvent(event);
		m_sprite->startDragging(this);
	}
}

void CMindMapNodeSpriteSizer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{	
	if (m_pressed)
	{
		setPos( mapToParent(event->pos()) );
		m_sprite->doDragging(this);
	}
	else
	{
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void CMindMapNodeSpriteSizer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() != Qt::LeftButton || !m_pressed )
	{
		event->ignore();
		return;
	}
	else
	{
		m_pressed = false;
		update();
		m_sprite->doDragging(this);
		m_sprite->finishDragging(this);
		QGraphicsItem::mouseReleaseEvent(event);	
	}
}

CMindMapNodeSprite::CMindMapNodeSprite(CMindMapNode *node, int order)
    : QGraphicsObject(node),
      m_node(node),
      m_order(order),
      m_width(32),
      m_height(32),
      m_hovered(false),
      m_spriteId(0)
{
    setParentItem( dynamic_cast<QGraphicsItem*>(node) );
}

CMindMapNodeSprite::~CMindMapNodeSprite()
{

}

QRectF CMindMapNodeSprite::contentRect() const
{
    return QRectF(-m_width / 2.0, -m_height / 2.0, m_width, m_height);
}

QRectF CMindMapNodeSprite::boundingRect() const
{
    qreal       w, h;

    w = m_width;
    h = m_height;

    return QRectF(-w / 2.0, -h / 2.0, w, h);
}

QPainterPath CMindMapNodeSprite::shape() const
{
    QPainterPath    path;

    path.addRect( boundingRect() );
    return path;
}

void CMindMapNodeSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void CMindMapNodeSprite::setOrder(int value)
{
    if (value < 0)
        return;

    m_order = value;
}

int CMindMapNodeSprite::order() const
{
    return m_order;
}

void CMindMapNodeSprite::setSize(const QSizeF &value)
{
    m_width = value.width();
    m_height = value.height();
	layoutContent();
    update();
}

QSizeF CMindMapNodeSprite::size() const
{
    return QSizeF(m_width, m_height);
}

void CMindMapNodeSprite::setSpriteId(int value)
{
    m_spriteId = value;
}

int CMindMapNodeSprite::spriteId()
{
    return m_spriteId;
}

void CMindMapNodeSprite::fromXml(const QDomElement &n)
{
    QDomElement         e;
    QObject             *obj = NULL;
    QString             name, type;
    QMetaObject         *meta = NULL;
    QMetaProperty       pro;

    obj = dynamic_cast<QObject*>(this);
    e = n.firstChildElement( QString("property") );
    while (!e.isNull())
    {
        QByteArray	b;

        name = e.attribute( QString("name") );
        type = e.attribute( QString("type") );

        b = QByteArray::fromBase64( e.text().toAscii() );
        QDataStream s(&b, QIODevice::ReadOnly);
        QVariant	value(s);
        if (value.convert( QVariant::nameToType( qPrintable(type) ) ) )
        {
            meta = const_cast<QMetaObject*>( obj->metaObject() );
            pro = meta->property( meta->indexOfProperty(qPrintable(name)) );
            if (pro.isWritable())
                obj->setProperty(qPrintable(name), value);
        }
        e = e.nextSiblingElement( QString("property") );
    }
}

void CMindMapNodeSprite::toXml(QDomElement &n)
{
    QDomDocument        doc;
    QMetaObject         *meta = NULL;
    QMetaProperty       prop;
    QStringList         filters;

    filters = filterProperties();

    doc = n.ownerDocument();
    meta = const_cast<QMetaObject*>(metaObject());

    for (int i = 0; i < meta->propertyCount(); ++i)
    {
        QString		propName;
        QByteArray	b;
        QDataStream s(&b, QIODevice::WriteOnly);

        prop = meta->property(i);
        propName = QString(prop.name());
        if (!filters.contains(propName, Qt::CaseInsensitive) || prop.userType() == 0)
        {
            continue;
        }

        if (prop.isValid() && prop.isReadable())
        {
            s <<  prop.read(this);
            QDomElement	e = doc.createElement(QString("property"));
            e.setAttribute( QString("name"), QString(prop.name()) );
            e.setAttribute( QString("type"), QString(prop.typeName()) );
            e.appendChild( doc.createTextNode( QString(b.toBase64() ) ) );
            n.appendChild(e);
        }
    }
}

int CMindMapNodeSprite::spriteType() const
{
    return CMindMapNode::Undefined;
}

void CMindMapNodeSprite::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
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

void CMindMapNodeSprite::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;
    update();
}

void CMindMapNodeSprite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = false;
    update();
}

void CMindMapNodeSprite::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        triggered();
    }
    else
    {
        QGraphicsObject::mousePressEvent(event);
    }
}

void CMindMapNodeSprite::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem   *item = NULL;
    QGraphicsScene  *sc = NULL;

    if (event->buttons() & Qt::LeftButton)
    {
        sc = scene();
        if (sc && m_node)
        {
            item = (QGraphicsItem*)m_node;
            if (item)
                sc->sendEvent(item, event);
        }
    }
}

QMenu* CMindMapNodeSprite::contextMenu()
{
    // EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
    return NULL;
}

void CMindMapNodeSprite::triggered()
{
    // EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
}

CMindMapNode* CMindMapNodeSprite::node()
{
    return m_node;
}

void CMindMapNodeSprite::layoutContent()
{
	// EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
}

QStringList CMindMapNodeSprite::filterProperties()
{
    QStringList     filters;

    filters << "order"
            << "spriteType"
            << "size";

    return filters;
}

void CMindMapNodeSprite::startDragging(CMindMapNodeSpriteSizer *sizer)
{
	// EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
}

void CMindMapNodeSprite::doDragging(CMindMapNodeSpriteSizer *sizer)
{
	// EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
}

void CMindMapNodeSprite::finishDragging(CMindMapNodeSpriteSizer *sizer)
{
	// EMPTY, TO BE IMPLEMENTED IN SUBCLASSES
}

CMindMapPixmapSprite::CMindMapPixmapSprite(CMindMapNode *node)
    : CMindMapNodeSprite(node),
      m_embeded(true),
	  m_defaultSize(128),
	  m_sizer(NULL),
	  m_minSize(8)
{
    setOrder(0);
	setFlag(ItemIsSelectable, true);

	m_sizer = new CMindMapNodeSpriteSizer(this);
	m_sizer->setVisible(false);
    setSize( QSizeF(m_defaultSize, m_defaultSize) );
}

CMindMapPixmapSprite::~CMindMapPixmapSprite()
{

}

void CMindMapPixmapSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF      r;

    r = contentRect();

    painter->save();
	painter->setRenderHint( QPainter::Antialiasing, true );
    if (qFuzzyCompare(m_width / m_height, m_defaultRatio))
        QIcon(m_pixmap).paint(painter, r.toRect(), Qt::AlignCenter);
    else
        painter->drawPixmap(r, m_pixmap, QRectF());

    if (isSelected())
    {
        painter->setPen( QPen(Qt::blue, 2, Qt::SolidLine ) );
        painter->setBrush( Qt::NoBrush );
        painter->drawRect( boundingRect() );
    }

    painter->restore();
}

void CMindMapPixmapSprite::setPixmapPath(const QString &value)
{
	QFileInfo		info(value);

	if (info.isFile() && info.exists())
	{
		m_pixmap.load(value);
		updateInfo();
	}
}

void CMindMapPixmapSprite::setPixmap(const QPixmap &value)
{
	m_pixmap = value;
	updateInfo();
}

void CMindMapPixmapSprite::updateInfo()
{
    qreal		delta;
	qreal		w, h;
	QSize		s;
    QSizeF      originSize;

    originSize = size();
    s = m_pixmap.size();

    if ( !qFuzzyCompare( originSize.width(), m_defaultSize ) ||
         !qFuzzyCompare( originSize.height(), m_defaultSize) )
    {
        w = originSize.width();
        h = originSize.height();
        m_defaultRatio = w / h;
    }
    else
    {
        w = s.width();
        h = s.height();
        if (w > 0 && h > 0)
        {
            delta = w / h;
            if (w > h)
            {
                w = qMin(w, m_defaultSize);
                h = w / delta;
            }
            else
            {
                h = qMin(h, m_defaultSize);
                w = h * delta;
            }
            m_defaultRatio = delta;
        }

        setSize( QSizeF(w, h) );
    }
}

const QPixmap& CMindMapPixmapSprite::pixmap()
{
    return m_pixmap;
}

void CMindMapPixmapSprite::setEmbeded(bool value)
{
    m_embeded = value;
}

bool CMindMapPixmapSprite::embeded() const
{
    return m_embeded;
}

void CMindMapPixmapSprite::restore()
{
    QSize       s;

    s = m_pixmap.size();
    setSize( QSizeF(s.width(), s.height()) );
}

QMenu* CMindMapPixmapSprite::contextMenu()
{
    CMindMapNodeSpriteMenu	*menu = NULL;

    menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::Pixmap );
    if (menu)
        menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

void CMindMapPixmapSprite::triggered()
{
   m_sizer->setVisible(true);
   if (m_node)
	   m_node->setSelected(true);
}

int CMindMapPixmapSprite::spriteType() const
{
    return CMindMapNode::Pixmap;
}

QStringList CMindMapPixmapSprite::filterProperties()
{
    QStringList     filters;

	filters = CMindMapNodeSprite::filterProperties();
    filters.insert(0, "embeded");
    filters.insert(0, "pixmap");

    return filters;
}

void CMindMapPixmapSprite::startDragging(CMindMapNodeSpriteSizer *sizer)
{
	// NOTHING
	m_oldSize = size();
}

void CMindMapPixmapSprite::doDragging(CMindMapNodeSpriteSizer *sizer)
{
	qreal			w = 0, h = 0;
	QPointF			p;
	QRectF			r;

	r = boundingRect();
	p = sizer->pos();
	w = qAbs(p.x() - r.left());
	h = qAbs(p.y() - r.top());

	if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
	{
		w = ( w == 0 ? m_minSize : w);
		h = ( h == 0 ? m_minSize : h);
	}
	else
	{
		h = ( h == 0 ? m_minSize : h);
		w = h * m_defaultRatio;
	}

	setSize( QSizeF(w, h) );
}

void CMindMapPixmapSprite::finishDragging(CMindMapNodeSpriteSizer *sizer)
{
    Q_UNUSED(sizer);

	CDiagram		*d = NULL;

	d = dynamic_cast<CDiagram*>(scene());
	if (d)
	{
        d->addUndoCommand( new CMindMapNodeSpriteResizeCommand(d, this, m_oldSize, size()) );
	}
}

void CMindMapPixmapSprite::layoutContent()
{
	QRectF		r;

	r = boundingRect();
	m_sizer->setPos( r.bottomRight() );
	m_node->onStructChanged();
	update();
}

QVariant CMindMapPixmapSprite::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		if (m_sizer)
			m_sizer->setVisible(value.toBool());
		return value;
	}
	else
	{
		return QGraphicsItem::itemChange(change, value);
	}
}

CMindMapIconSprite::CMindMapIconSprite(CMindMapNode *node,
                                       QIcon icon,
                                       int index)
    : CMindMapNodeSprite(node),
      m_icon(icon),
      m_iconIndex(index)
{
    QSize       s(16, 16);

    m_icon.actualSize(s);
    if (m_icon.isNull() || s.width() < 0)
        setSize( QSizeF(16, 16) );
    else
        setSize( QSizeF(s.width(), s.height()));

    setAcceptHoverEvents(true);
    setOrder(0);
    setCursor( Qt::PointingHandCursor );
}

CMindMapIconSprite::~CMindMapIconSprite()
{

}

void CMindMapIconSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF      r;

    r = contentRect();

    painter->save();
    m_icon.paint(painter, r.toRect(), Qt::AlignCenter);
    if (m_hovered)
    {
        painter->setBrush( QColor(0, 0, 0, 64) );
        painter->setPen( Qt::lightGray );
        painter->drawRoundedRect(r, 2, 2 );
    }

    painter->restore();
}

void CMindMapIconSprite::setIcon(const QIcon &value)
{
    QSize       s;

    m_icon = value;
    m_icon.actualSize(s);
    if (s.width() > 0)
        setSize( QSizeF(s.width(), s.height()) );
    else
        setSize( QSizeF(16, 16) );
    update();
}

QIcon CMindMapIconSprite::icon() const
{
    return m_icon;
}

void CMindMapIconSprite::setIconIndex(int value)
{
    m_iconIndex = value;
}

int CMindMapIconSprite::iconIndex() const
{
    return m_iconIndex;
}

QMenu* CMindMapIconSprite::contextMenu()
{
    CMindMapNodeSpriteMenu	*menu = NULL;

	menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::Icon );
	if (menu)
		menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

void CMindMapIconSprite::triggered()
{
    int     index, recIdx;
    QIcon   icon;

    index = iconIndex();
    recIdx = index;

    // we pass in the current index of the icon,
    // the value will be changed to the index of the next alternative icon
    // if there is any.
    icon = CMindMapIconProvider::instance()->getNextAlterIcon(index);
    if (!icon.isNull() && index != recIdx)
    {
        QSize   s;

        icon.actualSize(s);
        setIcon(icon);
        setIconIndex(index);
    }
}

int CMindMapIconSprite::spriteType() const
{
    return CMindMapNode::Icon;
}

QStringList CMindMapIconSprite::filterProperties()
{
    QStringList     filters;

	filters = CMindMapNodeSprite::filterProperties();
    filters << "icon"
            << "iconIndex";

    return filters;
}

CMindMapLinkSprite::CMindMapLinkSprite(CMindMapNode *node, QUrl link, int type)
    : CMindMapIconSprite(node),
      m_link(link),
      m_linkType(type)
{
    setAcceptHoverEvents(true);
    setIcon( QIcon(":/mindstorm/icons/ui-navigation.png") );
    setOrder(0);
}

CMindMapLinkSprite::~CMindMapLinkSprite()
{

}

void CMindMapLinkSprite::setLink(QUrl value)
{
    m_link = value;
    update();
}

QUrl CMindMapLinkSprite::link() const
{
    return m_link;
}

void CMindMapLinkSprite::setLinkType(int value)
{
    m_linkType = value;
    update();
}

int CMindMapLinkSprite::linkType() const
{
    return m_linkType;
}

QMenu* CMindMapLinkSprite::contextMenu()
{
    CMindMapNodeSpriteMenu	*menu = NULL;

	menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::Link );
	if (menu)
		menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

void CMindMapLinkSprite::triggered()
{
    switch(m_linkType)
    {
    case CMindMapLinkSprite::Web:
    case CMindMapLinkSprite::Email:
	case CMindMapLinkSprite::Topic:
	case CMindMapLinkSprite::SubGraph:
        QDesktopServices::openUrl( m_link );
        break;

    default:
        break;

    }; // End of switch
}

int CMindMapLinkSprite::spriteType() const
{
    return CMindMapNode::Link;
}

void CMindMapLinkSprite::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QString                 lineBreak("\n");
    QStringList             lines;
    QMap<int, QString>      titles;

    CMindMapNodeSprite::hoverEnterEvent(event);

    titles[CMindMapLinkSprite::Web] = tr("Web Url");
    titles[CMindMapLinkSprite::Email] = tr("Email");
    titles[CMindMapLinkSprite::Topic] = tr("Link to Topic");
    titles[CMindMapLinkSprite::SubGraph] = tr("Link to Sheet");

    lines << titles[m_linkType]
          << m_link.toString();

    QToolTip::showText(event->screenPos(), lines.join( lineBreak ), NULL, QRect() );
}

void CMindMapLinkSprite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    CMindMapNodeSprite::hoverLeaveEvent(event);
}

QStringList CMindMapLinkSprite::filterProperties()
{
    QStringList     filters;

    filters = CMindMapNodeSprite::filterProperties();
    filters << "link"
            << "linkType";

    return filters;
}

CMindMapFileSprite::CMindMapFileSprite(CMindMapNode *node)
    : CMindMapIconSprite(node)
{
    setAcceptHoverEvents(true);
    setIcon( QIcon(":/mindstorm/icons/ui-attach.png") );
    setOrder(0);
}

CMindMapFileSprite::~CMindMapFileSprite()
{

}

bool CMindMapFileSprite::addFile(const QString &value, bool embeded)
{
	int				fileType;
	bool			r = false;
	QUrl			url;
	QFileInfo		info(value);

	if (m_files.contains(value))
		return r;

	if (info.isFile() && info.exists())
    {
		fileType = CMindMapFileSprite::LocalFile;
		r = true;
    }
    else
    {
        url.setUrl(value);
        if (url.isValid() && !url.isLocalFile())
        {
			fileType = CMindMapFileSprite::RemoteFile;
			r = true;
        }
		else
		{
			return r;
		}
    }

	m_files.push_back(value);
	m_fileTypes.push_back(fileType);
	m_embedMarkers.push_back(embeded);

	return r;
}

bool CMindMapFileSprite::deleteFile(const QString &value)
{
	int		index;

	if (!m_files.contains(value))
		return false;
	
	index = m_files.indexOf(value);
	return deleteFile(index);
}

bool CMindMapFileSprite::deleteFile(int index)
{
	if (index >= 0 && index < m_files.length())
	{
		m_files.removeAt(index);
		m_fileTypes.removeAt(index);
		m_embedMarkers.removeAt(index);

		return true;
	}

	return false;
}

void CMindMapFileSprite::setFiles(const QStringList &value)
{
	m_files = value;
}

QStringList CMindMapFileSprite::files() const
{
	return m_files;
}

void CMindMapFileSprite::setFileEmbedMarkers(const QStringList &value)
{
    m_embedMarkers.clear();

    foreach (QString marker, value)
    {
        m_embedMarkers.push_back( Utils::stringToBool(marker) );
    }
}

QStringList CMindMapFileSprite::fileEmbedMarkers() const
{
    QStringList     markers;

    foreach (bool r, m_embedMarkers)
    {
        markers.push_back( Utils::boolToString(r) );
    }

    return markers;
}

void CMindMapFileSprite::setFileTypes(const QStringList &value)
{
    m_fileTypes.clear();

    foreach (QString type, value)
    {
        m_fileTypes.push_back( type.toInt() );
    }
}

QStringList CMindMapFileSprite::fileTypes() const
{
    QStringList     types;

    foreach (int type, m_fileTypes)
    {
        types.push_back( QString::number(type) );
    }

    return types;
}

QMenu* CMindMapFileSprite::contextMenu()
{
    CMindMapNodeSpriteMenu	*menu = NULL;

    menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::File );
    if (menu)
        menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

void CMindMapFileSprite::triggered()
{
    QPointF         p;
    QRectF          r;
    QGraphicsView   *v = NULL;
    QGraphicsScene  *sc = NULL;
    QMenu           *menu = NULL;

    menu = contextMenu();
    sc = scene();
    if (!sc || !menu)
        return;

    v = sc->views().first();
    if (!v)
        return;

    r = boundingRect();
    p = m_node->mapToScene( r.bottomLeft() );

    menu->exec( v->mapToGlobal( v->mapFromScene(p) ) );
}

int CMindMapFileSprite::spriteType() const
{
    return CMindMapNode::File;
}

void CMindMapFileSprite::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    int             index = 0;
    QString         tpl(tr("[File %1] %2"));
    QString         lineBreak("\n");
    QStringList     files;

    CMindMapNodeSprite::hoverEnterEvent(event);

    files << tr("Attached Files:");
    foreach (QString file, m_files)
    {
        files << tpl.arg(QString::number(index)).arg( QFileInfo(file).fileName() );
        ++ index;
    }

    QToolTip::showText(event->screenPos(), files.join( lineBreak ), NULL, QRect() );
}

void CMindMapFileSprite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    CMindMapNodeSprite::hoverLeaveEvent(event);
}

QStringList CMindMapFileSprite::filterProperties()
{
    QStringList     filters;

    filters = CMindMapNodeSprite::filterProperties();
    filters << "files"
            << "fileTypes"
            << "fileEmbedMarkers";

    return filters;
}

CMindMapNoteSprite::CMindMapNoteSprite(CMindMapNode *node)
    : CMindMapIconSprite(node)
{
    setAcceptHoverEvents(true);
    setIcon( QIcon(":/mindstorm/icons/ui-note.png") );
    setOrder(0);
}

CMindMapNoteSprite::~CMindMapNoteSprite()
{

}

void CMindMapNoteSprite::setNote(const QString &value)
{
    m_note = value;
}

QString CMindMapNoteSprite::note() const
{
    return m_note;
}

QMenu* CMindMapNoteSprite::contextMenu()
{
	CMindMapNodeSpriteMenu	*menu = NULL;

	menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::Note );
	if (menu)
		menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

void CMindMapNoteSprite::triggered()
{
    QPointF         p;
    QRectF          r;
    QGraphicsView   *v = NULL;
    QGraphicsScene  *sc = NULL;
    QMenu           *menu = NULL;

    menu = contextMenu();
    sc = scene();
    if (!sc || !menu)
        return;

    v = sc->views().first();
    if (!v)
        return;

    r = boundingRect();
    p = m_node->mapToScene( r.bottomLeft() );

    menu->exec( v->mapToGlobal( v->mapFromScene(p) ) );
}

int CMindMapNoteSprite::spriteType() const
{
    return CMindMapNode::Note;
}

void CMindMapNoteSprite::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    CMindMapNodeSprite::hoverEnterEvent(event);
    QToolTip::showText(event->screenPos(), note(), NULL, QRect());
}

void CMindMapNoteSprite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    CMindMapNodeSprite::hoverLeaveEvent(event);
}

QStringList CMindMapNoteSprite::filterProperties()
{
    QStringList     filters;

    filters = CMindMapNodeSprite::filterProperties();
    filters << "note";

    return filters;
}

CMindMapTaskSprite::CMindMapTaskSprite(CMindMapNode *node)
    : CMindMapNodeSprite(node),
      m_useTime(false)
{
    m_font = QFont("Arial", 9);
}

CMindMapTaskSprite::~CMindMapTaskSprite()
{

}

void CMindMapTaskSprite::setAssignedTo(const QString &value)
{
    m_assignedTo = value.trimmed();
    setSize( preferredSize() );
    update();
}

QString CMindMapTaskSprite::assignedTo()
{
    return m_assignedTo;
}

void CMindMapTaskSprite::setDueDate(const QDateTime &date)
{
    m_dueDate = date;
    setSize( preferredSize() );
    update();
}

QDateTime CMindMapTaskSprite::dueDate() const
{
    return m_dueDate;
}

void CMindMapTaskSprite::setUseTime(bool value)
{
    m_useTime = value;
}

bool CMindMapTaskSprite::useTime()
{
    return m_useTime;
}

void CMindMapTaskSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QIcon           assigneeIcon(":/res/icons/node-people.png");
    QIcon           dueDateIcon(":/res/icons/node-clock.png");
    QRectF          r, iconRect, textRect;
    QSizeF          upNodeSize;
    qreal           iconSize = 16, spacing = 4;
    QString         dueDateStr;
    QFontMetrics    metrics( m_font );

    upNodeSize = m_node->contentSize();
    r = boundingRect();
    textRect.setTopRight( r.topLeft() );

    painter->save();
    painter->setPen( QPen(Qt::darkGray, 1, Qt::SolidLine ));
    painter->drawLine( QPointF(-qMin(upNodeSize.width(), m_node->contentRect().width()) / 2.0, r.top()),
                       QPointF(qMin(upNodeSize.width(), m_node->contentRect().width()) / 2.0, r.top()) );

    painter->setFont( m_font );

    if (!m_assignedTo.trimmed().isEmpty())
    {
        iconRect = QRectF(r.topLeft() + QPointF(spacing, 0),
                          QSizeF(iconSize, r.height()));
        assigneeIcon.paint(painter, iconRect.toRect(), Qt::AlignCenter);
        textRect = QRectF(iconRect.topRight() + QPointF(spacing, 0),
                          QSizeF(metrics.width(m_assignedTo), r.height()));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_assignedTo);
    }

    if (!m_dueDate.isNull())
    {
        dueDateStr = dueDateString();
        iconRect = QRectF(textRect.topRight() + QPointF(spacing, 0),
                          QSizeF(iconSize, r.height()));
        dueDateIcon.paint(painter, iconRect.toRect(), Qt::AlignCenter);

        textRect = QRectF(iconRect.topRight() + QPointF(spacing, 0),
                          QSizeF(metrics.width(dueDateStr), r.height()));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dueDateStr);
    }

    painter->restore();
}

bool CMindMapTaskSprite::isNull() const
{
    return ( m_assignedTo.trimmed().isEmpty() && m_dueDate.isNull());
}

int CMindMapTaskSprite::spriteType() const
{
    return CMindMapNode::Task;
}

QSizeF CMindMapTaskSprite::preferredSize()
{
    qreal           iconSize = 16;
    qreal           spacing = 4, padding = 4, w = 0;
    QFontMetrics    metrics(m_font);

    if (!m_assignedTo.trimmed().isEmpty())
        w += ( iconSize + spacing + metrics.width( m_assignedTo ) );
    if (!m_dueDate.isNull())
        w += ( iconSize + spacing + metrics.width( dueDateString() ) );

    return QSizeF( w + 2 * padding,
                   qMax(iconSize + 0.0, metrics.height() + 0.0) + 2 * padding);
}

QString CMindMapTaskSprite::dueDateString()
{
    QString         r;

    if (!m_dueDate.isNull())
    {
        if (!m_useTime)
            r = m_dueDate.toString("MM/dd/yyyy");
        else
            r = m_dueDate.toString("MM/dd/yyyy h:m a");
    }

    return r;
}

QStringList CMindMapTaskSprite::filterProperties()
{
    QStringList     filters;

    filters = CMindMapNodeSprite::filterProperties();
    filters << "assignedTo"
            << "dueDate";

    return filters;
}

void CMindMapTaskSprite::triggered()
{
	if (m_node)
	   m_node->setSelected(true);
}

CMindMapTaskPrioritySprite::CMindMapTaskPrioritySprite(CMindMapNode *node, int priority)
	: CMindMapIconSprite(node),
	  m_priority(priority),
	  m_maxPriority(7)
{
	m_icon = QIcon( QString(":/mindstorm/icons/ui-number-%1.png").arg( QString::number(m_priority) ) );
	setOrder(1);
}

CMindMapTaskPrioritySprite::~CMindMapTaskPrioritySprite()
{
}

void CMindMapTaskPrioritySprite::setPriority(int value)
{
	if (value >= 0 && value < m_maxPriority)
	{
		m_priority = value;
		m_icon = QIcon( QString(":/mindstorm/icons/ui-number-%1.png").arg( QString::number(m_priority) ) );
	}
	update();
}

int CMindMapTaskPrioritySprite::priority()
{
	return m_priority;
}

int CMindMapTaskPrioritySprite::spriteType() const
{
	return CMindMapNode::TaskPriority;
}

QStringList CMindMapTaskPrioritySprite::filterProperties()
{
	QStringList			filters;

	filters = CMindMapNodeSprite::filterProperties();
    filters << "icon"
			<< "priority";

	return filters;
}

void CMindMapTaskPrioritySprite::triggered()
{
	int		r = 0;

	r = (m_priority + 1) % m_maxPriority;
	setPriority(r);
}

QMenu* CMindMapTaskPrioritySprite::contextMenu()
{
	CMindMapNodeSpriteMenu	*menu = NULL;

	menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::TaskPriority );
	if (menu)
		menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}

CMindMapTaskProgressSprite::CMindMapTaskProgressSprite(CMindMapNode *node, int progress)
	: CMindMapIconSprite(node),
	  m_progress(progress),
	  m_maxProgressNo(5)
{
	m_icon = QIcon( QString(":/mindstorm/icons/ui-progress-%1.png").arg( QString::number(m_progress) ) );
	setOrder(2);
}

CMindMapTaskProgressSprite::~CMindMapTaskProgressSprite()
{
}

void CMindMapTaskProgressSprite::setProgress(int value)
{
	if (value >= 0 && value < m_maxProgressNo)
	{
		m_progress = value;
		m_icon = QIcon( QString(":/mindstorm/icons/ui-progress-%1.png").arg( QString::number(m_progress) ) );
	}
	update();
}

int CMindMapTaskProgressSprite::progress()
{
	return m_progress;
}

int CMindMapTaskProgressSprite::spriteType() const
{
	return CMindMapNode::TaskProgress;
}


QStringList CMindMapTaskProgressSprite::filterProperties()
{
	QStringList			filters;

	filters = CMindMapNodeSprite::filterProperties();
    filters << "icon"
			<< "progress";

	return filters;
}

void CMindMapTaskProgressSprite::triggered()
{
	 int		r = 0;

	r = (m_progress + 1) % m_maxProgressNo;
	setProgress(r);
}

QMenu* CMindMapTaskProgressSprite::contextMenu()
{
	CMindMapNodeSpriteMenu	*menu = NULL;

	menu = CMindMapMenuManager::instance()->nodeSpriteMenu( CMindMapNode::TaskProgress );
	if (menu)
		menu->reset(this);

    return dynamic_cast<QMenu*>( menu );
}
