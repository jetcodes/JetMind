/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>

CDiagramEdgeStyleCategory::CDiagramEdgeStyleCategory(const QString &name)
	: m_name(name)
{
	load();
}

CDiagramEdgeStyleCategory::~CDiagramEdgeStyleCategory()
{

}

int CDiagramEdgeStyleCategory::count()
{
	return m_styles.count();
}

CDiagramEdgeStyle* CDiagramEdgeStyleCategory::getStyle(const QString &name)
{
	CDiagramEdgeStyle		*r = NULL;

	if (m_styles.contains(name))
		r = m_styles[name];
	return r;
}

CDiagramEdgeStyle* CDiagramEdgeStyleCategory::getDefaultStyle()
{
	int						index;
	CDiagramEdgeStyle		*c = NULL;

	index = m_styles.count() / 2;
	if (!m_styles.empty())
		c = m_styles.values().at(index);	
	return c;
}

QList<CDiagramEdgeStyle*> CDiagramEdgeStyleCategory::styles()
{
	return m_styles.values();
}