/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMARROWDATABASE_H
#define PRODRAW_CDIAGRAMARROWDATABASE_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramArrow;
class CDiagramArrowCategory;

class CORE_EXPORT CDiagramArrowDatabase: public QObject
{
	Q_OBJECT

public:
	int categoryCount() { return m_categories.count(); }
	QList<QString> categoryNames() { return m_categories.keys(); }
	QList<CDiagramArrowCategory*> categories() { return m_categories.values(); }

	int registerCategory(const QString &name, CDiagramArrowCategory *s);
	CDiagramArrowCategory* getCategory(const QString &name);
	CDiagramArrowCategory* getDefaultCategory();
	CDiagramArrow* getDefaultArrow();
	void setDefaultArrow(CDiagramArrow* arrow);
	

	// static related
	static CDiagramArrowDatabase* instance()
	{
		if (m_inst == NULL)
		{
			m_inst = new CDiagramArrowDatabase();
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
	CDiagramArrowDatabase();
	Q_DISABLE_COPY(CDiagramArrowDatabase);

	static CDiagramArrowDatabase				*m_inst;
	CDiagramArrow								*m_defaultArrow;
	QMap<QString, CDiagramArrowCategory*>		m_categories;
	
}; // End of class CDiagramArrowDatabase

#endif // PRODRAW_CDIAGRAMARROWDATABASE_H
