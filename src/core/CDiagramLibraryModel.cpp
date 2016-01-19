/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>

#include "CDiagramLibraryModel.h"

CDiagramLibraryModel* CDiagramLibraryModel::m_inst = NULL;

CDiagramLibraryItem::CDiagramLibraryItem()
	: m_initWidth(0),
	  m_initHeight(0),
	  m_initMethod(InitedByCode),
	  m_initParam("")
{
}

QString CDiagramLibraryItem::toString()
{
	QString	  tpl="%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11";

	return tpl.arg(m_name)
			  .arg(m_desc)
			  .arg(m_group)
			  .arg(m_iconUrl)
			  .arg(m_metaType)
			  .arg(m_metaName)
			  .arg(m_initMethod)
			  .arg(m_initParam)
			  .arg(m_initWidth)
			  .arg(m_initHeight)
			  .arg(m_libCategory);
}

void CDiagramLibraryItem::fromString(const QString &content)
{
	int				index;
	QStringList		list;
	QString			values[11];

	list = content.split(";");
	index = qMin(11, list.count());
	for (int i = 0; i < index; ++i)
	{
		values[i] = list.at(i);
	}

	m_name			= values[0];
	m_desc			= values[1];
	m_group			= values[2];
	m_iconUrl		= values[3];
	m_metaType		= values[4];
	m_metaName		= values[5];
	m_initMethod	= values[6];
	m_initParam		= values[7];
	m_initWidth		= values[8].toFloat();
	m_initHeight	= values[9].toFloat();
	m_libCategory	= values[10];

	m_icon = QIcon(m_iconUrl);
}

void CDiagramLibraryItem::toXml(QDomElement &e)
{
	QDomElement		el;
	QDomDocument	doc;

	doc = e.ownerDocument();
	el = doc.createElement("name");
	el.appendChild(doc.createTextNode(m_name));
	e.appendChild(el);

	el = doc.createElement("desc");
	el.appendChild(doc.createTextNode(m_desc));
	e.appendChild(el);

	el = doc.createElement("icon");
	el.setAttribute("type", QFileInfo(m_iconUrl).isAbsolute() ? "absolute" : "relative");
	el.appendChild( doc.createTextNode(m_iconUrl) );
	e.appendChild(el);

	el = doc.createElement("metaobject");
	el.setAttribute("category", m_metaType);
	el.appendChild(doc.createTextNode(m_metaName));
	e.appendChild(el);

	el = doc.createElement("init");
	el.setAttribute("method", m_initMethod);
	el.setAttribute("type", QFileInfo(m_initParam).isAbsolute() ? "absolute" : "relative");
	el.setAttribute("width", QString::number(m_initWidth));
	el.setAttribute("height", QString::number(m_initHeight));
	el.appendChild( doc.createTextNode(m_initParam) );
	e.appendChild(el);

	return;
}

void CDiagramLibraryItem::fromXml(const QDomElement &e)
{
	
}

QIcon CDiagramLibraryItem::icon()
{
	return m_icon;
}

void CDiagramLibraryItem::setIcon(const QIcon &value)
{
	m_icon = value;
}

QString CDiagramLibraryItem::iconUrl()
{
	return m_iconUrl;
}

void CDiagramLibraryItem::setIconUrl(const QString &value)
{
	m_iconUrl = value;
	m_icon = QIcon(value);
}

QString CDiagramLibraryItem::name()
{
	return m_name;
}

void CDiagramLibraryItem::setName(const QString &value)
{
	m_name = value;
}

QString CDiagramLibraryItem::desc()
{
	return m_desc;
}

void CDiagramLibraryItem::setDesc(const QString &value)
{
	m_desc = value;
}

QString CDiagramLibraryItem::group()
{
	return m_group;
}

void CDiagramLibraryItem::setGroup(const QString &value)
{
	m_group = value;
}

void CDiagramLibraryItem::setLibraryCategory(const QString &value)
{
	m_libCategory = value;
}

QString CDiagramLibraryItem::libraryCategory()
{
	return m_libCategory;
}

