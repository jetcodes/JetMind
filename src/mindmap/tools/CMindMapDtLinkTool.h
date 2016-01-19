/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTLINKTOOL_H
#define PRODRAW_CMINDMAPDTLINKTOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>
#include "../CMindMapMenuManager.h"

namespace Ui {
    class CMindMapDtLinkTool;
}

class CMindMapDtLinkTool;
class CMindMapUiLinkEditor;
class CMindMapLinkSpriteMenu;

class CMindMapLinkSpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
	Q_OBJECT

public:
	CMindMapLinkSpriteMenu(QWidget *parent = NULL);
    virtual ~CMindMapLinkSpriteMenu();

	void reset(CMindMapNodeSprite *sprite);

Q_SIGNALS:
	void linkTriggered();
	void deleteLink();

private:
    Q_DISABLE_COPY(CMindMapLinkSpriteMenu)

}; // End of class CMindMapLinkSpriteMenu

class CMindMapDtLinkTool : public CDiagramTool
{
    Q_OBJECT
    
public:
    CMindMapDtLinkTool(QWidget *parent = 0);
    virtual ~CMindMapDtLinkTool();
    
    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onResetLink(int, QString);
    void onDeleteLink();
    void onShowLinkEditor();
	void onLinkTriggered();

private:
    Ui::CMindMapDtLinkTool *ui;

    CMindMapUiLinkEditor             *m_linkEditor;
	CMindMapLinkSpriteMenu			 *m_linkSpriteMenu;
}; // End of class CMindMapDtLinkTool

#endif // PRODRAW_CMINDMAPDTLINKTOOL_H
