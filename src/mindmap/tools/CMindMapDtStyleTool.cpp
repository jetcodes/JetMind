/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramWidget>
#include <core/CDiagramEditor>
#include <core/CDiagramCommands>
#include <widgets/CUiNodeStylePanel>

#include "../CMindMapNode.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtStyleTool.h"
#include "ui_CMindMapDtStyleTool.h"

CMindMapDtStyleTool::CMindMapDtStyleTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_styleGrid(NULL)
{
	ui = new Ui::CMindMapDtStyleTool();
	ui->setupUi(this);

	m_styleGrid = new CUiNodeStylePanel(this);
	m_styleGrid->setVisible(false);
	ui->m_btnFillStyle->setMenu(m_styleGrid);

	connect(ui->m_btnFillStyle, SIGNAL(clicked()), this, SLOT(onFillStyle()));
	connect(m_styleGrid, SIGNAL(styleReset()), this, SLOT(onFillStyleReset()));
	connect(m_styleGrid, SIGNAL(styleHovered(const CDiagramNodeStyle&)), this, SLOT(onFillStyleHovered(const CDiagramNodeStyle&)));
	connect(m_styleGrid, SIGNAL(styleSelected(const CDiagramNodeStyle&)), this, SLOT(onFillStyleSelected(const CDiagramNodeStyle&)));
}

CMindMapDtStyleTool::~CMindMapDtStyleTool()
{
	delete ui;
}

void CMindMapDtStyleTool::reset()
{
	CMindMapNode		*node = NULL;

	if (!m_d)
		return;

	m_oldStyles.clear();
	foreach (CDiagramItem *item , m_d->selectedTopLevelItems())
	{
		CDiagramNodeStyle	style;

		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
		{
			style.setPen(node->pen());
			style.setBrush(node->brush());
		}
		else
		{
			style.setPen(item->pen());
			style.setBrush(item->brush());
		}
		m_oldStyles.insert(item, style);
	}
}

void CMindMapDtStyleTool::onFillStyle()
{
	onFillStyleSelected(m_styleGrid->defaultStyle());
}

void CMindMapDtStyleTool::onFillStyleSelected(const CDiagramNodeStyle &s)
{
	CDiagramNodeStyle		t;
	QList<CDiagramItem*>	items;
	/*CMindMapNode			*node = NULL;
	QList<CMindMapNode*>	nodes;*/
	CDiagramCommand			*cmd = NULL;

    if (!m_d)
        return;

	// first reset it, and then we apply the new style
	onFillStyleReset();

	t = s;
	items = m_d->selectedTopLevelItems();
	//foreach( CDiagramItem *item, items)
	//{
	//	node = dynamic_cast<CMindMapNode*>(item);
	//	if (node)
	//		nodes.push_back(node);
	//}

	// redo will call all the children's redo, if you provide custom
	// redo method's implementation, you should provide a call to all the children's redo
	cmd = new CMindMapChangePropertyCommand(m_d, items, "brush", QVariant(t.brush()));
	new CMindMapChangePropertyCommand(m_d, items, "pen", QVariant(t.pen()), cmd);

	m_d->addUndoCommand(cmd);

}

void CMindMapDtStyleTool::onFillStyleHovered(const CDiagramNodeStyle &style)
{
    CDiagramNodeStyle		t;
    QList<CDiagramItem*>	items;
	CDiagramItem			*item = NULL;
    CMindMapNode			*node = NULL;

    if (!m_d) return;

    t = style;
    items = m_d->selectedTopLevelItems();
    for (int i = 0; i < items.length(); ++i)
    {
		item = items.at(i);
        node = dynamic_cast<CMindMapNode*>(item);
        if (node)
        {
            node->setPen(t.pen());
            node->setBrush(t.brush());
        }
		else
		{
			item->setPen(t.pen());
			item->setBrush(t.brush());
		}
    }
}

void CMindMapDtStyleTool::onFillStyleReset()
{
    CDiagramNodeStyle		style;
    QList<CDiagramItem*>	items;
	CDiagramItem			*item = NULL;
    CMindMapNode			*node = NULL;
    //QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    items = m_d->selectedTopLevelItems();
   /* foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node)
            nodes.push_back(node);
    }*/

    for (int i = 0; i < items.length(); ++i)
    {
		item = items.at(i);
		node = dynamic_cast<CMindMapNode*>(item);
		style = m_oldStyles[item];
		if (node)
		{
			node->setPen(style.pen());
			node->setBrush(style.brush());
		}
		else
		{
			item->setPen(style.pen());
			item->setBrush(style.brush());
		}
    }
}