QString CDiagramLibraryItem::metaType()
{
	return m_metaType;
}

void CDiagramLibraryItem::setMetaType(const QString &value)
{
	m_metaType = value;
}

QString CDiagramLibraryItem::metaName()
{
	return m_metaName;
}

void CDiagramLibraryItem::setMetaName(const QString &value)
{
	m_metaName = value;
}

QString CDiagramLibraryItem::initMethod()
{
	return m_initMethod;
}

void CDiagramLibraryItem::setInitMethod(const QString &value)
{
	m_initMethod = value;
}

QString CDiagramLibraryItem::initParam()
{
	return m_initParam;
}

void CDiagramLibraryItem::setInitParam(const QString &value)
{
	m_initParam = value;
}

qreal CDiagramLibraryItem::initWidth()
{
	return m_initWidth;
}

void CDiagramLibraryItem::setInitWidth(qreal value)
{
	m_initWidth = value;
}

qreal CDiagramLibraryItem::initHeight()
{
	return m_initHeight;
}

void CDiagramLibraryItem::setInitHeight(qreal value)
{
	m_initHeight = value;
}

CDiagramLibraryCategoryModel::CDiagramLibraryCategoryModel(QObject *parent, const QString &file)
	: QAbstractListModel(parent),
	  m_viewMode(QListView::ListMode),
	  m_order(0),
	  m_selected(true),
	  m_generate(XmlConfiguredCategory)
{
	if (!file.isEmpty())
		loadFromFile(file);
}

QVariant CDiagramLibraryCategoryModel::data(const QModelIndex &index, int role) const
{
	QVariant		r;
	QString			tooltipTpl("%1\r\n%2");
	CDiagramLibraryItem	*item = NULL;
	const int		row = index.row();

	if (row < 0 || row >= m_items.size())
		return QVariant();

	item = m_items.at(row);
	switch(role)
	{
	case Qt::DisplayRole:
		r = QVariant(item->name() );
		break;

	case Qt::DecorationRole:
		r = QVariant( item->icon() );
		break;

	case Qt::EditRole:
		break;

	case Qt::ToolTipRole:
		r = QVariant( tooltipTpl.arg(item->desc()).arg( tr("Drag / Double Click to Add")) );
		break;

	case FilterNameRole:
		r = QVariant(item->name() );
		break;

	case FilterGroupRole:
		r = QVariant(item->group());
		break;

	default:
		break;
	};

	return r;
}

int CDiagramLibraryCategoryModel::rowCount(const QModelIndex &parent) const
{
	return m_items.size();
}

bool CDiagramLibraryCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	CDiagramLibraryItem		*item = NULL;
	const int			row = index.row();

	if (role != Qt::EditRole || row < 0 || row >= m_items.size() || value.type() != QVariant::String)
		return false;

	item = m_items.at(row);
	item->setName( value.toString() );

	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags CDiagramLibraryCategoryModel::flags(const QModelIndex &index ) const
{
	Qt::ItemFlags		rc = Qt::ItemIsEnabled;
	const int			row = index.row();

	if (row > 0 && row < m_items.size())
	{
		rc |= Qt::ItemIsSelectable;
		if (m_viewMode == QListView::ListMode)
			rc |= Qt::ItemIsEditable;
	}
	return rc;
}

bool CDiagramLibraryCategoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	const int		size = m_items.size();
	const int		last = row + count - 1;

	if (row < 0 || row >= size || last >= size || count < 1)
		return false;
	
	beginRemoveRows(parent, row, last);
	for (int i = last; i >= row; --i)
		m_items.removeAt(i);
	endRemoveRows();

	return true;
}

QListView::ViewMode CDiagramLibraryCategoryModel::viewMode() const
{
	return m_viewMode; 
}

void CDiagramLibraryCategoryModel::setViewMode(QListView::ViewMode m)
{
	if (m_viewMode == m)
		return;

	m_viewMode = m;
	reset();
}

CDiagramLibraryItem* CDiagramLibraryCategoryModel::itemAt(const QModelIndex &index) const
{
	return itemAt(index.row());
}

