/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <core/CDiagramArrowCategory>

CDiagramArrowCategory::CDiagramArrowCategory(const QString &name)
	: m_name(name)
{
	load();
}

CDiagramArrowCategory::~CDiagramArrowCategory()
{
	
}

int CDiagramArrowCategory::count()
{
	return m_arrows.count();
}

CDiagramArrow* CDiagramArrowCategory::getArrow(const QString &name)
{
	CDiagramArrow		*s = NULL;

	if (m_arrows.contains(name))
		s = m_arrows[name];
	return s;
}

CDiagramArrow* CDiagramArrowCategory::getDefaultArrow()
{
	CDiagramArrow		*s = NULL;

	if (!m_arrows.empty())
		s = m_arrows.values().first();
	return s;
}

QList<CDiagramArrow*> CDiagramArrowCategory::arrows()
{
	return m_arrows.values();
}