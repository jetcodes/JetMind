/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>

CDiagramItemTypeDatabase* CDiagramItemTypeDatabase::m_inst = NULL;

int CDiagramItemTypeDatabase::registerCategory(const QString &name, CDiagramItemTypeCategory *category)
{
	int		r = -1;

	if (!m_categories.contains(name))
	{
		m_categories[name] = category;
		r = 0;
	}
	return r;
}

CDiagramItemTypeCategory* CDiagramItemTypeDatabase::getCategory(const QString &name)
{
	CDiagramItemTypeCategory			*c = NULL;

	if (m_categories.contains(name))
	{
		c = m_categories[name];
	}
	return c;
}

CDiagramItem* CDiagramItemTypeDatabase::createItem(const QString &category, const QString &name, QGenericArgument val0, QGenericArgument val1)
{
	CDiagramItem					*s = NULL;
	CDiagramItemTypeCategory			*c = NULL;

	c = getCategory(category);
	if (c)
	{
		s = c->createItem(name, val0, val1);
	}
	return s;
}