/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramWidget>
#include <core/CDiagramCommands>
#include <widgets/CUiNodeStylePanel>

#include "../CMindMapNode.h"
#include "../CMindMapNodeSprite.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapIconProvider.h"
#include "../CMindMapCommands.h"
#include "../CMindMapNodeGroup.h"
#include "CMindMapDtQuickTool.h"
#include "ui_CMindMapDtQuickTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapDtQuickTool::CMindMapDtQuickTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_styleGrid(NULL),
	  m_shapeList(NULL),
	  m_layoutList(NULL),
	  m_iconList(NULL),
	  m_pathList(NULL),
	  m_newItemList(NULL),
      m_defaultNewItem(0)
{
    ui = new Ui::CMindMapDtQuickTool();
	ui->setupUi(this);

    m_styleGrid = new CUiNodeStylePanel(NULL);
    //ui->m_btnStyle->setMenu(m_styleGrid);

	m_shapeList = new CMindMapUiShapeList(this);
	ui->m_btnShape->setMenu(m_shapeList);

	m_layoutList = new CMindMapUiLayoutList(this);
	ui->m_btnLayout->setMenu(m_layoutList);

    m_iconList = new CMindMapUiIconList(NULL);
    //ui->m_btnIcon->setMenu(m_iconList);

	m_pathList = new CMindMapUiPathList(this);
	ui->m_btnPath->setMenu(m_pathList);

	m_newItemList = new CMindMapUiNewItemList(this);
	ui->m_btnNew->setMenu(m_newItemList);

    connect(ui->m_btnIcon, SIGNAL(clicked()), this, SLOT(onShowIconMenu()));
    connect(ui->m_btnStyle, SIGNAL(clicked()), this, SLOT(onShowFillStyleMenu()));
	connect(ui->m_btnNew, SIGNAL(clicked()), this, SLOT(onNewNode()));
    connect(ui->m_btnStyle, SIGNAL(clicked()), this, SLOT(onFillStyle()));
    connect(m_newItemList, SIGNAL(insertPositionSelected(int, QIcon)), this, SLOT(onInsertPositionSelected(int, QIcon)));

    connect(ui->m_btnAttach, SIGNAL(clicked()), this, SIGNAL(showAttachFileDialog()));
    connect(ui->m_btnBoundary, SIGNAL(clicked()), this, SIGNAL(addBoundary()));
    connect(ui->m_btnLink, SIGNAL(clicked()), this, SIGNAL(showLinkEditor()));
    connect(ui->m_btnNote, SIGNAL(clicked()), this, SIGNAL(showNoteEditor()));
    connect(ui->m_btnTask, SIGNAL(clicked()), this, SIGNAL(showTaskEditor()));
    connect(m_styleGrid, SIGNAL(styleReset()), this, SIGNAL(fillStyleReset()));
    connect(m_styleGrid, SIGNAL(styleHovered(const CDiagramNodeStyle&)), this, SIGNAL(fillStyleHovered(const CDiagramNodeStyle&)));
    connect(m_styleGrid, SIGNAL(styleSelected(const CDiagramNodeStyle&)), this, SIGNAL(fillStyleSelected(const CDiagramNodeStyle&)));
    connect(m_shapeList, SIGNAL(shapeSelected(int)), this, SIGNAL(shapeSelected(int)));
    connect(m_layoutList, SIGNAL(layoutSelected(int)), this, SIGNAL(layoutSelected(int)));
    connect(m_iconList, SIGNAL(iconSelected(const QIcon&, int)), this, SIGNAL(iconSelected(const QIcon&, int)));
    connect(m_iconList, SIGNAL(browseIconLibrary()), this, SIGNAL(browseIconLibrary()));
    connect(m_iconList, SIGNAL(endIconList()), this, SIGNAL(browsePixmap()));
    connect(m_pathList, SIGNAL(pathSelected(int)), this, SIGNAL(pathSelected(int)));
}

CMindMapDtQuickTool::~CMindMapDtQuickTool()
{
	delete ui;
}

bool CMindMapDtQuickTool::support(const QList<CDiagramItem*> &items)
{
    return true;
}

