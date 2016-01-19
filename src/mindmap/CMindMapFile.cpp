/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>

#include "CMindMapNode.h"
#include "CMindMapFile.h"
#include "CMindMapSettings.h"

CMindMapGraph::CMindMapGraph()
	: CDiagram(),
	  m_id(-1),
      m_level(0),
      m_viewCenter( QPointF(0, 0) ),
      m_parentGraphId(-1),
      m_parentGraph(NULL),
      m_file(NULL),
      m_itemModel(NULL),
      m_title(tr("Sheet")),
      m_hidden(false)
{
	m_itemModel = new QStandardItemModel(this);
	setAutoAlignment(false);

    connect(this, SIGNAL(itemsAdded(CDiagramItem*)), this, SLOT(onItemAdded(CDiagramItem*)));
}

CMindMapGraph::CMindMapGraph(CMindMapFile *file,  CMindMapGraph *parentGraph)
    : CDiagram(),
      m_id(-1),
      m_level(0),
      m_viewCenter( QPointF(0, 0) ),
      m_parentGraphId(-1),
      m_parentGraph(parentGraph),
      m_file(file),
      m_itemModel(NULL),
      m_title(tr("Sheet")),
      m_hidden(false)
{
	if (m_parentGraph)
	{
		m_parentGraphId = m_parentGraph->id();
	}

    m_itemModel = new QStandardItemModel(this);
	setAutoAlignment(false);

    connect(this, SIGNAL(itemsAdded(CDiagramItem*)), this, SLOT(onItemAdded(CDiagramItem*)));
}

CMindMapGraph::~CMindMapGraph()
{
}

int CMindMapGraph::id()
{
    return m_id;
}

void CMindMapGraph::setId(int id)
{
    m_id = id;
    if (m_file)
        m_file->onGraphChanged();
}

int CMindMapGraph::level()
{
	return m_level;
}

void CMindMapGraph::setLevel(int value)
{
    m_level = value;
    if (m_file)
        m_file->onGraphChanged();
}

void CMindMapGraph::setViewCenter(const QPointF &value)
{
    m_viewCenter = value;
    if (m_file)
        m_file->onGraphChanged();
}

QPointF CMindMapGraph::viewCenter()
{
    return m_viewCenter;
}

void CMindMapGraph::setSize(qreal w, qreal h)
{
    setSize(QSizeF(w, h));
}

void CMindMapGraph::setSize(QSizeF size)
{
    m_size = size;
    if (m_file)
        m_file->onGraphChanged();
}

QSizeF CMindMapGraph::size() const
{
    return m_size;
}

void CMindMapGraph::setTitle(const QString &value)
{
	m_title = value;
    if (m_file)
        m_file->onGraphChanged();
}

QString CMindMapGraph::title() const
{
	return m_title;
}

void CMindMapGraph::setHidden(bool value)
{
    m_hidden = value;
}

bool CMindMapGraph::hidden()
{
    return m_hidden;
}

void CMindMapGraph::setBackgroundTexture(const QImage &value)
{
    QBrush      brush;

    if (!value.isNull())
    {
        brush = QBrush(value);
        setBackground(brush);
    }
}

QImage CMindMapGraph::backgroundTexture()
{
    QBrush      brush;

    brush = background();
    return brush.textureImage();
}

void CMindMapGraph::setFile(CMindMapFile *file)
{
	m_file = file;
}

CMindMapFile* CMindMapGraph::file()
{
	return m_file;
}

void CMindMapGraph::setParentGraphId(int id)
{
    m_parentGraphId = id;
	if (m_file)
	{
		setParentGraph(m_file->getGraphById(id));
	}
}

void CMindMapGraph::setParentGraph(CMindMapGraph *graph)
{
    m_parentGraph = graph;
    if (graph)
        m_parentGraphId = graph->id();
    else
        m_parentGraphId = -1;
}

int CMindMapGraph::parentGraphId() const
{
    return m_parentGraphId;
}

