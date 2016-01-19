/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramNode>
#include <core/CDiagramEditor>
#include <core/CDiagramCommands>

#include "../CMindMapNode.h"
#include "../CMindMapNodeSprite.h"
#include "../CMindMapCommands.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapMenuManager.h"
#include "CMindMapDtLinkTool.h"
#include "ui_CMindMapDtLinkTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapLinkSpriteMenu::CMindMapLinkSpriteMenu(QWidget *parent)
	: QMenu(parent)
{
}

CMindMapLinkSpriteMenu::~CMindMapLinkSpriteMenu()
{
}

void CMindMapLinkSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
	QAction				*act = NULL;
	QMap<int, QIcon>     icons;
	CMindMapLinkSprite	 *linkSprite = NULL;

	linkSprite = dynamic_cast<CMindMapLinkSprite*>(sprite);
	if (!linkSprite)
		return;

	// first, clear things up
	clear();

    icons[CMindMapLinkSprite::Web] = QIcon(":/mindstorm/icons/ui-globe.png");
    icons[CMindMapLinkSprite::Email] = QIcon(":/mindstorm/icons/ui-email.png");
    icons[CMindMapLinkSprite::Topic] = QIcon(":/mindstorm/icons/ui-topic.png");
    icons[CMindMapLinkSprite::SubGraph] = QIcon(":/mindstorm/icons/ui-sheet.png");

	// first, link trigger action
	act = addAction( icons[ linkSprite->linkType() ], linkSprite->link().toString() );
	connect(act, SIGNAL(triggered()), this, SIGNAL(linkTriggered()));

	// separator for good looking
    addSeparator();

	// second, remove action
    act = addAction( QIcon(":/mindstorm/icons/ui-delete.png"), tr("Remove link"));
	connect(act, SIGNAL(triggered()), this, SIGNAL(deleteLink()));

    CMindMapNodeSpriteMenu::reset(sprite);
}

CMindMapDtLinkTool::CMindMapDtLinkTool(QWidget *parent)
    : CDiagramTool(parent),
      m_linkEditor(NULL),
	  m_linkSpriteMenu(NULL)
{
    ui = new Ui::CMindMapDtLinkTool();
    ui->setupUi(this);

    m_linkEditor = new CMindMapUiLinkEditor(NULL);
    m_linkEditor->hide();

	m_linkSpriteMenu = new CMindMapLinkSpriteMenu(NULL);
	// register the menu to the manager
	CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::Link, m_linkSpriteMenu);

    connect(ui->m_btnBrowse, SIGNAL(clicked()), this, SLOT(onShowLinkEditor()));
    connect(ui->m_btnDeleteLink, SIGNAL(clicked()), this, SLOT(onDeleteLink()));
    connect(m_linkEditor, SIGNAL(deleteLink()), this, SLOT(onDeleteLink()));
    connect(m_linkEditor, SIGNAL(resetLink(int,QString)), this, SLOT(onResetLink(int,QString)));
	connect(m_linkSpriteMenu, SIGNAL(deleteLink()), this, SLOT(onDeleteLink()));
    connect(m_linkSpriteMenu, SIGNAL(linkTriggered()), this, SLOT(onLinkTriggered()));
}

CMindMapDtLinkTool::~CMindMapDtLinkTool()
{
    delete ui;
}

bool CMindMapDtLinkTool::support(const QList<CDiagramItem *> &items)
{
    Q_UNUSED(items)

    bool                    r = false;
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return r;

    nodes = getAllSelectedNodes(m_d);
    r = (nodes.length() == 1);

    return r;
}

void CMindMapDtLinkTool::reset()
{
    bool                            deleteEnabled = false;
    CMindMapNode                    *node = NULL;
    CMindMapLinkSprite              *linkSprite = NULL;
    QList<CMindMapNode*>            nodes;
    QList<CMindMapNodeSprite*>      sprites;

    if (!m_d)
        return;

    ui->m_btnBrowse->setIcon( QIcon(":/res/icons/node-chain.png") );
    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        node = nodes.first();
        if (node)
        {
            sprites = node->sprites( CMindMapNode::Link );
            if (sprites.length() == 1)
            {
                linkSprite = dynamic_cast<CMindMapLinkSprite*>(sprites.first());
                deleteEnabled = true;
                ui->m_btnBrowse->setIcon( QIcon(":/res/icons/node-chain-edit.png") );
            }
        }
    }

    ui->m_btnDeleteLink->setVisible(deleteEnabled);
}

void CMindMapDtLinkTool::onShowLinkEditor()
{
    if (!m_d || !m_linkEditor)
        return;

    m_linkEditor->reset(m_d);
    m_linkEditor->show();
}

void CMindMapDtLinkTool::onResetLink(int linkType, QString linkAddress)
{
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Link,
                                                          QVariant(linkType),
                                                          QVariant(linkAddress)) );
        ui->m_btnDeleteLink->setVisible(true);
        ui->m_btnBrowse->setIcon( QIcon(":/res/icons/node-chain-edit.png") );
    }
}

void CMindMapDtLinkTool::onDeleteLink()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Link,
                                                             true) );
        ui->m_btnDeleteLink->setVisible(false);
        ui->m_btnBrowse->setIcon( QIcon(":/res/icons/node-chain.png") );
    }
}

void CMindMapDtLinkTool::onLinkTriggered()
{
}
