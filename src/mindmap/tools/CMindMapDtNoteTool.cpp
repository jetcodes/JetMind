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
#include "CMindMapDtNoteTool.h"
#include "ui_CMindMapDtNoteTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapNoteSpriteMenu::CMindMapNoteSpriteMenu(QWidget *parent)
	: QMenu(parent)
{

}

CMindMapNoteSpriteMenu::~CMindMapNoteSpriteMenu()
{
}

void CMindMapNoteSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
    QAction					*act = NULL;
	CMindMapNoteSprite		*noteSprite = NULL;

	noteSprite = dynamic_cast<CMindMapNoteSprite*>( sprite );
	if (!noteSprite)
		return;

    clear();
	
	act = addAction(tr("Edit Note"));
	connect(act, SIGNAL(triggered()), this, SIGNAL(showNoteEditor()));

    act = addAction(QIcon(":/mindstorm/icons/ui-delete.png"), tr("Delete Note"));
	connect(act, SIGNAL(triggered()), this, SIGNAL(deleteNote()));

    CMindMapNodeSpriteMenu::reset(sprite);
}

CMindMapDtNoteTool::CMindMapDtNoteTool(QWidget *parent)
    : CDiagramTool(parent),
      m_noteEditor(NULL),
	  m_noteSpriteMenu(NULL)
{
    ui = new Ui::CMindMapDtNoteTool();
    ui->setupUi(this);

    m_noteEditor = new CMindMapUiNoteEditor(NULL);
    m_noteEditor->hide();

	m_noteSpriteMenu = new CMindMapNoteSpriteMenu(NULL);
	// register the menu to the manager
	CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::Note, m_noteSpriteMenu);

    connect(ui->m_btnAddNote, SIGNAL(clicked()), this, SLOT(onShowNoteEditor()));
    connect(ui->m_btnDropNote, SIGNAL(clicked()), this, SLOT(onDeleteNote()));
    connect(m_noteEditor, SIGNAL(deleteNote()), this, SLOT(onDeleteNote()));
    connect(m_noteEditor, SIGNAL(resetNote(QString)), this, SLOT(onResetNote(QString)));
	connect(m_noteSpriteMenu, SIGNAL(deleteNote()), this, SLOT(onDeleteNote()));
	connect(m_noteSpriteMenu, SIGNAL(showNoteEditor()), this, SLOT(onShowNoteEditor()));
}

CMindMapDtNoteTool::~CMindMapDtNoteTool()
{
    delete ui;
}

bool CMindMapDtNoteTool::support(const QList<CDiagramItem *> &items)
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

void CMindMapDtNoteTool::reset()
{
    bool                            deleteEnabled = false;
    CMindMapNode                    *node = NULL;
    CMindMapNoteSprite              *noteSprite = NULL;
    QList<CMindMapNode*>            nodes;
    QList<CMindMapNodeSprite*>      sprites;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        node = nodes.first();
        if (node)
        {
            sprites = node->sprites( CMindMapNode::Note );
            if (sprites.length() == 1)
            {
                noteSprite = dynamic_cast<CMindMapNoteSprite*>(sprites.first());
                deleteEnabled = true;
            }
        }
    }

    ui->m_btnDropNote->setVisible(deleteEnabled);
}

void CMindMapDtNoteTool::onShowNoteEditor()
{
    if (!m_d || !m_noteEditor)
        return;

    if ( m_noteEditor->reset(m_d) )
        m_noteEditor->show();
}

void CMindMapDtNoteTool::onResetNote(const QString &value)
{
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Note,
                                                          QVariant(value)) );
        ui->m_btnDropNote->setVisible(true);
    }
}

void CMindMapDtNoteTool::onDeleteNote()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Note,
                                                             true) );
        ui->m_btnDropNote->setVisible(false);
    }
}
