/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramCommands>
#include <core/CDiagramClipboard>

#include "../CMindMapNode.h"
#include "../CMindMapNodeGroup.h"
#include "../CMindMapCommands.h"
#include "../CMindMapUiItem.h"
#include "CMindMapDtEditTool.h"
#include "ui_CMindMapDtEditTool.h"

extern QList<CMindMapNode*>	getAllSelectedNodes(CDiagram *d);

void selectAllSubNodes(CMindMapNode *node)
{
    CMindMapNode            *n = NULL;
    QStack<CMindMapNode*>   nodes;
    QList<CMindMapNode*>    childNodes;

    if (!node)
        return;

    nodes.push(node);
    while (!nodes.isEmpty())
    {
        n = nodes.pop();
        if (!n->isSelected())
            n->setSelected(true);

        childNodes = n->childs();
        foreach (n, childNodes)
        {
            nodes.push(n);
        }
    }
}

CMindMapDtEditTool::CMindMapDtEditTool(QWidget *parent)
	: CDiagramTool(parent)
{
	QClipboard *clipboard = QApplication::clipboard();

    ui = new Ui::CMindMapDtEditTool();
	ui->setupUi(this);

    connect(ui->m_btnCopy, SIGNAL(clicked()), this, SLOT(onCopy()));
	connect(ui->m_btnCut, SIGNAL(clicked()), this, SLOT(onCut()));
	connect(ui->m_btnPaste, SIGNAL(clicked()), this, SLOT(onPaste()));
	connect(ui->m_btnRedo, SIGNAL(clicked()), this, SLOT(onRedo()));
	connect(ui->m_btnUndo, SIGNAL(clicked()), this, SLOT(onUndo()));
	connect(ui->m_btnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(onClipboardChanged(QClipboard::Mode)));
}

CMindMapDtEditTool::~CMindMapDtEditTool()
{
	delete ui;
}

bool CMindMapDtEditTool::support(const QList<CDiagramItem*> &items)
{
    return true;
}

void CMindMapDtEditTool::init(CDiagram *d)
{
	if (!d)
		return;

	CDiagramTool::init(d);
	connect(d->undoStack(), SIGNAL(canRedoChanged(bool)), this, SLOT(onCanRedoChanged(bool)));
	connect(d->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(onCanUndoChanged(bool)));
}

void CMindMapDtEditTool::uninit(CDiagram *d)
{
	if (!d || d != m_d)
		return;

	disconnect(m_d->undoStack(), SIGNAL(canRedoChanged(bool)), this, SLOT(onCanRedoChanged(bool)));
	disconnect(m_d->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(onCanUndoChanged(bool)));

	CDiagramTool::uninit(d);
}

void CMindMapDtEditTool::reset()
{
    bool                    nodesSelected = false;
    bool                    clipboardNonEmpty = false;
    QList<CMindMapNode*>    nodes;
    CDiagramClipboard       *clipboard = NULL;

	if (!m_d)
		return;

    clipboard = CDiagramClipboard::clipboard();
    if (clipboard)
        clipboardNonEmpty = clipboard->isNonEmpty();
    nodes = getAllSelectedNodes(m_d);
    nodesSelected = (nodes.length() > 0);

    ui->m_btnCopy->setEnabled(nodesSelected);
    ui->m_btnCut->setEnabled(nodesSelected);
	ui->m_btnRedo->setEnabled(m_d->undoStack()->canRedo());
	ui->m_btnUndo->setEnabled(m_d->undoStack()->canUndo());
    ui->m_btnPaste->setEnabled(clipboardNonEmpty);
}

void CMindMapDtEditTool::onCopy()
{
	CMindMapNode			*node = NULL;
    QList<CDiagramItem*>	items;
    QList<CMindMapNode*>	nodes;

	if (!m_d)
		return;

    items = m_d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node && !node->isRoot())
        {
            nodes.push_back(node);
        }
        else
        {
            item->setSelected(false);
        }
    }

	if (nodes.length() > 0)
	{
		m_d->copy();
	}
}

void CMindMapDtEditTool::onClone()
{

}

void CMindMapDtEditTool::onCut()
{
	CMindMapNode			*node = NULL;
    QList<CDiagramItem*>	items;
    QList<CMindMapNode*>	nodes;

	if (!m_d)
		return;

    items = m_d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node && !node->isRoot())
        {
            nodes.push_back(node);
        }
        else
        {
            item->setSelected(false);
        }
    }

	if (nodes.length() > 0)
	{
        foreach (node, nodes)
        {
            selectAllSubNodes(node);
        }

		m_d->copy();
        m_d->addUndoCommand(new CMindMapDelNodeCommand(m_d, nodes, 0) );
	}
}

void CMindMapDtEditTool::onPaste()
{
	QList<CMindMapNode*>	nodes;

	if (!m_d) 
		return;
	
	nodes = getAllSelectedNodes(m_d);
	if (nodes.length() == 1)
	{
		m_d->addUndoCommand(new CMindMapPasteCommand(m_d, nodes));
	}
}

void CMindMapDtEditTool::onRedo()
{
	if (!m_d) return;
	m_d->undoStack()->redo();
}

void CMindMapDtEditTool::onUndo()
{
	if (!m_d) return;
	m_d->undoStack()->undo();
}

void CMindMapDtEditTool::onDelete()
{
    CDiagramCommand             *cmd = NULL, *parentCmd = NULL;
    CMindMapNode                *node = NULL;
    CMindMapNodeGroup           *group = NULL;
    CDiagramTextNode            *text = NULL;
    QList<CDiagramItem*>        items;
    QList<CMindMapNode*>        nodes;
    QList<CMindMapNodeGroup*>   groups;
    QList<CDiagramItem*>        texts;

    items = m_d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        text = dynamic_cast<CDiagramTextNode*>(item);
        group = dynamic_cast<CMindMapNodeGroup*>(item);
        if (node)
        {
            if (!node->isRoot())
                nodes.push_back(node);
        }
        else if (text)
        {
            texts.push_back(item);
        }
        else if (group)
        {
            groups.push_back(group);
        }
    }

    if (m_d)
    {
        // delete node groups
        if (groups.length() > 0)
        {
            cmd = new CMindMapDropGroupCommand(m_d, groups);
            parentCmd = cmd;
        }

        // delete free texts
        if (texts.length() > 0)
        {
            if (cmd)
            {
                new CDeleteItemCommand(m_d, texts, parentCmd);
            }
            else
            {
                cmd = new CDeleteItemCommand(m_d, texts);
                parentCmd = cmd;
            }
        }

        // delete topic nodes
        if (nodes.length() > 0)
        {
            if (cmd)
                new CMindMapDelNodeCommand(m_d, nodes, parentCmd);
            else
                cmd = new CMindMapDelNodeCommand(m_d, nodes);
        }

        m_d->addUndoCommand(cmd);
    }
}

void CMindMapDtEditTool::onClipboardChanged(QClipboard::Mode mode)
{
	QClipboard	*clipboard = QApplication::clipboard();

	if (mode == QClipboard::Clipboard)
	{
		if (clipboard->text() != QString("") ||
			!clipboard->image().isNull() ||
			!clipboard->pixmap().isNull())
		{
			ui->m_btnPaste->setEnabled(true);
		}
		else
		{
			ui->m_btnPaste->setEnabled(false);
		}
	}
}

void CMindMapDtEditTool::onCanRedoChanged(bool value)
{
	ui->m_btnRedo->setEnabled(value);
}

void CMindMapDtEditTool::onCanUndoChanged(bool value)
{
	ui->m_btnUndo->setEnabled(value);
}