void CMindMapGraph::setSubGraphIds(QStringList ids)
{
    m_subGraphIds.clear();
    foreach (QString id, ids)
    {
        m_subGraphIds.push_back( id.toInt() );
    }
}

QStringList CMindMapGraph::subGraphIds() const
{
    QStringList     ids;

    foreach (int id, m_subGraphIds)
    {
        ids.push_back( QString::number(id) );
    }

    return ids;
}

CMindMapGraph* CMindMapGraph::parentGraph()
{
    if (!m_file)
        return NULL;

    if (!m_parentGraph && m_parentGraphId > 0)
    {
        m_parentGraph = m_file->getGraphById(m_parentGraphId);
    }

    return m_parentGraph;
}

CMindMapGraph *CMindMapGraph::createSubGraph()
{
    int                 id = -1;
    CMindMapGraph       *graph = NULL;

    if (!m_file)
        return NULL;

    graph = m_file->createSubGraph(this);
    if (graph)
    {
        id = graph->id();
        if (!m_subGraphIds.contains(id))
        {
            m_subGraphes.push_back(graph);
            m_subGraphIds.push_back(id);
            m_file->addSubGraph(graph);
        }
    }
    return graph;
}

void CMindMapGraph::removeSubGraph(CMindMapGraph *graph)
{
    int         id = -1;

    if (!graph || !m_file)
        return;

    id = graph->id();
    if (m_subGraphes.contains(graph))
    {
        m_subGraphIds.removeAll(id);
        m_subGraphes.removeAll(graph);
        m_file->removeSubGraph(graph);
    }
    graph->deleteLater();

    if (m_file)
        m_file->onGraphChanged();
}

QList<CMindMapGraph*> CMindMapGraph::subGraphes()
{
    CMindMapGraph   *graph = NULL;

    if (!m_file)
        return QList<CMindMapGraph*>();

    if (m_subGraphes.isEmpty())
    {
        foreach (int id, m_subGraphIds)
        {
            graph = m_file->getGraphById(id);
            if (graph)
                m_subGraphes.push_back(graph);
        }
    }
	return m_subGraphes;
}

