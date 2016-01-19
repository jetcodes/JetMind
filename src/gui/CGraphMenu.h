/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CGRAPHMENU_H
#define PRODRAW_CGRAPHMENU_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramContextMenuHandler>

class CWorkbench;

class CGraphOperationMenu : public CDiagramContextMenu
{
    Q_OBJECT

public:
    CGraphOperationMenu(CWorkbench *bench);
    virtual ~CGraphOperationMenu();

    bool support(CDiagram *d);
    void reset(CDiagram *d);
    void apply(QMenu *menu);

protected:
    Q_DISABLE_COPY(CGraphOperationMenu)

    CWorkbench      *m_bench;
    CDiagram        *m_d;

}; // End of class CGraphOperationMenu

class CGraphDemoMenu : public CDiagramContextMenu
{
    Q_OBJECT

public:
    CGraphDemoMenu(CWorkbench *bench);
    virtual ~CGraphDemoMenu();

    bool support(CDiagram *d);
    void reset(CDiagram *d);
    void apply(QMenu *menu);

protected:
    Q_DISABLE_COPY(CGraphDemoMenu)

    CWorkbench      *m_bench;
    CDiagram        *m_d;

}; // End of class CGraphDemoMenu

#endif // PRODRAW_CGRAPHMENU_H
