/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramCommands>

#include "../CMindMapNode.h"
#include "../CMindMapMenuManager.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "../CMindMapNodeGroup.h"
#include "../CMindMapNodeSprite.h"
#include "CMindMapDtGroupTool.h"
#include "ui_CMindMapDtGroupTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

bool nodeOrderLessThan(CMindMapNode *n1, CMindMapNode *n2)
{
    bool    r = false;

    r = (n1->index() < n2->index());

    return r;
}

CMindMapNode* checkGroupable(CDiagram *d, int &startIndex, int &endIndex, int groupType)
{
    int                         levelTemp = -1, level = -1;
    bool                        r = true;
    CMindMapNode                *node = NULL, *parentNode = NULL;
    QList<CMindMapNode*>        nodesTemp, nodesFinal;
    QList<CDiagramItem*>        items;

    if (!d)
        return false;

    // firstly, filter the mindmpa nodes
    items = d->selectedTopLevelItems();
    foreach (CDiagramItem *item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node)
        {
            levelTemp = node->level();
            if (levelTemp < level || level == -1)
                level = levelTemp;
            if (levelTemp <= level)
                nodesTemp.push_back(node);
        }
    }

    // second, filter the nodes with the minimal level
    // numbers
    foreach (node, nodesTemp)
    {
        if (node->level() == level)
        {
            nodesFinal.push_back(node);
            if (parentNode == NULL)
                parentNode = node->upLevelNode();
        }
    }

    // sort the filtered nodes according to the
    // index, from the smallest to the largest.
    qStableSort(nodesFinal.begin(), nodesFinal.end(), nodeOrderLessThan);
    if (nodesFinal.length() > 0)
    {
        startIndex = nodesFinal.first()->index();
        endIndex = nodesFinal.last()->index();
    }

    // check whether all the filtered & sorted nodes are right next
    // to each other.
    for (int i = 0; i < nodesFinal.length() - 1; ++i)
    {
        if (nodesFinal.at(i)->index() + 1 != nodesFinal.at(i + 1)->index())
        {
            r = false;
            break;
        }
    }

    // if so, and we are not in the root node, then
    // we are good to go to add groups
    if (r && parentNode)
        r = !parentNode->checkExistedGroup(startIndex, endIndex, groupType);
    parentNode = r ? parentNode : NULL;

    return parentNode;
}

CMindMapNodeBoundaryMenu::CMindMapNodeBoundaryMenu(QWidget *parent)
    : QMenu(parent)
{

}

CMindMapNodeBoundaryMenu::~CMindMapNodeBoundaryMenu()
{

}

void CMindMapNodeBoundaryMenu::reset(CMindMapNodeGroup *group)
{
    QAction     *act = NULL;

    if (!group)
        return;

    clear();
    act = addAction(QIcon(":/mindstorm/icons/ui-delete.png"),
                    tr("Remove Boundary"),
                    this,
                    SIGNAL(dropBoundary()));
}

CMindMapDtGroupTool::CMindMapDtGroupTool(QWidget *parent)
    : CDiagramTool(parent),
      m_boundaryMenu(NULL)
{
    ui = new Ui::CMindMapDtGroupTool();
    ui->setupUi(this);

    m_boundaryMenu = new CMindMapNodeBoundaryMenu(NULL);
    CMindMapMenuManager::instance()->setNodeGroupMenu(CMindMapNode::Boundary, m_boundaryMenu);

    connect(ui->m_btnBoundary, SIGNAL(clicked()), this, SLOT(onAddBoundary()));
    connect(ui->m_btnDropBoundary, SIGNAL(clicked()), this, SLOT(onDropBoundary()));
    connect(m_boundaryMenu, SIGNAL(dropBoundary()), this, SLOT(onDropBoundary()));
}

CMindMapDtGroupTool::~CMindMapDtGroupTool()
{
    delete ui;
}

bool CMindMapDtGroupTool::support(const QList<CDiagramItem *> &items)
{
    bool                        r = false;
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return r;

    nodes = getAllSelectedNodes(m_d);
    r = (nodes.length() > 0);

    if (!r)
    {
        if (items.length() == 1)
        {
            r = (dynamic_cast<CMindMapNodeGroup*>(items.first()) != NULL);
        }
    }

    return r;
}

void CMindMapDtGroupTool::reset()
{
    bool                        groupExisted = false;
    CMindMapNodeGroup           *group = NULL;
    QList<CDiagramItem*>        items;

    if (!m_d)
        return;

    ui->m_btnDropBoundary->setVisible(false);
    items = m_d->selectedTopLevelItems();
    if (items.length() == 1)
    {
        group = dynamic_cast<CMindMapNodeGroup*>(items.first());
        if (group)
        {
            ui->m_btnDropBoundary->setVisible(true);
            groupExisted = true;
        }
    }

    ui->m_btnBoundary->setVisible(!groupExisted);
}

void CMindMapDtGroupTool::onAddBoundary()
{
    int                 startIndex = -1, endIndex = -1;
    CMindMapNode        *parentNode = NULL;

    if (!m_d)
        return;

    parentNode = checkGroupable(m_d, startIndex, endIndex, CMindMapNode::Boundary);
    if (parentNode)
    {
        m_d->addUndoCommand( new CMindMapAddGroupCommand(m_d,
                                                         parentNode,
                                                         startIndex,
                                                         endIndex,
                                                         CMindMapNode::Boundary,
                                                         CMindMapNodeBoundary::RounededRect,
                                                         NULL));
        ui->m_btnBoundary->setVisible(false);
        ui->m_btnDropBoundary->setVisible(true);
    }
}

void CMindMapDtGroupTool::onDropBoundary()
{
    CMindMapNodeGroup           *group = NULL;
    QList<CDiagramItem*>        items;

    if (!m_d)
        return;

    items = m_d->selectedTopLevelItems();
    if (items.length() == 1)
    {
        group = dynamic_cast<CMindMapNodeGroup*>(items.first());
        if (group)
        {
            m_d->addUndoCommand( new CMindMapDropGroupCommand(m_d,
                                                              group,
                                                              NULL));
            ui->m_btnDropBoundary->setVisible(false);
            ui->m_btnBoundary->setVisible(true);
        }
    }
}
