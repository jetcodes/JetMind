/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTTASKTOOL_H
#define PRODRAW_CMINDMAPDTTASKTOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

#include "../CMindMapMenuManager.h"

namespace Ui {
class CMindMapDtTaskTool;
}

class CMindMapDtTaskTool;
class CMindMapUiTaskEditor;
class CMindMapUiDateEditor;

class CMindMapTaskPrioritySpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
	Q_OBJECT

public:
	CMindMapTaskPrioritySpriteMenu(QWidget *parent = NULL);
	virtual ~CMindMapTaskPrioritySpriteMenu();

	void reset(CMindMapNodeSprite *sprite);
	
Q_SIGNALS:
	void priorityChanged(int);
	void deletePrioritySprite();

public Q_SLOTS:
	void onActionTriggered(QAction*);

private:
    Q_DISABLE_COPY(CMindMapTaskPrioritySpriteMenu)

}; // End of class CMindMapTaskProgressSpriteMenu

class CMindMapTaskProgressSpriteMenu: public QMenu, public CMindMapNodeSpriteMenu
{
    Q_OBJECT

public:
    CMindMapTaskProgressSpriteMenu(QWidget *parent = NULL);
    virtual ~CMindMapTaskProgressSpriteMenu();

    void reset(CMindMapNodeSprite *sprite);

Q_SIGNALS:
    void progressChanged(int);
    void deleteProgressSprite();

public Q_SLOTS:
    void onActionTriggered(QAction*);

private:
    Q_DISABLE_COPY(CMindMapTaskProgressSpriteMenu)

}; // End of class CMindMapTaskProgressSpriteMenu

class CMindMapDtTaskTool : public CDiagramTool
{
    Q_OBJECT
    
public:
    CMindMapDtTaskTool(QWidget *parent = 0);
    virtual ~CMindMapDtTaskTool();
    
    bool support(const QList<CDiagramItem *> &items);
    void reset();

public Q_SLOTS:
    void onShowTaskEditor();
    void onShowDateEditor();
    void onClearTask();
    void onAssignedTo(QString);
	void onDateSelected(QString);
    void onDueDateChanged(QString);
    void onPriorityChanged(int);
    void onPriorityActivated(int);
    void onProgressChanged(int);
    void onProgressActivated(int);
	void onDeletePrioritySprite();
    void onDeleteProgressSprite();
    void onResetTask(int, int, QString, QString, bool);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::CMindMapDtTaskTool          *ui;

    CMindMapUiTaskEditor            *m_taskEditor;
    CMindMapUiDateEditor            *m_dateEditor;
	CMindMapTaskPrioritySpriteMenu	*m_taskPrioritySpriteMenu;
    CMindMapTaskProgressSpriteMenu  *m_taskProgressSpriteMenu;

}; // End of class CMindMapDtTaskTool

#endif // PRODRAW_CMINDMAPDTTASKTOOL_H
