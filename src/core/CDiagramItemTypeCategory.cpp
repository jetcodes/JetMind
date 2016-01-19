/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>

CDiagramItemType::CDiagramItemType(const QString &name, 
										 const QString &desc, 
										 const QIcon &icon,
										 const QMetaObject *meta,
										 const QString &group)
	: m_name(name),
	  m_desc(desc),
	  m_icon(icon),
	  m_group(group)
{
	m_meta = const_cast<QMetaObject*>(meta);
}

CDiagramItemType::~CDiagramItemType()
{
	if (m_meta)
		delete m_meta;
}

void CDiagramItemType::setName(const QString &name)
{
	m_name = name;
}

void CDiagramItemType::setDesc(const QString &desc)
{
	m_desc = desc;
}

void CDiagramItemType::setIcon(QIcon &icon)
{
	m_icon = icon;
}

void CDiagramItemType::setGroup(const QString &group)
{
	m_group = group;
}

QString CDiagramItemType::group()
{
	return m_group;
}

void CDiagramItemType::setMetaObject(const QMetaObject *meta)
{
	m_meta = const_cast<QMetaObject*>(meta);
}

CDiagramItem* CDiagramItemType::create(QGenericArgument val0, QGenericArgument val1)
{
	CDiagramItem	*r = NULL;

	if (m_meta)
	{
		r = static_cast<CDiagramItem*>(m_meta->newInstance(val0, val1));
	}
	return r;
}

CDiagramItemTypeCategory::CDiagramItemTypeCategory(const QString &name)
	:m_name(name),
	 m_published(true),
	 m_checked(true)
{
	m_groups.push_back("All");
}

QMetaObject* CDiagramItemTypeCategory::getItemMetaObject(const QString &name)
{
	QMetaObject			*o = NULL;

	if (m_items.contains(name))
		o = m_items[name]->metaObject();
	return o;
}

CDiagramItemType* CDiagramItemTypeCategory::getItemWrapper(const QString &name)
{
	CDiagramItemType		*w = NULL;

	if (m_items.contains(name))
		w = m_items[name];
	return w;
}

QList<QString> CDiagramItemTypeCategory::groups()
{
	return m_groups;
}

void CDiagramItemTypeCategory::addType(CDiagramItemType *wrapper)
{
	QString		group;

	group = wrapper->group();
	m_items.insert(wrapper->name(), wrapper);
	if (!m_groups.contains(group))
		m_groups.push_back(group);
}