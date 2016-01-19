/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramWidget>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtLayoutTool.h"
#include "ui_CMindMapDtLayoutTool.h"

CMindMapDtLayoutTool::CMindMapDtLayoutTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_layoutList(NULL)
{
	ui = new Ui::CMindMapDtLayoutTool();
	ui->setupUi(this);

	m_layoutList = new CMindMapUiLayoutList(this);
	ui->m_btnLayout->setMenu(m_layoutList);

	connect(m_layoutList, SIGNAL(layoutSelected(int)), this, SLOT(onLayoutSelected(int)));
}

CMindMapDtLayoutTool::~CMindMapDtLayoutTool()
{
	delete ui;
}

bool CMindMapDtLayoutTool::support(const QList<CDiagramItem *> &items)
{
    bool            r = false;
    CDiagramItem    *item = NULL;

    if (!m_d)
        return r;

    foreach (item, items)
    {
        if (dynamic_cast<CMindMapNode*>(item))
        {
            r = true;
            break;
        }
    }

    return r;
}

void CMindMapDtLayoutTool::reset()
{
	CMindMapNode				*node = NULL;
	QList<CDiagramItem*>		items;
	QList<CMindMapNode*>		nodes;

	if (!m_d)
		return;
	
	m_layoutList->reset();
	items = m_d->selectedTopLevelItems();
	foreach (CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>( item );
		if (node)
			nodes.push_back(node);
	}
		
	if (nodes.length() == 1)
	{
		m_layoutList->reset( nodes.first()->isRoot() );
	}
}

void CMindMapDtLayoutTool::onLayoutSelected(int value)
{
    /*CMindMapNode				*node = NULL;*/
    QList<CDiagramItem*>		items;
    //QList<CMindMapNode*>		nodes;

	if (!m_d || value < CMindMapNode::MindMap || value >= CMindMapNode::NoneLayout)
		return;

    items = m_d->selectedTopLevelItems();
   /* foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>( item );
        if (node)
            nodes.push_back(node);
    }
	*/
	if (items.length() > 0)
        m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "layoutType", QVariant(value)));
}
