/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtNewTool.h"
#include "ui_CMindMapDtNewTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapDtNewTool::CMindMapDtNewTool(QWidget *parent)
	: CDiagramTool(parent)
{
	ui = new Ui::CMindMapDtNewTool();
	ui->setupUi(this);

	connect(ui->m_btnInsertBefore, SIGNAL(clicked()), this, SLOT(onInsertBefore()));
	connect(ui->m_btnInsertAfter, SIGNAL(clicked()), this, SLOT(onInsertAfter()));
	connect(ui->m_btnInsertParent, SIGNAL(clicked()), this, SLOT(onInsertParent()));
	connect(ui->m_btnInsertChild, SIGNAL(clicked()), this, SLOT(onInsertChild()));
}

CMindMapDtNewTool::~CMindMapDtNewTool()
{
	delete ui;
}

bool CMindMapDtNewTool::support(const QList<CDiagramItem*> &items)
{
    bool						r = false;
	QList<CMindMapNode*>		nodes;

    if (!m_d)
        return r;

    nodes = getAllSelectedNodes(m_d);
    r = (nodes.length() == 1);

	return r;
}

void CMindMapDtNewTool::reset()
{
	CMindMapNode				*node = NULL;
	QList<CDiagramItem*>		items;

	if (!m_d)
		return;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	node = dynamic_cast<CMindMapNode*>(items.first());
	if (!node)
		return;
	
	if (node->isRoot())
	{
		ui->m_btnInsertAfter->hide();
		ui->m_btnInsertBefore->hide();
		ui->m_btnInsertParent->hide();
	}
	else
	{
		ui->m_btnInsertAfter->show();
		ui->m_btnInsertBefore->show();
		ui->m_btnInsertParent->show();
	}
}

void CMindMapDtNewTool::onInsertBefore()
{
    CMindMapNode			*pressedNode = NULL;
    CMindMapNode			*parentNode = NULL;
	QList<CDiagramItem*>	items;
    bool                    duplicateStyle = false;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	pressedNode = dynamic_cast<CMindMapNode*>(items.first());
	if (!pressedNode)
		return;

    parentNode = pressedNode->upLevelNode();
    if (parentNode)
    {
        duplicateStyle = (QApplication::keyboardModifiers() & Qt::Key_Alt);
        m_d->addUndoCommand(new CMindMapNewNodeCommand(m_d,
                                                       parentNode,
                                                       pressedNode,
                                                       true,
                                                       duplicateStyle));
    }
}

void CMindMapDtNewTool::onInsertAfter()
{
    CMindMapNode			*pressedNode = NULL;
	CMindMapNode			*parentNode = NULL;
	QList<CDiagramItem*>	items;
    bool                    duplicateStyle = false;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	pressedNode = dynamic_cast<CMindMapNode*>(items.first());
	if (!pressedNode)
		return;

    parentNode = pressedNode->upLevelNode();
    if (parentNode)
    {
        duplicateStyle = (QApplication::keyboardModifiers() & Qt::Key_Alt);
        m_d->addUndoCommand(new CMindMapNewNodeCommand(m_d,
                                                       parentNode,
                                                       pressedNode,
                                                       false,
                                                       duplicateStyle));
    }
}

void CMindMapDtNewTool::onInsertParent()
{
	CMindMapNode			*pressedNode = NULL;
	CMindMapNode			*parentNode = NULL, *newParentNode = NULL;
	QList<CDiagramItem*>	items;
    bool                    duplicateStyle = false;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	pressedNode = dynamic_cast<CMindMapNode*>(items.first());
	if (!pressedNode)
		return;

	parentNode = pressedNode->upLevelNode();
	if (!parentNode)
		return;

	newParentNode = parentNode->createChildNode();
	if (!newParentNode)
		return;

    duplicateStyle = (QApplication::keyboardModifiers() & Qt::Key_Alt);
	m_d->addUndoCommand( new CMindMapInsertParentCommand(m_d, pressedNode, newParentNode, 0) );
}

void CMindMapDtNewTool::onInsertChild()
{
    CMindMapNode			*pressedNode = NULL;
	QList<CDiagramItem*>	items;
    bool                    duplicateStyle = false;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	pressedNode = dynamic_cast<CMindMapNode*>(items.first());
	if (!pressedNode)
		return;

    duplicateStyle = (QApplication::keyboardModifiers() & Qt::Key_Alt);
    m_d->addUndoCommand(new CMindMapNewNodeCommand(m_d,
                                                   pressedNode,
                                                   NULL,
                                                   false,
                                                   duplicateStyle));
}
