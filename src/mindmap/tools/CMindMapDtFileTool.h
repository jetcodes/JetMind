/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTFILETOOL_H
#define PRODRAW_CMINDMAPDTFILETOOL_H

#include <QtGui>
#include <core/CDiagramWidget>

#include "../CMindMapUiItem.h"
#include "../CMindMapMenuManager.h"

namespace Ui {
class CMindMapDtFileTool;
}

class CMindMapFileSpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
    Q_OBJECT

public:
    CMindMapFileSpriteMenu(QWidget *parent = NULL);
    virtual ~CMindMapFileSpriteMenu();

    void reset(CMindMapNodeSprite *sprite);

Q_SIGNALS:
    void attachFile();
    void removeAttach();
    void deleteAttachedFile();
    void viewFile(QString);

public Q_SLOTS:
    void onFileClicked(QAction *act);

private:
    Q_DISABLE_COPY(CMindMapFileSpriteMenu)

    QAction         *m_actAttachFile;
    QAction         *m_actDeleteAttachedFile;
    QAction         *m_actRemoveAttach;
}; // End of class CMindMapIconSpriteMenu

class CMindMapDtFileTool : public CDiagramTool
{
    Q_OBJECT
    
public:
    CMindMapDtFileTool(QWidget *parent = 0);
    virtual ~CMindMapDtFileTool();
    
    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onAttachFile();
    void onShowFileDropper();
    void onDeleteAttachedFiles(const QStringList &files);
    void onRemoveAttach();
    void onViewFile(QString filePath);

private:
    Ui::CMindMapDtFileTool *ui;

    CMindMapFileSpriteMenu          *m_fileSpriteMenu;
    CMindMapUiFileDropper           *m_fileDropper;

}; // End of class CMindMapDtFileTool

#endif // PRODRAW_CMINDMAPDTFILETOOL_H
