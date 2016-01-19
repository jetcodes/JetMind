/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>

#include "CUiPathStyleList.h"

CUiPathStyleList::CUiPathStyleList(QWidget *parent)
	: QMenu(parent),
	  m_defaultStyle(NULL)
{
	setupMenus();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
	connect(this, SIGNAL(hovered(QAction*)), this, SLOT(onActionHovered(QAction*)));
}

CUiPathStyleList::~CUiPathStyleList()
{

}

void CUiPathStyleList::clearAll()
{
	clear();
	m_styles.clear();
	m_actions.clear();
}

void CUiPathStyleList::addMenu(CDiagramEdgeStyle *r)
{
	QAction		*act = NULL;

	if (!r || m_styles.contains(r))
		return;
	act = addAction(r->getIcon(), r->getName());
	m_styles.push_back(r);
	m_actions.push_back(act);

}

void CUiPathStyleList::addMenu(CDiagramEdgeStyleCategory *ctg)
{
	if (!ctg) 
		return;

	foreach (CDiagramEdgeStyle *r, ctg->styles())
	{
		addMenu(r);
	}
}

CDiagramEdgeStyle* CUiPathStyleList::defaultStyle()
{
	CDiagramEdgeStyle		*r = NULL;

	if (m_defaultStyle) 
	{
		r = m_defaultStyle;
	}
	else
	{
		if (m_styles.count() > 4)
			r = m_styles[4];
		else if (m_styles.count() > 0)
			r = m_styles[0];
		m_defaultStyle = r;
	}
	return r;
}

void CUiPathStyleList::setDefaultStyle(CDiagramEdgeStyle *style)
{
	if (style)
		m_defaultStyle = style;
}

void CUiPathStyleList::setupMenus()
{
	CDiagramEdgeStyleCategory		*ctg = NULL;
	ctg = CDiagramEdgeStyleDatabase::instance()->getCategory( Global::CTG_COREITEMS );
	if (!ctg) return;
	
	addMenu(ctg);
}

void CUiPathStyleList::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	m_defaultStyle = m_styles[i];
	emit styleSelected( m_defaultStyle );
}

void CUiPathStyleList::onActionHovered(QAction *act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	if (i < m_actions.length())
		emit styleHovered( m_styles[i] );
}