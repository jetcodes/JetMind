/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CGlobal>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>

#include "CUiCoreNodePicker.h"

CUiCoreNodePicker::CUiCoreNodePicker(QWidget *parent)
	: QMenu(parent)
{
	setupMenus();
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiCoreNodePicker::~CUiCoreNodePicker()
{
}

void CUiCoreNodePicker::clearAll()
{
	clear();
	m_objects.clear();
	m_actions.clear();
}

void CUiCoreNodePicker::addMenu(CDiagramItemType *w)
{
	QAction			*act = NULL;
	
	if (!w || m_objects.contains(w))
		return;

	act = addAction( w->icon(), w->name() );
	m_actions.push_back(act);
	m_objects.push_back(w);
}

void CUiCoreNodePicker::addMenu(CDiagramItemTypeCategory *ctg)
{
	int				index;
	QMetaObject		*obj = NULL;

	if (!ctg) return;
	foreach (CDiagramItemType *w, ctg->items())
	{
		addMenu(w);
	}
}

CDiagramItemType* CUiCoreNodePicker::defaultShape()
{
	CDiagramItemType	*w = NULL;

	if (m_objects.count() > 0)
		w = m_objects[0];
	return w;
}

void CUiCoreNodePicker::setupMenus()
{
	QAction							*act = NULL;
	CDiagramItemTypeCategory			*ctg = NULL;
	QList<CDiagramItemType*>		shapes;
	CDiagramItemType				*wrapper = NULL;
	CDiagramItemTypeDatabase			*db = NULL;
	QStringList						list;

	list << QString::fromUtf8("Rect") << QString::fromUtf8("Ellipse")
		 << QString::fromUtf8("Rounded Rect") << QString::fromUtf8("Circle")
		 << QString::fromUtf8("Square") << QString::fromUtf8("Rounded Square");

	db = CDiagramItemTypeDatabase::instance();
	ctg = CDiagramItemTypeDatabase::instance()->getCategory( Global::CTG_COREITEMS );
	
	if (!ctg) return;
	
	for (int i = 0; i < list.size(); ++i)
	{
		wrapper = ctg->getItemWrapper(list.at(i));
		if (wrapper)
			addMenu(wrapper);
	}
}

void CUiCoreNodePicker::onActionTriggered(QAction* act)
{
	int		i = 0;

	for(i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	
	emit shapeSelected( m_objects[i] );
}