QList<CMindMapNode*> CMindMapGraph::getAllSaveNodes()
{
    CMindMapNode            *node = NULL;
    QList<CMindMapNode*>    nodes;
    QList<CDiagramItem*>    items;

    items = topLevelItems();
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

void CMindMapGraph::filterNodes(const QString &filter)
{
    int                         filterType = 0;
    bool                        filterCaseSensitive = false;
	CMindMapNode				*node = NULL;
    CMindMapSetting             *setting = NULL;
	QStandardItem				*item = NULL;
    QList<QStandardItem*>		filteredItems;

    setting = CMindMapSetting::instance();
    if (!setting)
        return;

    clearSelection();
	if (m_itemModel)
	{
        filterType = setting->get(Settings::MindMapFilterType, QVariant(Qt::MatchContains)).toInt();
        filterCaseSensitive = setting->get(Settings::MindMapFilterCaseSensitive, QVariant(false)).toBool();
        if (filterCaseSensitive)
            filteredItems = m_itemModel->findItems(filter, ((Qt::MatchFlags)filterType) | Qt::MatchCaseSensitive, 0);
        else
            filteredItems = m_itemModel->findItems(filter, ((Qt::MatchFlags)filterType) , 0);

		// shows the filtered nodes
		foreach (item, filteredItems)
		{
            // extract node id
            node = dynamic_cast<CMindMapNode*>( getItemById(item->data(Qt::UserRole + 2).toInt()) );
			if (node)
			{
				node->setSelected(true);
			}
		}
	}
}

void CMindMapGraph::locateNode(int id)
{
	QPointF			p;
	CMindMapNode	*node = NULL;
	QGraphicsView	*view = NULL;

	node = dynamic_cast<CMindMapNode*>( getItemById(id) );
	if (node)
	{
		clearSelection();
		node->setSelected(true);
		p = node->mapToScene( node->boundingRect().center() );

		view = getMainView();
		if (view)
		{
			view->centerOn(p);
		}
	}
}

QAbstractItemModel* CMindMapGraph::dataModel(bool tree)
{
	CMindMapNode				*node = NULL;
	QList<CMindMapNode*>		nodes;
	QStandardItem				*item = NULL;
	

	m_itemModel->clear();
	nodes = getAllSaveNodes();

	if (tree)
	{
		int							level = 0;
		QStack<CMindMapNode*>		stack;
		QVector<QStandardItem*>		parents(10);

		foreach (node, nodes)
		{
			if (node->isRoot())
			{
				break;
			}
		}

		if (!node)
			return m_itemModel;

		parents[0] = m_itemModel->invisibleRootItem();
		stack.push(node);
		while (!stack.isEmpty())
		{
			node = stack.pop();
			item = new QStandardItem();
			item->setText( node->text() );
            item->setData(QVariant(id()), Qt::UserRole + 1);
            item->setData(QVariant(node->id()), Qt::UserRole + 2);
			
			level = node->level();
			if (level >= parents.size() - 1)
				parents.resize( parents.size() * 2 );
			parents[level]->appendRow(item);
			parents[level+1] = item;

			// re-fill the nodes to the stack
			nodes = node->childs();
			foreach (node, nodes)
			{
				stack.push( node );
			}
		}
	}
	else
	{
		foreach (node, nodes)
		{
			item = new QStandardItem();
			item->setText( node->text() );
            item->setData(QVariant(id()), Qt::UserRole + 1);
            item->setData(QVariant(node->id()), Qt::UserRole + 2);
			m_itemModel->appendRow(item);
		}
	}

	return m_itemModel;
}

void CMindMapGraph::initDefault()
{
    CMindMapNode    *node = NULL;
    CMindMapSetting     *setting = NULL;

    node = new CMindMapNode(this);
    connect(node, SIGNAL(areaChanged(QRectF)), this, SLOT(onContentRectChanged(QRectF)));
    insertItem(node, QPointF(0, 0));

    setting = CMindMapSetting::instance();
    if (setting)
        setBackground( setting->get( Settings::MindMapBackgroundBrush, QVariant(QBrush(Qt::white))).value<QBrush>());

    if (m_file)
        m_file->onGraphChanged();
}

void CMindMapGraph::toXml(QDomElement &n)
{
    QDomDocument            doc;
    QDomElement             topicsElement, topicElement;
    QDomElement             textNodesElement, textNodeElement;
    QMetaObject             *meta = NULL;
    QMetaProperty           prop;
    QStringList             filters;
    QList<CMindMapNode*>    nodes;
    QList<CDiagramItem*>    topItems;

    filters << "palette"
            << "bspTreeDepth";

    doc = n.ownerDocument();
    meta = const_cast<QMetaObject*>(metaObject());
    for (int i = 0; i < meta->propertyCount(); ++i)
    {
        QString		propName;
        QByteArray	b;
        QDataStream s(&b, QIODevice::WriteOnly);

        prop = meta->property(i);
        propName = QString(prop.name());
        if (filters.contains(propName, Qt::CaseInsensitive) || prop.userType() == 0 )
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

    // persistent all topic nodes
    topicsElement = doc.createElement("topics");
    nodes = getAllSaveNodes();
    foreach (CMindMapNode *node, nodes)
    {
        topicElement = doc.createElement("topic");
        node->toXml(topicElement);
        topicsElement.appendChild( topicElement );
    }
    n.appendChild( topicsElement );

    // persistent all text nodes
    textNodesElement = doc.createElement("textNodes");
    topItems = topLevelItems();
    foreach (CDiagramItem *item, topItems)
    {
        CDiagramTextNode    *textNode = NULL;

        textNode = dynamic_cast<CDiagramTextNode*>(item);
        if (textNode)
        {
            textNodeElement = doc.createElement("textNode");
            textNode->toXml(textNodeElement);
            textNodesElement.appendChild( textNodeElement );
        }
    }
    n.appendChild( textNodesElement );
}

void CMindMapGraph::fromXml(QDomElement &n)
{
    CMindMapNode            *node = NULL;
    CDiagramTextNode        *textNode = NULL;
    QDomElement             e;
    QObject                 *obj = NULL;
    QString                 name, type;
    QMetaObject             *meta = NULL;
    QMetaProperty           pro;
    CMindMapSetting         *setting = NULL;

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

    // extract all nodes
    e = n.firstChildElement( "topics" );
    e = e.firstChildElement( "topic" );
    while (!e.isNull())
    {
        node = new CMindMapNode(this);
        node->fromXml( e );
        insertItem( node, node->pos(), false );
        e = e.nextSiblingElement("topic");
    }

    // extract all text
    e = n.firstChildElement("textNodes");
    e = e.firstChildElement("textNode");
    while (!e.isNull())
    {
        textNode = new CDiagramTextNode(this);
        textNode->fromXml(e);
        insertItem( textNode, textNode->pos(), false);
        e = e.nextSiblingElement("textNode");
    }
}

void CMindMapGraph::onRebuildGraph()
{
    CMindMapGraph               *graph = NULL;
    CMindMapNode                *node = NULL, *root = NULL;
    QList<CMindMapNode*>        nodes;

    if (!m_file)
        return;

    // rebuild childs
    nodes = getAllSaveNodes();
    foreach (node, nodes)
    {
        node->onRebuildChilds();
        if (node->isRoot())
            root = node;
    }

    // just connect here, then we will not cause
    // resize all the way of construction
    if (root)
        connect(root, SIGNAL(areaChanged(QRectF)), this, SLOT(onContentRectChanged(QRectF)));

    // parent graph setup
    if (m_parentGraphId >= 0)
    {
        m_parentGraph = m_file->getGraphById( m_parentGraphId );
    }

    // sub graphes setup
    foreach (int id, m_subGraphIds)
    {
        graph = m_file->getSubGraphById(id);
        if (graph)
        {
            m_subGraphes.push_back( graph );
        }
    }
}

void CMindMapGraph::onItemAdded(CDiagramItem *item)
{
	// EMPTY NOW
}

void CMindMapGraph::onContentRectChanged(QRectF rect)
{
    bool        changed = false;
    qreal       w, h;
    QRectF      sc;

    sc = sceneRect();

    w = sc.width();
    h = sc.height();
    if (sc.left() >= rect.left() || sc.right() <= rect.right())
    {
        w = qMax(1.5 * w, w + qMax(sc.left() - rect.left(), rect.right() - sc.right()) * 2);
        changed = true;
    }

    if (sc.top() >= rect.top() || sc.bottom() <= rect.bottom())
    {
        h = qMax(1.5 * h, h + qMax(sc.top() - rect.top(), rect.bottom() - sc.bottom()));
        changed = true;
    }

    if (changed)
        setSceneRect( QRectF(-w / 2.0, -h / 2.0, w, h));
}

// CMindMapFile
CMindMapFile::CMindMapFile(const QString &fileName, const QString &filePath )
    : CDiagramFile(),
      m_currentGraph(NULL),
      m_nextSubGraphId(0),
      m_nextGraphId(0)
{
	int			count = 0;
	QString		tpl("Untitle Document %1");

	count = CDiagramFileManager::instance()->fileCount();

    m_useCompress = false;
	setType( QString( CMindMapFile::staticMetaObject.className() ) );
	setFileName( tpl.arg(count) );
}

CMindMapFile::~CMindMapFile()
{

}

QList<QUndoStack*> CMindMapFile::undoStacks()
{
    QList<QUndoStack*>      stacks;
    QList<CMindMapGraph*>   graphes;

    graphes.append( m_graphes);
    graphes.append( m_subGraphes.values() );

    foreach (CMindMapGraph *graph, graphes)
    {
        stacks.push_back( graph->undoStack() );
    }

    return stacks;
}

void CMindMapFile::initDefault()
{
    m_currentGraph	= new CMindMapGraph(this, NULL);
    if (!m_currentGraph)
        return;

    m_currentGraph->initDefault();
    addGraph(m_currentGraph);
}

int CMindMapFile::graphCount() const
{
    return m_graphes.length();
}

CMindMapGraph* CMindMapFile::getGraphById(int id)
{
	CMindMapGraph	*graph = NULL;

	foreach (CMindMapGraph *g, m_graphes)
	{
		if (g->id() == id)
		{
			graph = g;
			break;
		}
	}

	return graph;
}

CMindMapGraph* CMindMapFile::getGraphByIndex(int index)
{
    CMindMapGraph       *graph = NULL;
    
    if (index < 0 || index > m_graphes.length() - 1)
        return NULL;

    graph = m_graphes[index];
    return graph;
}

CMindMapGraph* CMindMapFile::getGraphByTitle(const QString &value)
{
    CMindMapGraph           *graph = NULL;

    foreach (CMindMapGraph *g, m_graphes)
    {
        if (g->title() == value)
        {
            graph = g;
            break;
        }
    }

    return graph;
}

QList<CMindMapGraph*> CMindMapFile::graphes() const
{
    return m_graphes;
}

QStringList CMindMapFile::graphTitles() const
{
    QStringList             titles;
    CMindMapGraph           *graph = NULL;

    foreach (graph, m_graphes)
    {
        titles << graph->title();
    }

    return titles;
}

void CMindMapFile::addGraph(CMindMapGraph *graph)
{
    if (!graph || m_graphes.contains(graph))
        return;

    graph->setId(nextGraphId());
    graph->setFile(this);
    m_graphes.push_back(graph);
    connect(this, SIGNAL(loadFinished()), graph, SLOT(onRebuildGraph()));
    connect(graph, SIGNAL(changed()), this, SLOT(onContentChanged()));

    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}

void CMindMapFile::removeGraph(CMindMapGraph *graph)
{
    if (!graph)
        return;

    if (m_graphes.contains(graph))
    {
        disconnect(this, SIGNAL(loadFinished()), graph, SLOT(onRebuildGraph()));
        disconnect(graph, SIGNAL(changed()), this, SLOT(onContentChanged()));
        m_graphes.removeAll(graph);
        if (graph = m_currentGraph)
            m_currentGraph = NULL;
    }

    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}

void CMindMapFile::moveGraph(int fromIndex, int toIndex)
{
    Q_ASSERT(fromIndex >= 0 && fromIndex < m_graphes.length());
    Q_ASSERT(toIndex >= 0 && toIndex < m_graphes.length());

    m_graphes.move(fromIndex, toIndex);
    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}

CMindMapGraph* CMindMapFile::createSubGraph(CMindMapGraph *parentGraph)
{
	CMindMapGraph		*graph = NULL;
	
	if (!parentGraph)
		return graph;
	
	graph = new CMindMapGraph(this, parentGraph);
	graph->setId( nextSubGraphId() );
	return graph;
}

void CMindMapFile::addSubGraph(CMindMapGraph *graph)
{
	int			id = -1;

	if (!graph)
		return;

	if (!m_subGraphes.contains(id))
	{
		m_subGraphes[id] = graph;
        connect(this, SIGNAL(loadFinished()), graph, SLOT(onRebuildGraph()));
        connect(graph, SIGNAL(changed()), this, SLOT(onContentChanged()));
	}

    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}

void CMindMapFile::removeSubGraph(CMindMapGraph *graph)
{
    int         id;

    if (!graph)
        return;

    disconnect(this, SIGNAL(loadFinished()), graph, SLOT(onRebuildGraph()));
    disconnect(graph, SIGNAL(changed()), this, SLOT(onContentChanged()));

    id = graph->id();
    m_subGraphes.remove(id);

    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}

CMindMapGraph* CMindMapFile::getSubGraphById(int value)
{
    CMindMapGraph   *graph = NULL;

    if (m_subGraphes.contains(value))
        graph = m_subGraphes[value];

    return graph;
}

void CMindMapFile::setNextSubGraphId(int value)
{
    if (value < 0)
        return;

    m_nextSubGraphId = value;
}

int CMindMapFile::nextSubGraphId()
{
    int     r = m_nextSubGraphId;

    ++ m_nextSubGraphId;
    return r;
}

void CMindMapFile::setNextGraphId(int value)
{
    if (value < 0)
        return;

    m_nextGraphId = value;
}

int CMindMapFile::nextGraphId()
{
    int     r = m_nextGraphId;

    ++ m_nextGraphId;
    return r;
}

CDiagram* CMindMapFile::getEditDiagram()
{
    if (!m_currentGraph && m_graphes.length() > 0)
        m_currentGraph = m_graphes.first();

	return dynamic_cast<CDiagram*>( m_currentGraph );
}

void CMindMapFile::setEditDiagram(CDiagram *d)
{
	if (!d)
		m_currentGraph = dynamic_cast<CMindMapGraph*>( d );
}

void CMindMapFile::save(const QString &fullPath)
{
	QByteArray		data;
	QFileInfo		fileInfo(fullPath);
    
	// get the data to saved
	save(data);
	
	// serialize it
	if (!serialize( data, fullPath))
		return;
	
    if (!m_hasPhysicalFile)
	{
		m_createdDate = QDateTime::currentDateTimeUtc();
		m_hasPhysicalFile = true;
	}

    m_fileName = fileInfo.fileName();
    m_path = fileInfo.path();
    m_lastUpdatedDate = QDateTime::currentDateTimeUtc();
    m_revision += 1;
    m_unsaved = false;
    emit saveStatusChanged(!m_unsaved);
}

void CMindMapFile::save(QByteArray &data)
{
    QDomDocument    doc;

    // generate xml file from the file content
    toXml(doc);

    // compress the string to save space
    // data = qCompress(doc.toString().toUtf8(), 7);
    data = doc.toString().toUtf8();
}

void CMindMapFile::load(const QString &fullPath)
{
    QFileInfo           fileInfo(fullPath);
    QByteArray          data;

    if (!unserialize(fullPath, data))
		return;

    // then we can get the pure data
    load( data );
	
    m_path = fileInfo.path();
    m_fileName = fileInfo.fileName();
	m_hasPhysicalFile = true;
    m_unsaved = false;

    emit loadFinished();
}

void CMindMapFile::load(QByteArray &data)
{
    QDomDocument	doc;

	if (doc.setContent( QString::fromUtf8(data) ))
    {
        fromXml(doc);
    }
}

void CMindMapFile::toXml(QDomDocument &doc)
{
    /*
     * <?xml version="1.0"?>
     * <mindmap version="1.0">
     *  <author></author>
     *  <revision></revision>
     *  <createdDate></createdDate>
     *  <lastUpdatedDate></lastUpdatedDate>
     *  <graphCount>12</graphCount>
     *  <subGraphCount>10</subGraphCount>
     *  <graphes currentIndex = "1">
     *      <graph>
     *          ...
     *      </graph>
     *      <graph>
     *          ...
     *      </graph>
     *      ...
     *  <graphes>
     *  <subGraphes>
     *      <subGraph>
     *          ...
     *      </subGraph>
     *      <subGraph>
     *          ...
     *      </subGraph>
     *      ...
     *  </subGraphes>
     * </mindmap>
     **/
    QDomElement         mindmap, tmp;
    QDomElement         graphes, subGraphes;

    mindmap = doc.createElement("mindmap");
    mindmap.setAttribute("version", "1.0");

    tmp = doc.createElement("author");
    tmp.appendChild( doc.createTextNode(author()) );
    mindmap.appendChild(tmp);

    tmp = doc.createElement("revision");
    tmp.appendChild( doc.createTextNode( QString::number(revision()) ));
    mindmap.appendChild(tmp);

    tmp = doc.createElement("createdDate");
    tmp.appendChild( doc.createTextNode( createdDate().toString("")));
    mindmap.appendChild(tmp);

    tmp = doc.createElement("lastUpdatedDate");
    tmp.appendChild( doc.createTextNode( lastUpdatedDate().toString( )) );
    mindmap.appendChild(tmp);

    tmp = doc.createElement("graphCount");
    tmp.appendChild( doc.createTextNode( QString::number(m_graphes.count())));
    mindmap.appendChild(tmp);

    tmp = doc.createElement("subGraphCount");
    tmp.appendChild( doc.createTextNode( QString::number(m_subGraphes.count())));
    mindmap.appendChild(tmp);

    tmp = doc.createElement("nextSubGraphId");
    tmp.appendChild( doc.createTextNode( QString::number(m_nextSubGraphId)));
    mindmap.appendChild(tmp);

    tmp = doc.createElement("nextGraphId");
    tmp.appendChild( doc.createTextNode( QString::number(m_nextGraphId)));
    mindmap.appendChild(tmp);

    // top level graphes
    graphes = doc.createElement("graphes");
    foreach (CMindMapGraph *graph, m_graphes)
    {
        QDomElement         el;

        el = doc.createElement("graph");
        graph->toXml(el);
        graphes.appendChild( el );
    }
    mindmap.appendChild(graphes);

    // sub graphes
    subGraphes = doc.createElement("subGraphes");
    foreach (CMindMapGraph *graph, m_subGraphes.values())
    {
        QDomElement         el;

        el = doc.createElement("subGraph");
        graph->toXml(el);
        subGraphes.appendChild( el );
    }
    mindmap.appendChild(subGraphes);

    doc.appendChild(mindmap);
}

void CMindMapFile::fromXml(const QDomDocument &doc)
{
    QString             version;
    CMindMapGraph       *graph = NULL;
    QDomElement         root;
    QDomElement         graphesElement, graphElement;
    QDomElement         subGraphesElement, subGraphElement;

    root = doc.documentElement();
    if (root.isNull())
        return;

    version = root.attribute("version");
    if (version != "1.0")
    {
        qDebug() << "Incorrect file format version.";
        return;
    }

    setAuthor( root.firstChildElement("author").text() );
    setRevision( root.firstChildElement("revision").text().toInt() );
    setCreatedDate( QDateTime::fromString(root.firstChildElement("createdDate").text(), "") );
    setLastUpdatedDate( QDateTime::fromString(root.firstChildElement("lastUpdatedDate").text(), "") );
    setNextGraphId( root.firstChildElement("nextGraphId").text().toInt() );
    setNextSubGraphId( root.firstChildElement("nextSubGraphId").text().toInt() );

    // recreate subgraphes
    subGraphesElement = root.firstChildElement("subGraphes");
    subGraphElement = subGraphesElement.firstChildElement("subGraph");
    while (!subGraphElement.isNull())
    {
        graph = new CMindMapGraph(this);
        graph->fromXml(subGraphElement);
        addSubGraph(graph);
        subGraphElement = subGraphElement.nextSiblingElement("subGraph");
    }

    // recreate graphes
    graphesElement = root.firstChildElement("graphes");
    graphElement = graphesElement.firstChildElement("graph");
    while (!graphElement.isNull())
    {
        graph = new CMindMapGraph(this);
        graph->fromXml(graphElement);
        addGraph(graph);
        graphElement = graphElement.nextSiblingElement("graph");
    }

    if (m_graphes.length() > 0)
        m_currentGraph = m_graphes.first();
}

QAbstractItemModel* CMindMapFile::dataModel()
{
	QStandardItem			*root = NULL, *item = NULL;
	QStandardItemModel		*model = NULL;
	QStandardItemModel		*graphModel = NULL;

	model = new QStandardItemModel(this);
	root = model->invisibleRootItem();
	foreach (CMindMapGraph *graph, m_graphes)
	{
		graphModel = dynamic_cast<QStandardItemModel*>( graph->dataModel() );
		if (graphModel && root)
		{
			item = graphModel->invisibleRootItem();
			root->appendRow(item);
		}
	}

	return model;
}

void CMindMapFile::onGraphChanged()
{
    m_unsaved = true;
    emit saveStatusChanged(!m_unsaved);
}
