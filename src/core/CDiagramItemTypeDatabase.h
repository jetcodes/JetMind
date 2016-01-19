/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMITEMDATABASE_H
#define PRODRAW_CDIAGRAMITEMDATABASE_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramItem;
class CDiagramItemTypeCategory;

// CDiagramItemTypeDatabase class provides the storage and
// manipulation of item categories.
class CORE_EXPORT CDiagramItemTypeDatabase: public QObject
{
	Q_OBJECT

public:
	int categoryCount() { return m_categories.count(); }
	QList<QString> categoryNames() { return m_categories.keys(); }
	QList<CDiagramItemTypeCategory*> categories() { return m_categories.values(); }

	int registerCategory(const QString &name, CDiagramItemTypeCategory *category);
	CDiagramItemTypeCategory* getCategory(const QString &name);
	CDiagramItem* createItem(const QString &category, 
							 const QString &name, 
							 QGenericArgument val0 = QGenericArgument( 0 ), 
							 QGenericArgument val1 = QGenericArgument());

	static CDiagramItemTypeDatabase* instance()
	{
		if ( m_inst == NULL)
		{
			m_inst = new CDiagramItemTypeDatabase();
		}
		return m_inst;
	}

	static void destroy()
	{
		if (m_inst != NULL)
		{
			delete m_inst;
			m_inst = NULL;
		}
	}

private:
	CDiagramItemTypeDatabase() {}
	Q_DISABLE_COPY(CDiagramItemTypeDatabase)

	static CDiagramItemTypeDatabase			   *m_inst;
	QMap<QString, CDiagramItemTypeCategory*>	m_categories;

}; // End of class CDiagramItemTypeDatabase

#endif // PRODRAW_CDIAGRAMITEMDATABASE_H