void CMindMapDtQuickTool::reset()
{
    bool                            nonGroup = true;
	CMindMapNode					*n = NULL;
	QList<CDiagramItem*>			items;
	QList<CMindMapNode*>			nodes;

	if (!m_d)
		return;

	items = m_d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        if (dynamic_cast<CMindMapNodeGroup*>(item))
        {
            nonGroup = false;
            break;
        }
    }

	nodes = getAllSelectedNodes(m_d);
	if (nodes.length() > 1)
	{
        ui->m_sepLineOne->setVisible(false);
		ui->m_btnNew->setVisible(false);
        ui->m_btnLink->setVisible(false);
        ui->m_btnNote->setVisible(false);
        ui->m_btnAttach->setVisible(false);

        ui->m_sepLineTwo->setVisible(true);
        ui->m_btnBoundary->setVisible(true);
        ui->m_btnStyle->setVisible(true);
        ui->m_btnIcon->setVisible(true);
        ui->m_btnShape->setVisible(true);
        ui->m_btnPath->setVisible(true);
        ui->m_btnLayout->setVisible(true);
	}
	else
    {
		if (nodes.length() == 0)
		{
			ui->m_btnNew->setVisible(false);
			ui->m_sepLineOne->setVisible(false);
			ui->m_btnShape->setVisible(false);
			ui->m_btnPath->setVisible(false);
			ui->m_btnLayout->setVisible(false);
			ui->m_sepLineTwo->setVisible(false);
			ui->m_btnIcon->setVisible(false);
			ui->m_btnLink->setVisible(false);
			ui->m_btnNote->setVisible(false);
			ui->m_btnAttach->setVisible(false);
			ui->m_btnTask->setVisible(false);

            ui->m_btnStyle->setVisible(true);
            ui->m_btnBoundary->setVisible( false );
		}
		else
		{
            ui->m_btnNew->setVisible(true);
            ui->m_sepLineOne->setVisible(true);
            ui->m_btnShape->setVisible(true);
            ui->m_btnPath->setVisible(true);
            ui->m_btnLayout->setVisible(true);
            ui->m_sepLineTwo->setVisible(true);
            ui->m_btnIcon->setVisible(true);
            ui->m_btnLink->setVisible(true);
            ui->m_btnNote->setVisible(true);
            ui->m_btnAttach->setVisible(true);
            ui->m_btnTask->setVisible(true);

			if (nodes.first()->sprites(CMindMapNode::Link).length() > 0)
				ui->m_btnLink->setIcon( QIcon(":/res/icons/node-chain-edit.png") );
			else
				ui->m_btnLink->setIcon( QIcon(":/res/icons/node-chain.png") );
		}
	}
	
	n = dynamic_cast<CMindMapNode*>( items.first() );
	if (n)
	{
		m_newItemList->reset( n->isRoot() );
		m_layoutList->reset( n->isRoot() );
	}

	// activate the layout
	setVisible(false);
	layout()->invalidate();
	setVisible(true);
}

void CMindMapDtQuickTool::onNewNode()
{
    emit insertTopicChild();
}

void CMindMapDtQuickTool::onFillStyle()
{
    emit fillStyleSelected( m_styleGrid->defaultStyle() );
}

void CMindMapDtQuickTool::onInsertPositionSelected(int value, QIcon icon)
{
    CMindMapNode			*pressedNode = NULL;
	CMindMapNode			*parentNode = NULL;

	if (!m_d)
		return;

	pressedNode = dynamic_cast<CMindMapNode*>( m_d->selectedTopLevelItem() );
	if (!pressedNode) return;

	parentNode = pressedNode->upLevelNode();
    if (!parentNode && !pressedNode->isRoot())
        return;

	switch(value)
	{
	case CMindMapNode::InsertSiblingAfter:
        emit insertTopicAfter();
		break;

	case CMindMapNode::InsertSiblingBefore:
        emit insertTopicBefore();
		break;
	
	case CMindMapNode::InsertParent:
        emit insertTopicParent();
		break;
	
	case CMindMapNode::InsertChild:
	default:
        emit insertTopicChild();
		break;
	}; // End of switch

	ui->m_btnNew->setIcon( icon );
}

void CMindMapDtQuickTool::onShowIconMenu()
{
    QPoint      p;

    if (!m_d || !m_p)
        return;

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnIcon, (QWidget*)m_iconList);
    if (!p.isNull())
        m_iconList->exec( p );
}

void CMindMapDtQuickTool::onShowFillStyleMenu()
{
    QPoint      p;

    if (!m_d || !m_p)
        return;

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnStyle, (QWidget*)m_styleGrid);
    if (!p.isNull())
        m_styleGrid->exec(p);
}
