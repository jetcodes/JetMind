/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramLayout>
#include <core/CDiagramLayoutManager>

CDiagramLayoutManager* CDiagramLayoutManager::m_inst = NULL;

CDiagramLayoutManager::CDiagramLayoutManager()
{
	
}

void CDiagramLayoutManager::addLayout(const QString &key, CDiagramLayout *layout)
{
	if (!layout || m_layouts.contains(key))
		return;
	m_layouts.insert(key, layout);
}

CDiagramLayout* CDiagramLayoutManager::getLayout(const QString &key)
{
	CDiagramLayout	*r = NULL;

	if (m_layouts.contains(key))
		r = m_layouts[key];
	return r;
}

QList<CDiagramLayout*> CDiagramLayoutManager::layouts()
{
	return m_layouts.values();
}

QList<QString> CDiagramLayoutManager::keys()
{
	return m_layouts.keys();
}

CDiagramLayoutManager* CDiagramLayoutManager::inst()
{
	if (m_inst)
		m_inst = new CDiagramLayoutManager();
	return m_inst;
}