/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTNOTETOOL_H
#define PRODRAW_CMINDMAPDTNOTETOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>
#include "../CMindMapMenuManager.h"

namespace Ui {
    class CMindMapDtNoteTool;
}

class CMindMapNodeSprite;
class CMindMapNodeSpriteMenu;
class CMindMapUiNoteEditor;

class CMindMapNoteSpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
	Q_OBJECT

public:
	CMindMapNoteSpriteMenu(QWidget *parent);
	virtual ~CMindMapNoteSpriteMenu();

	void reset(CMindMapNodeSprite* sprite);

Q_SIGNALS:
	void deleteNote();
	void showNoteEditor();

private:
    Q_DISABLE_COPY(CMindMapNoteSpriteMenu)

}; // End of class CMindMapNoteSpriteMenu

class CMindMapDtNoteTool : public CDiagramTool
{
    Q_OBJECT
    
public:
    CMindMapDtNoteTool(QWidget *parent = 0);
    virtual ~CMindMapDtNoteTool();
    
    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onResetNote(const QString &value);
    void onDeleteNote();
	void onShowNoteEditor();

private:
    Ui::CMindMapDtNoteTool      *ui;

    CMindMapUiNoteEditor        *m_noteEditor;
	CMindMapNoteSpriteMenu		*m_noteSpriteMenu;

}; // End of class CMindMapDtNoteTool

#endif // PRODRAW_CMINDMAPDTNOTETOOL_H