CDiagramLibraryItem* CDiagramLibraryCategoryModel::itemAt(int row) const
{
	CDiagramLibraryItem		*r = NULL;

	if (row >= 0 && row < m_items.size())
		r = m_items.at(row);
	return r;
}

void CDiagramLibraryCategoryModel::loadFromFile(const QString &file)
{
	QFile			inFile;
	QFileInfo		info(file);
	QTextStream		stream(&inFile);

	m_config = file;
	if (info.exists() && info.isFile())
	{
		inFile.setFileName(info.absoluteFilePath());
		if (!inFile.open(QIODevice::ReadOnly))
		{
			// Error handling here
			return;
		}

		loadFromString(stream.readAll());
		inFile.close();
	}
}

void CDiagramLibraryCategoryModel::loadFromString(const QString &content)
{
    QString                         genWay, libCtg;
    QString                         group;
    QDomElement                     e1, e2;
    QDomDocument                    doc("category");
	CDiagramLibraryItem				*item = NULL;
	QList<CDiagramLibraryItem*>		*itemList;
	CDiagramItemTypeCategory		*ctg = NULL;
    CDiagramItemType                *wrapper = NULL;
    QList<CDiagramItemType*>        wrapperItems;

	QString errorMsg;
	int	errorLine, errorCol;
	if (!doc.setContent(content, &errorMsg, &errorLine, &errorCol))
	{
		qDebug() << "doc error:" << errorMsg << " line:" << errorLine << " col:" << errorCol;
		return;
	}
	e1 = doc.documentElement();
	genWay = e1.attribute("generate");
	m_generate = e1.attribute("generate");

	// the category's items are generated from code
	// instead of xml files and images
	if (genWay == AutoGeneratedCategory )
	{
		libCtg = e1.attribute("name");
		ctg = CDiagramItemTypeDatabase::instance()->getCategory(libCtg);
		if (ctg)
		{
			wrapperItems = ctg->items();
			for (int i = 0; i < wrapperItems.count(); ++i)
			{
				wrapper = wrapperItems.at(i);
				group = wrapper->group();
				item = new CDiagramLibraryItem();
				item->setName( wrapper->name() );
				item->setDesc( wrapper->desc() );
				item->setGroup( group );
				item->setIcon( wrapper->icon() );
				item->setLibraryCategory(name());
				item->setMetaName( wrapper->name() );
				item->setMetaType( libCtg );
				item->setInitMethod( InitedByCode );
				m_items.push_back(item);
				if (m_groups.contains(group))
				{
					m_groups[group]->push_back(item);
				}
				else
				{
					itemList = new QList<CDiagramLibraryItem*>();
					itemList->push_back(item);
					m_groups.insert(group, itemList);
				}
			}
		}
	}
	else
	{
		e1 = e1.firstChildElement( "group" );
		while (!e1.isNull())
		{
			group = e1.attribute("name");
			e2 = e1.firstChildElement( "shape" );
			itemList = new QList<CDiagramLibraryItem*>();
			while (!e2.isNull())
			{
				item = loadItem(e2);
				if (item)
				{
					item->setGroup(group);
					item->setLibraryCategory(name());
					m_items.push_back(item);
					itemList->push_back(item);
				}
				e2 = e2.nextSiblingElement( "shape" );
			}
			if (itemList->count() > 0)
				m_groups.insert(group, itemList);
			e1 = e1.nextSiblingElement( "group" );
		}
	}
}

