/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMITEMCATEGORY_H
#define PRODRAW_CDIAGRAMITEMCATEGORY_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramItem>

// CDiagramItemType wraps the meta object, name, description
// of specified diagram item.
class CORE_EXPORT CDiagramItemType
{
public:
	CDiagramItemType() {}
	CDiagramItemType(const QString &name, 
						const QString &desc, 
						const QIcon &icon, 
						const QMetaObject *meta = NULL, 
						const QString &group = "All");
	virtual ~CDiagramItemType();

	CDiagramItem* create(QGenericArgument val0 = QGenericArgument( 0 ), QGenericArgument val1 = QGenericArgument());

	void setName(const QString &name);
	QString name() { return m_name; }

	void setDesc(const QString &desc);
	QString desc() { return m_desc; }

	void setIcon(QIcon &icon);
	QIcon icon() { return m_icon; }

	void setGroup(const QString &group);
	QString group();

	void setMetaObject(const QMetaObject *meta);
	QMetaObject* metaObject() { return m_meta; }

protected:
	Q_DISABLE_COPY(CDiagramItemType)

	QString			m_name;
	QString			m_desc;
	QString			m_group;
	QIcon			m_icon;
	QMetaObject		*m_meta;

}; // End of class CDiagramItemType

// CDiagramItemTypeCategory provides the basic logic for manipulating the
// different style of items. Different items can be organized into
// different categories.
class CORE_EXPORT CDiagramItemTypeCategory
{
public:
	CDiagramItemTypeCategory(const QString &name);
	virtual ~CDiagramItemTypeCategory() {}
	
	// To be overrided to load customized diagram items
	// inside different plugins.
	virtual void load() {};

	QString name() { return m_name; }
	QIcon icon() { return m_icon; }
	int count() { return m_items.count(); }
	
	void setPublished(bool value) { m_published = value; }
	bool published() { return m_published; }

	void setChecked(bool value) { m_checked = value; }
	bool checked() { return m_checked; }

	QList<CDiagramItemType*> items() { return m_items.values(); }
	QMetaObject* getItemMetaObject(const QString &name);
	CDiagramItemType* getItemWrapper(const QString &name);

	QList<QString> groups(); 

	CDiagramItem* createItem(const QString &name, QGenericArgument val0 = QGenericArgument( 0 ), QGenericArgument val1 = QGenericArgument())
	{
		CDiagramItem		*item = NULL;
		CDiagramItemType	*wrapper = NULL;

		if (m_items.contains(name))
		{
			wrapper = m_items[name];
			item = wrapper->create(val0, val1);
		}
		item->setCategory(m_name);
		item->setName(name);
		return item;
	}

protected:
	void addType(CDiagramItemType *wrapper);

	QMap<QString, CDiagramItemType*>	m_items;
	QString								m_name;
	QIcon								m_icon;
	bool								m_published;
	bool								m_checked;
	QList<QString>						m_groups;

private:
	CDiagramItemTypeCategory();
	Q_DISABLE_COPY(CDiagramItemTypeCategory)

}; // End of class CDiagramItemType

#endif // PRODRAW_CDIAGRAMITEMCATEGORY_H