/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CDiagramArrow>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramArrowDatabase>

CDiagramArrowDatabase* CDiagramArrowDatabase::m_inst = NULL;

CDiagramArrowDatabase::CDiagramArrowDatabase()
	: m_defaultArrow(NULL)
{
}

CDiagramArrowCategory* CDiagramArrowDatabase::getCategory(const QString &name)
{
	CDiagramArrowCategory		*c = NULL;

	if (m_categories.find(name) != m_categories.end())
	{
		c = m_categories[name];
	}
	return c;
}

CDiagramArrowCategory* CDiagramArrowDatabase::getDefaultCategory()
{
	CDiagramArrowCategory		*c = NULL;

	if (!m_categories.empty())
		c = m_categories.values().first();	
	return c;
}

int CDiagramArrowDatabase::registerCategory(const QString &name, CDiagramArrowCategory* ctg)
{
	int		r = -1;

	if (m_categories.find(name) == m_categories.end())
	{
		m_categories[name] = ctg;
		r = 0;
	}
	return r;
}

CDiagramArrow* CDiagramArrowDatabase::getDefaultArrow()
{
	CDiagramArrowCategory	*ctg = NULL;

	if (!m_defaultArrow)
	{
		ctg = getDefaultCategory();
		if (ctg)
			m_defaultArrow = ctg->getDefaultArrow();
	}
	return m_defaultArrow;
}

void CDiagramArrowDatabase::setDefaultArrow(CDiagramArrow *arrow)
{
	m_defaultArrow = arrow;
}