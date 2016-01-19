/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramNode>
#include <core/CDiagramEditor>
#include <core/CDiagramCommands>
#include <widgets/CUiColorPanel>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtShapeTool.h"
#include "ui_CMindMapDtShapeTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapDtShapeTool::CMindMapDtShapeTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_fillColors(NULL),
	  m_shapeList(NULL)
{
	ui = new Ui::CMindMapDtShapeTool();
	ui->setupUi(this);

    m_fillColors = new CUiColorPanel(NULL);
	m_fillColors->setVisible(false);

	m_shapeList = new CMindMapUiShapeList(this);
	ui->m_btnShape->setMenu(m_shapeList);

	connect(ui->m_btnFillColor, SIGNAL(clicked()), this, SLOT(onFillColor()));
	connect(m_fillColors, SIGNAL(colorSelected(QColor)), this, SLOT(onFillColorSelected(QColor)));
	connect(m_fillColors, SIGNAL(gradientSelected(QGradient)), this, SLOT(onFillGradientSelected(QGradient)));
	connect(m_shapeList, SIGNAL(shapeSelected(int)), this, SLOT(onShapeSelected(int)));
}

CMindMapDtShapeTool::~CMindMapDtShapeTool()
{
	delete ui;
}

bool CMindMapDtShapeTool::support(const QList<CDiagramItem *> &items)
{
    Q_UNUSED(items)

    return true;
}

void CMindMapDtShapeTool::reset()
{
	QList<CMindMapNode*>	nodes;

	if (!m_d)
		return;

	nodes = getAllSelectedNodes(m_d);
	if (nodes.length() == 0)
	{
		ui->m_btnShape->setVisible(false);
	}
	else
	{
		ui->m_btnShape->setVisible(true);
	}
}

void CMindMapDtShapeTool::onFillColor()
{
	QColor					c;
    QPoint					p;
	QBrush					brush;
	QList<CDiagramItem*>	items;

    if (!m_d || !m_p)
		return;

	items = m_d->getItemsHasFocus();
	if (items.length() == 0)
		return;

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnFillColor, m_fillColors);
    if (!p.isNull())
    {
        m_fillColors->raise();
        m_fillColors->show();
        m_fillColors->move( p );
    }

	brush = items.at(0)->brush();
	if (brush.gradient()) 
	{
		m_fillColors->setMode( CUiColorPanel::GradientFill );
		m_fillColors->setGradient(*brush.gradient());
	}
	else
	{
		c = brush.color();
		m_fillColors->setMode( CUiColorPanel::SolidFill );
		m_fillColors->setSelectedColor(c);
	}
}

void CMindMapDtShapeTool::onFillColorSelected(const QColor &c)
{	
	QList<CDiagramItem*>	items;
	//CMindMapNode			*node = NULL;
	//QList<CMindMapNode*>	nodes;
	
    if (!m_d)
        return;
	
    items = m_d->selectedTopLevelItems();
	/*foreach (CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/

    if (items.length() > 0)
        m_d->addUndoCommand( new CMindMapChangePropertyCommand(m_d, items, "brush", QVariant(QBrush(c))));
}

void CMindMapDtShapeTool::onFillGradientSelected(const QGradient &g)
{
	QList<CDiagramItem*>	items;
	//CMindMapNode			*node = NULL;
	//QList<CMindMapNode*>	nodes;
	
    if (!m_d)
        return;
	
    items = m_d->selectedTopLevelItems();
	//foreach (CDiagramItem *item, items)
	//{
	//	node = dynamic_cast<CMindMapNode*>(item);
	//	if (node)
	//		nodes.push_back(node);
	//}

    if (items.length() > 0)
        m_d->addUndoCommand( new CMindMapChangePropertyCommand(m_d, items, "brush", QVariant(QBrush(g))));
}

void CMindMapDtShapeTool::onShapeSelected(int value)
{
    QList<CDiagramItem*>	items;
    //CMindMapNode			*node = NULL;
    //QList<CMindMapNode*>	nodes;

    if (!m_d || value < CMindMapNode::Rect || value >= CMindMapNode::NoneType)
        return;

    items = m_d->selectedTopLevelItems();
    //foreach (CDiagramItem *item, items)
    //{
    //    node = dynamic_cast<CMindMapNode*>(item);
    //    if (node)
    //        nodes.push_back(node);
    //}

    if (items.length() > 0)
        m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "nodeType", QVariant(value)));
}
