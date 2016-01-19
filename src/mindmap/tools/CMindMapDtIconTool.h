/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTICONTOOL_H
#define PRODRAW_CMINDMAPDTICONTOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>
#include "../CMindMapUiItem.h"
#include "../CMindMapMenuManager.h"

namespace Ui {class CMindMapDtIconTool;}

class CMindMapUiIconList;
class CMindMapUiPixmapList;
class CMindMapUiIconBrowser;

class CMindMapIconSpriteMenu: public CMindMapUiIconAlterList, public CMindMapNodeSpriteMenu
{
public:
	CMindMapIconSpriteMenu(QWidget *parent = NULL);
	virtual ~CMindMapIconSpriteMenu();

	void reset(CMindMapNodeSprite *sprite);

private:
    Q_DISABLE_COPY(CMindMapIconSpriteMenu)

}; // End of class CMindMapIconSpriteMenu

class CMindMapPixmapSpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
    Q_OBJECT

public:
    CMindMapPixmapSpriteMenu(QWidget *parent = NULL);
    virtual ~CMindMapPixmapSpriteMenu();

    void reset(CMindMapNodeSprite *sprite);

Q_SIGNALS:
    void deletePixmap();
    void restorePixmapSize();
    void alterPixmap();

private:
    Q_DISABLE_COPY(CMindMapPixmapSpriteMenu)

}; // End of class CMindMapPixmapSpriteMenu

class CMindMapDtIconTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtIconTool(QWidget *parent = 0);
	virtual ~CMindMapDtIconTool();

    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onShowIconList();
    void onBrowseIconLibrary();
    void onIconSelected(const QIcon &icon, int index);
	void onShowPixmapList();
	void onPixmapSelected(const QString &filePath);
	void onBrowsePixmap();
    void onDeleteAllIcons();
    void onDeleteIcon(int value);
    void onDeletePixmap();
    void onRestorePixmapSize();
	
private:
	Ui::CMindMapDtIconTool *ui;

	CMindMapUiIconList			*m_iconList;
    CMindMapUiIconBrowser       *m_iconBrowser;
    CMindMapUiPixmapList		*m_pixmapList;
	CMindMapIconSpriteMenu		*m_iconSpriteMenu;
    CMindMapPixmapSpriteMenu    *m_pixmapSpriteMenu;

}; // End of class CMindMapDtIconTool

#endif // PRODRAW_CMINDMAPDTICONTOOL_H
