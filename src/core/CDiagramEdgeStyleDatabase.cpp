/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>

CDiagramEdgeStyleDatabase* CDiagramEdgeStyleDatabase::m_inst = NULL;

CDiagramEdgeStyleDatabase::CDiagramEdgeStyleDatabase()
	: m_defaultRouter(NULL)
{
}

CDiagramEdgeStyleCategory* CDiagramEdgeStyleDatabase::getCategory(const QString &name)
{
	CDiagramEdgeStyleCategory		*c = NULL;

	if (m_categories.contains(name))
	{
		c = m_categories[name];
	}
	return c;
}

CDiagramEdgeStyleCategory* CDiagramEdgeStyleDatabase::getDefaultCategory()
{
	CDiagramEdgeStyleCategory		*c = NULL;

	if (!m_categories.empty())
		c = m_categories.values().first();	
	return c;
}

int CDiagramEdgeStyleDatabase::registerCategory(const QString &name, CDiagramEdgeStyleCategory* ctg)
{
	int		r = -1;

	if (!m_categories.contains(name))
	{
		m_categories[name] = ctg;
		r = 0;
	}
	return r;
}

void CDiagramEdgeStyleDatabase::setDefaultRouter(CDiagramEdgeStyle *value)
{
	m_defaultRouter = value;
}

CDiagramEdgeStyle* CDiagramEdgeStyleDatabase::getDefaultRouter()
{
	return m_defaultRouter;
}