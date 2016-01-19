/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramContext>
#include <core/CDiagramContextMenuHandler>
#include <mindmap/CMindMapFile>
#include <mindmap/CMindMapNode>

#include "CActionManager.h"
#include "CGraphMenu.h"
#include "CWorkbench.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CGraphOperationMenu::CGraphOperationMenu(CWorkbench *bench)
    : CDiagramContextMenu(),
      m_bench(bench),
      m_d(NULL)
{

}

CGraphOperationMenu::~CGraphOperationMenu()
{

}

bool CGraphOperationMenu::support(CDiagram *d)
{
    bool    r = false;

    if (d && d->mode() != CDiagram::TagMode)
        r = true;

    return r;
}

void CGraphOperationMenu::reset(CDiagram *d)
{
    m_d = d;
}

void CGraphOperationMenu::apply(QMenu *menu)
{
    CActionManager          *am = NULL;
    QList<CMindMapNode*>    nodes;

    if (!menu || !m_bench || !m_d)
        return;

    am = m_bench->actionManager();
    if (!am)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (!menu->isEmpty())
        menu->addSeparator();
    menu->addAction(am->m_actUndo);
    menu->addAction(am->m_actRedo);
    if (nodes.length() == 0)
    {
        menu->addSeparator();
        menu->addAction(am->m_actExport);
        menu->addAction(am->m_actPrint);
        menu->addSeparator();
        menu->addAction(am->m_actBrainStorm);
        menu->addAction(am->m_actPresentation);
        // menu->addSeparator();
        // menu->addAction(am->m_actSaveAsTemplate);
    }
    else
    {
        // EMPTY
    }
}

CGraphDemoMenu::CGraphDemoMenu(CWorkbench *bench)
    : CDiagramContextMenu(),
      m_bench(bench),
      m_d(NULL)
{

}

CGraphDemoMenu::~CGraphDemoMenu()
{

}

bool CGraphDemoMenu::support(CDiagram *d)
{
    bool    r = false;

    if (d && d->mode() == CDiagram::TagMode)
        r = true;

    return r;
}

void CGraphDemoMenu::reset(CDiagram *d)
{
    m_d = d;
}

void CGraphDemoMenu::apply(QMenu *menu)
{
    CActionManager          *am = NULL;

    if (!menu || !m_bench || !m_d)
        return;

    am = m_bench->actionManager();
    if (!am)
        return;

    if (!menu->isEmpty())
        menu->addSeparator();

    menu->addAction(am->m_actPresentation);
}