CDiagramLibraryItem* CDiagramLibraryCategoryModel::loadItem(const QDomElement &n)
{
    QDir                    dir;
    QString                 fullPath;
    CDiagramLibraryItem     *item = NULL;
    QDomElement             element;

    dir.setPath( CDiagramLibraryModel::instance()->libraryRootPath() );
	item = new CDiagramLibraryItem;
	if (item)
	{
		item->setName( n.firstChildElement( "name" ).text() );
		item->setDesc( n.firstChildElement( "desc" ).text() );
		item->setMetaType( n.firstChildElement( "metaobject" ).attribute("category") );
		item->setMetaName( n.firstChildElement( "metaobject" ).text() );

		element = n.firstChildElement( "icon" );
		if (element.attribute("type") == "relative")
			fullPath = dir.absoluteFilePath(element.text());
		else
			fullPath = element.text();
		item->setIconUrl( fullPath );

		element = n.firstChildElement( "init" );
		item->setInitMethod( element.attribute("method") );
		item->setInitWidth( element.attribute("width").toFloat() );
		item->setInitHeight( element.attribute("height").toFloat() );
		if (element.attribute("type") == "relative")
			fullPath = dir.absoluteFilePath(element.text());
		else
			fullPath = element.text();
		item->setInitParam( fullPath );
	}

	return item;
}

QList<QString> CDiagramLibraryCategoryModel::groups()
{
	return m_groups.keys();
}

QList<CDiagramLibraryItem*> CDiagramLibraryCategoryModel::items()
{
	return m_items;
}

void CDiagramLibraryCategoryModel::setItems(const QList<CDiagramLibraryItem*> &value)
{
	QString						group;
	CDiagramLibraryItem			*item = NULL;
	QList<CDiagramLibraryItem*>	*list = NULL;
	QMap<QString, QList<CDiagramLibraryItem*>* >::iterator it;

	for (it = m_groups.begin(); it != m_groups.end(); ++it)
	{
		SAFE_DELETE(it.value());
	}
	m_items = value;
	m_groups.clear();

	for (int i = 0; i < m_items.length(); ++i)
	{
		item = m_items.at(i);
		group = item->group();
		if (!m_groups.contains(group))
		{
			list = new QList<CDiagramLibraryItem*>();
			m_groups.insert(group, list);
		}
		else
		{
			m_groups[group]->push_back(item);
		}
	}
	reset();
}

void CDiagramLibraryCategoryModel::addItem(CDiagramLibraryItem* item)
{
	QString		group;

	if (!item)
		return;

	m_items.push_back(item);
	group = item->group();
	if (!m_groups.contains(group))
	{
		m_groups.insert(group, new QList<CDiagramLibraryItem*>());
	}
	m_groups[group]->push_back(item);

	reset();
}

void CDiagramLibraryCategoryModel::removeItem(CDiagramLibraryItem *item)
{
	QString		group;

	if (!item)
		return;

	m_items.removeAll(item);
	group = item->group();
	if (m_groups.contains(group))
	{
		m_groups[group]->removeAll(item);
	}
	
	reset();
}

bool CDiagramLibraryCategoryModel::selected()
{
	return m_selected;
}

void CDiagramLibraryCategoryModel::setSelected(bool value)
{
	m_selected = value;
}

int CDiagramLibraryCategoryModel::order()
{
	return m_order;
}

void CDiagramLibraryCategoryModel::setOrder(int value)
{
	m_order = value;
}

int CDiagramLibraryCategoryModel::count()
{
	return m_items.count();
}

QString CDiagramLibraryCategoryModel::name()
{
	return m_name;
}

void CDiagramLibraryCategoryModel::setName(const QString &value)
{
	m_name = value;
}

QString CDiagramLibraryCategoryModel::description()
{
	return m_desc;
}

void CDiagramLibraryCategoryModel::setDescription(const QString &value)
{
	m_desc = value;
}

QString CDiagramLibraryCategoryModel::toString()
{
	QDomDocument	doc("library");

	toXml(doc); 

	return doc.toString();
}

void CDiagramLibraryCategoryModel::fromString(const QString &value)
{
	loadFromString(value);
}

void CDiagramLibraryCategoryModel::toXml(QDomDocument &doc)
{
	QString							xml("<?xml version=\"2\"?>");
	QDomElement						c;
	QDomElement						el, g;
	QString							group;
	CDiagramLibraryItem				*item = NULL;
	QList<CDiagramLibraryItem*>*	items;
	QMap<QString, QList<CDiagramLibraryItem*>* >::iterator	it;
	
	doc.setContent(xml);
	c = doc.createElement("category");
	c.setAttribute("generate", m_generate);

	for (it = m_groups.begin(); it != m_groups.end(); ++it)
	{
		group = it.key();
		items = it.value();
		g = doc.createElement("group");
		g.setAttribute("name", group);

		for (int i = 0; i < items->length(); ++i)
		{
			item = items->at(i);
			el = doc.createElement("shape");
			item->toXml(el);
			g.appendChild(el);
		}
		c.appendChild(g);
	}
	doc.appendChild(c);
}

