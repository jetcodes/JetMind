/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTGROUPTOOL_H
#define PRODRAW_CMINDMAPDTGROUPTOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>
#include "../CMindMapMenuManager.h"

namespace Ui {
    class CMindMapDtGroupTool;
}

class CMindMapNodeBoundaryMenu : public QMenu, public CMindMapNodeGroupMenu
{
    Q_OBJECT

public:
    CMindMapNodeBoundaryMenu(QWidget *parent = NULL);
    virtual ~CMindMapNodeBoundaryMenu();

    void reset(CMindMapNodeGroup *group);

Q_SIGNALS:
    void dropBoundary();
    void changeBoundaryStyle(int);

private:
    Q_DISABLE_COPY(CMindMapNodeBoundaryMenu)

}; // End of class CMindMapNodeBoundaryMenu

class CMindMapDtGroupTool : public CDiagramTool
{
    Q_OBJECT
    
public:
    explicit CMindMapDtGroupTool(QWidget *parent = 0);
    virtual ~CMindMapDtGroupTool();
    
    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onAddBoundary();
    void onDropBoundary();

private:
    Ui::CMindMapDtGroupTool *ui;

    CMindMapNodeBoundaryMenu       *m_boundaryMenu;
}; // End of class CMindMapDtGroupTool

#endif // PRODRAW_CMINDMAPDTGROUPTOOL_H
