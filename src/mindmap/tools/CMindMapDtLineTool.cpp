/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramNode>
#include <core/CDiagramEditor>
#include <core/CDiagramCommands>
#include <widgets/CUiColorPanel>
#include <widgets/CUiPenStylePicker>
#include <widgets/CUiPenWeightPicker>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtLineTool.h"
#include "ui_CMindMapDtLineTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapDtLineTool::CMindMapDtLineTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_lineColors(NULL),
	  m_lineStyles(NULL),
	  m_lineWeights(NULL),
	  m_pathList(NULL)
{
	ui = new Ui::CMindMapDtLineTool();
	ui->setupUi(this);

    m_lineColors = new CUiColorPanel(NULL);
	m_lineStyles = new CUiPenStylePicker(this);
	m_lineWeights = new CUiPenWeightPicker(this);
	m_lineColors->setMode(CUiColorPanel::SolidOnly);

	ui->m_btnLineStyle->setMenu(m_lineStyles);
	ui->m_btnLineWeight->setMenu(m_lineWeights);

	m_pathList = new CMindMapUiPathList(this);
	ui->m_btnPath->setMenu(m_pathList);

	connect(m_pathList, SIGNAL(pathSelected(int)), this, SLOT(onPathSelected(int)));
	connect(ui->m_btnLineColor, SIGNAL(clicked()), this, SLOT(onLineColor()));
	connect(m_lineColors, SIGNAL(colorSelected(QColor)), this, SLOT(onLineColorSelected(QColor)));
	connect(m_lineStyles, SIGNAL(styleSelected(Qt::PenStyle)), this, SLOT(onLineStyleSelected(Qt::PenStyle)));
	connect(m_lineWeights, SIGNAL(weightSelected(qreal)), this, SLOT(onLineWeightSelected(qreal)));
}

CMindMapDtLineTool::~CMindMapDtLineTool()
{
	delete ui;
}

bool CMindMapDtLineTool::support(const QList<CDiagramItem*> &items)
{
	return true;
}

void CMindMapDtLineTool::reset()
{
	QList<CMindMapNode*>	nodes;

	if (!m_d)
		return;

	nodes = getAllSelectedNodes(m_d);
	if (nodes.length() == 0)
	{
		ui->m_btnPath->setVisible(false);
	}
	else
	{
		ui->m_btnPath->setVisible(true);
	}
}

void CMindMapDtLineTool::onPathSelected(int value)
{
    /*CMindMapNode			*node = NULL;*/
    QList<CDiagramItem*>	items;
    //QList<CMindMapNode*>	nodes;

	if (!m_d || value < CMindMapNode::StraightLine || value >= CMindMapNode::NoneLine)
		return;

	items = m_d->selectedTopLevelItems();
    //foreach( CDiagramItem *item, items)
    //{
    //    node = dynamic_cast<CMindMapNode*>(item);
    //    if (node)
    //        nodes.push_back(node);
    //}

    if (items.length() > 0)
        m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "pathType", QVariant(value)));
}


void CMindMapDtLineTool::onLineColor()
{
    QPoint					p;
	QColor					c;
	QPen					pen;
	QList<CDiagramItem*>	items;

    if (!m_d || !m_p)
		return;

	items = m_d->getItemsHasFocus();
	if (items.length() == 0)
		return;

	foreach (CDiagramItem *item, items)
	{
		if (dynamic_cast<CMindMapNode*>(item))
		{
			pen = item->pen();
			break;
		}
	}
	c = pen.color();
	m_lineColors->setMode( CUiColorPanel::SolidFill );
	m_lineColors->setSelectedColor(c);

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnLineColor, m_lineColors);
    if (!p.isNull())
    {
        m_lineColors->raise();
        m_lineColors->show();
        m_lineColors->move( p );
    }
}


void CMindMapDtLineTool::onLineColorSelected(const QColor &c)
{
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	/*QList<CMindMapNode*>	nodes;*/
	QList<QVariant>			newPens;
	QPen					pen;

	if (!m_d) return;

	items = m_d->selectedTopLevelItems();
	foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
		{
			pen = node->pen();
		}
		else
		{
			pen = item->pen();
		}
		pen.setColor(c);
		newPens.push_back( QVariant(pen) );
	}

	if (items.length() > 0)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "pen", newPens));
}

void CMindMapDtLineTool::onLineStyleSelected(const Qt::PenStyle s)
{
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	QList<CMindMapNode*>	nodes;
	QList<QVariant>			newPens;
	QPen					pen;
	
	if (!m_d) return;

	items = m_d->selectedTopLevelItems();
	foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
		{
			pen = node->pen();
		}
		else
		{
			pen = item->pen();
		}
		pen.setStyle(s);
		newPens.push_back( QVariant(pen) );
	}

	if (items.length() > 0)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "pen", newPens));
}

void CMindMapDtLineTool::onLineWeightSelected(const qreal v)
{
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	//QList<CMindMapNode*>	nodes;
	QList<QVariant>			newPens;
	QPen					pen;
	
	if (!m_d) return;

	items = m_d->selectedTopLevelItems();
	foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
		{
			pen = node->pen();
		}
		else
		{
			pen = item->pen();
		}
		pen.setWidthF(v);
		newPens.push_back( QVariant(pen) );
	}

	if (items.length() > 0)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "pen", newPens));
}
