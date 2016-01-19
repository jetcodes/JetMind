/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMEDGESTYLEDATABASE_H
#define PRODRAW_CDIAGRAMEDGESTYLEDATABASE_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramEdgeStyle;
class CDiagramEdgeStyleCategory;

class CORE_EXPORT CDiagramEdgeStyleDatabase: public QObject
{
	Q_OBJECT

public:	
	int categoryCount() { return m_categories.count(); }
	QList<QString> categoryNames() { return m_categories.keys(); }
	QList<CDiagramEdgeStyleCategory*> categories() { return m_categories.values(); }

	int registerCategory(const QString &name, CDiagramEdgeStyleCategory *category);
	CDiagramEdgeStyleCategory* getCategory(const QString &name);
	CDiagramEdgeStyleCategory* getDefaultCategory();
	CDiagramEdgeStyle* getDefaultRouter();
	void setDefaultRouter(CDiagramEdgeStyle *value);

	// static related
	static CDiagramEdgeStyleDatabase* instance()
	{
		if ( m_inst == NULL)
		{
			m_inst = new CDiagramEdgeStyleDatabase();
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

protected:
	CDiagramEdgeStyleDatabase();

private:
	Q_DISABLE_COPY(CDiagramEdgeStyleDatabase)
	QMap<QString, CDiagramEdgeStyleCategory*>	m_categories;
	CDiagramEdgeStyle							*m_defaultRouter;

	static CDiagramEdgeStyleDatabase				*m_inst;
};

#endif // PRODRAW_CDIAGRAMEDGESTYLEDATABASE_H