void CDiagramLibraryCategoryModel::fromXml(const QDomDocument &doc)
{
	loadFromString( doc.toString() );
}

CDiagramLibraryModel::CDiagramLibraryModel()
{
}

void CDiagramLibraryModel::loadFromFile(const QString &path, const QString &file)
{
	QFile			inFile;
	QFileInfo		info(path, file);
	QTextStream		stream(&inFile);

	if (info.exists() && info.isFile())
	{
		inFile.setFileName(info.absoluteFilePath());
		if (!inFile.open(QIODevice::ReadOnly))
		{
			// Error handling here
			return;
		}

		loadFromString(stream.readAll());
		inFile.close();
	}
}


void CDiagramLibraryModel::loadFromString(const QString &content)
{
	QDomNode				n;
	QDomElement				element;
	QDomDocument			doc("library");
	CDiagramLibraryCategoryModel	*ctgModel = NULL;

	if (!doc.setContent(content))
	{
		return;
	}
	element = doc.documentElement();

	element = element.firstChildElement( "categories" );
	element = element.firstChildElement( "category" );
	while (!element.isNull())
	{
		ctgModel = loadCategory(element);
		if (ctgModel)
			m_ctgs.push_back(ctgModel);
		element = element.nextSiblingElement( "category" );
	}
}

CDiagramLibraryCategoryModel* CDiagramLibraryModel::loadCategory(const QDomElement &n)
{
    bool                            selected = false;
    int                             order;
    QDir                            dir;
    QString                         name, desc;
    QString                         config;
	CDiagramLibraryCategoryModel	*ctgModel = NULL;

	selected = Utils::stringToBool( n.attribute("selected") );
	name = n.attribute("name");

	order = n.firstChildElement( "order" ).text().toInt();
	desc = n.firstChildElement( "desc" ).text();
	config = n.firstChildElement( "config" ).text();

    dir.setPath( m_libraryRootPath );

	ctgModel = new CDiagramLibraryCategoryModel();
	ctgModel->setName(name);
	ctgModel->setOrder(order);
	ctgModel->setDescription(desc);
	ctgModel->loadFromFile( dir.absoluteFilePath(config) );

	return ctgModel;
}

void CDiagramLibraryModel::addCategory(CDiagramLibraryCategoryModel *model)
{
	m_ctgs.push_back(model);
}

QList<CDiagramLibraryCategoryModel*> CDiagramLibraryModel::categories()
{
	return m_ctgs;
}

void CDiagramLibraryModel::setLibraryRootPath(const QString &path)
{
    m_libraryRootPath = path;
}

QString CDiagramLibraryModel::libraryRootPath() const
{
    return m_libraryRootPath;
}

CDiagramLibraryCategoryModel* CDiagramLibraryModel::getCategory(const QString &name)
{
	CDiagramLibraryCategoryModel	*ctg = NULL;

	for (int i = 0 ; i < m_ctgs.count(); ++i)
	{
		ctg = m_ctgs.at(i);
		if (ctg->name() == name)
			break;
		ctg = NULL;
	}
	return ctg;
}

CSearchResultModel::CSearchResultModel()
	: CDiagramLibraryCategoryModel()
{
	loadModel();
}

void CSearchResultModel::loadModel()
{
	QList<CDiagramLibraryCategoryModel*>	ctgs;
	
	m_items.clear();
	ctgs = CDiagramLibraryModel::instance()->categories();
	foreach (CDiagramLibraryCategoryModel *ctg, ctgs)
	{
		if (!ctg->selected())
			continue;
		foreach (CDiagramLibraryItem *w, ctg->items())
		{
			m_items.push_back(w);
		}
	}
}
