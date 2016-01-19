/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>

#include "../CMindMapNode.h"
#include "../CMindMapCommands.h"
#include "../CMindMapNodeSprite.h"
#include "../CMindMapUiItem.h"
#include "CMindMapDtTaskTool.h"
#include "ui_CMindMapDtTaskTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapTaskPrioritySpriteMenu::CMindMapTaskPrioritySpriteMenu(QWidget *parent)
	: QMenu(parent)
{
}

CMindMapTaskPrioritySpriteMenu::~CMindMapTaskPrioritySpriteMenu()
{
}

void CMindMapTaskPrioritySpriteMenu::reset(CMindMapNodeSprite *sprite)
{
	QAction							*act = NULL;
	QActionGroup					*actGroup = NULL;
	CMindMapTaskPrioritySprite		*taskPrioritySprite = NULL;

	taskPrioritySprite = dynamic_cast<CMindMapTaskPrioritySprite*>( sprite );
	if (!taskPrioritySprite)
		return;

    clear();
	
	actGroup = new QActionGroup(this);
	actGroup->setExclusive(false);

	act = addAction(tr("Priority 0"));
	act->setData(QVariant(0));
	actGroup->addAction(act);

	act = addAction(tr("Priority 1"));
	act->setData(QVariant(1));
	actGroup->addAction(act);

	act = addAction(tr("Priority 2"));
	act->setData(QVariant(2));
	actGroup->addAction(act);

	act = addAction(tr("Priority 3"));
	act->setData(QVariant(3));
	actGroup->addAction(act);

	act = addAction(tr("Priority 4"));
	act->setData(QVariant(4));
	actGroup->addAction(act);

	act = addAction(tr("Priority 5"));
	act->setData(QVariant(5));
	actGroup->addAction(act);
	
	act = addAction(tr("Priority 6"));
	act->setData(QVariant(6));
	actGroup->addAction(act);
	
	connect(actGroup, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
	addSeparator();

    act = addAction(QIcon(":/mindstorm/icons/ui-delete.png"), tr("Delete Priority Sprite"));
	connect(act, SIGNAL(triggered()), this, SIGNAL(deletePrioritySprite()));

    CMindMapNodeSpriteMenu::reset(sprite);
}

void CMindMapTaskPrioritySpriteMenu::onActionTriggered(QAction *act)
{
	int		r = 0;

	r = act->data().toInt();
	emit priorityChanged(r);
}

CMindMapTaskProgressSpriteMenu::CMindMapTaskProgressSpriteMenu(QWidget *parent)
    : QMenu(parent)
{
}

CMindMapTaskProgressSpriteMenu::~CMindMapTaskProgressSpriteMenu()
{
}

void CMindMapTaskProgressSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
    QAction							*act = NULL;
    QActionGroup					*actGroup = NULL;
    CMindMapTaskProgressSprite		*taskProgressSprite = NULL;

    taskProgressSprite = dynamic_cast<CMindMapTaskProgressSprite*>( sprite );
    if (!taskProgressSprite)
        return;

    clear();

    actGroup = new QActionGroup(this);
    actGroup->setExclusive(false);

    act = addAction(tr("0%"));
    act->setData(QVariant(0));
    actGroup->addAction(act);

    act = addAction(tr("25%"));
    act->setData(QVariant(1));
    actGroup->addAction(act);

    act = addAction(tr("50%"));
    act->setData(QVariant(2));
    actGroup->addAction(act);

    act = addAction(tr("75%"));
    act->setData(QVariant(3));
    actGroup->addAction(act);

    act = addAction(tr("100%"));
    act->setData(QVariant(4));
    actGroup->addAction(act);

    connect(actGroup, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
    addSeparator();

    act = addAction(QIcon(":/mindstorm/icons/ui-delete.png"), tr("Delete Progress Sprite"));
    connect(act, SIGNAL(triggered()), this, SIGNAL(deleteProgressSprite()));

    CMindMapNodeSpriteMenu::reset(sprite);
}

void CMindMapTaskProgressSpriteMenu::onActionTriggered(QAction *act)
{
    int		r = 0;

    r = act->data().toInt();
    emit progressChanged(r);
}

CMindMapDtTaskTool::CMindMapDtTaskTool(QWidget *parent)
    : CDiagramTool(parent),
      m_taskEditor(NULL),
      m_dateEditor(NULL),
      m_taskPrioritySpriteMenu(NULL),
      m_taskProgressSpriteMenu(NULL)
{
    ui = new Ui::CMindMapDtTaskTool();
    ui->setupUi(this);

    m_taskEditor = new CMindMapUiTaskEditor(NULL);
    m_taskEditor->hide();

    m_dateEditor = new CMindMapUiDateEditor(NULL);
    m_dateEditor->hide();

	m_taskPrioritySpriteMenu = new CMindMapTaskPrioritySpriteMenu(NULL);
    m_taskProgressSpriteMenu = new CMindMapTaskProgressSpriteMenu(NULL);
	// register the menu to the manager
	CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::TaskPriority, m_taskPrioritySpriteMenu);
    CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::TaskProgress, m_taskProgressSpriteMenu);

    ui->m_dueDate->installEventFilter(this);

    connect(ui->m_btnClear, SIGNAL(clicked()), this, SLOT(onClearTask()));
    connect(ui->m_btnShowDateEditor, SIGNAL(clicked()), this, SLOT(onShowDateEditor()));
    connect(ui->m_cbAssignedTo, SIGNAL(activated(QString)), this, SLOT(onAssignedTo(QString)));
    connect(ui->m_cbPriority, SIGNAL(activated(int)), this, SLOT(onPriorityActivated(int)));
    connect(ui->m_cbProgress, SIGNAL(activated(int)), this, SLOT(onProgressActivated(int)));
    connect(ui->m_dueDate, SIGNAL(textEdited(QString)), this, SLOT(onDueDateChanged(QString)));
    connect(m_taskEditor, SIGNAL(deleteTask()), this, SLOT(onClearTask()));
    connect(m_taskEditor, SIGNAL(resetTask(int,int,QString,QString, bool)), this, SLOT(onResetTask(int, int, QString, QString, bool)));
    connect(m_dateEditor, SIGNAL(dateSelected(QString)), this, SLOT(onDateSelected(QString)));
	connect(m_taskPrioritySpriteMenu, SIGNAL(priorityChanged(int)), this, SLOT(onPriorityChanged(int)));
	connect(m_taskPrioritySpriteMenu, SIGNAL(deletePrioritySprite()), this, SLOT(onDeletePrioritySprite()));
    connect(m_taskProgressSpriteMenu, SIGNAL(progressChanged(int)), this, SLOT(onProgressChanged(int)));
    connect(m_taskProgressSpriteMenu, SIGNAL(deleteProgressSprite()), this, SLOT(onDeleteProgressSprite()));
}

CMindMapDtTaskTool::~CMindMapDtTaskTool()
{
    delete ui;
}

bool CMindMapDtTaskTool::support(const QList<CDiagramItem *> &items)
{
    bool                    r = false;
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return r;

    nodes = getAllSelectedNodes(m_d);
    r = (nodes.length() == 1);

    return r;
}

void CMindMapDtTaskTool::reset()
{
    bool                        taskExisted = false;
    CMindMapNode                *node = NULL;
    QList<CMindMapNode*>        nodes;
    QList<CMindMapNodeSprite*>  sprites;
    QString                     assignee("");
    QString                     dueDate;
    int                         priority = -1, progress = -1, index = -1;
    QString                     custom(tr("Customize..."));

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        node = nodes.first();
        sprites = node->sprites(CMindMapNode::Task);
        if (sprites.length() > 0)
        {
			CMindMapTaskSprite          *taskSprite = NULL;

            taskSprite = dynamic_cast<CMindMapTaskSprite*>(sprites.first());
            if (taskSprite)
            {
                assignee = taskSprite->assignedTo();
                dueDate = taskSprite->dueDateString();
                taskExisted = true;
            }
        }

		sprites = node->sprites(CMindMapNode::TaskPriority);
		if (sprites.length() > 0)
        {
			CMindMapTaskPrioritySprite	*prioritySprite = NULL;

            prioritySprite = dynamic_cast<CMindMapTaskPrioritySprite*>(sprites.first());
            if (prioritySprite)
            {
                priority = prioritySprite->priority();
                taskExisted = true;
            }
        }

		sprites = node->sprites(CMindMapNode::TaskProgress);
		if (sprites.length() > 0)
        {
			CMindMapTaskProgressSprite	*progressSprite = NULL;

            progressSprite = dynamic_cast<CMindMapTaskProgressSprite*>(sprites.first());
            if (progressSprite)
            {
                progress = progressSprite->progress();
                taskExisted = true;
            }
        }

        // assignee reset
        if (assignee.isEmpty())
        {
            ui->m_cbAssignedTo->setCurrentIndex(0);
        }
        else
        {
            index = ui->m_cbAssignedTo->findText( assignee );
            if (index > -1 )
            {
                ui->m_cbAssignedTo->setCurrentIndex( index );
            }
            else
            {
                ui->m_cbAssignedTo->removeItem( ui->m_cbAssignedTo->findText(custom) );
                ui->m_cbAssignedTo->addItem( assignee );
                ui->m_cbAssignedTo->addItem( custom );
                ui->m_cbAssignedTo->setCurrentIndex( ui->m_cbAssignedTo->count() - 2 );
            }
        }

        // due date reset
        if (dueDate.isEmpty())
        {
            ui->m_dueDate->setText("");
        }
        else
        {
            ui->m_dueDate->setText(dueDate);
        }

        // priority reset
        if (priority == -1)
        {
            ui->m_cbPriority->setCurrentIndex(0);
        }
        else
        {
            ui->m_cbPriority->setCurrentIndex(priority + 1);
        }

        // progress reset
        if (progress == -1)
        {
            ui->m_cbProgress->setCurrentIndex(0);
        }
        else
        {
            ui->m_cbProgress->setCurrentIndex(progress + 1);
        }
    }

    ui->m_btnClear->setVisible( taskExisted );
}

void CMindMapDtTaskTool::onAssignedTo(QString user)
{
    int                     index = -1;
    bool                    ok;
    QString                 text, custom(tr("Customize..."));
    QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    index = ui->m_cbAssignedTo->findText(user);
    if (index == 0)
    {
        text = "";
    }
    else if (index == ui->m_cbAssignedTo->count() - 1)
    {
        user = QInputDialog::getText(NULL,
                                     tr("Assigned Task"),
                                     tr("Task assigned to:"),
                                     QLineEdit::Normal,
                                     tr("Anonymouse"),
                                     &ok,
                                     Qt::FramelessWindowHint);
        if (ok && !user.isEmpty())
        {
            text = user;
            index = ui->m_cbAssignedTo->findText(user);
            if (index < 0)
            {
                ui->m_cbAssignedTo->addItem(user);
                index = ui->m_cbAssignedTo->findText(custom);
                if (index >= 0)
                {
                    ui->m_cbAssignedTo->removeItem(index);
                    ui->m_cbAssignedTo->addItem(custom);
                }
                ui->m_cbAssignedTo->setCurrentIndex( ui->m_cbAssignedTo->findText(user) );
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        text = user;
    }

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Task,
                                                          QVariant(CMindMapTaskSprite::Assignee),
                                                          QVariant(text)) );
    }
}

void CMindMapDtTaskTool::onDateSelected(QString dueDate)
{
	ui->m_dueDate->setText( dueDate );
    if (dueDate.isEmpty())
        ui->m_dueDate->clearFocus();
    onDueDateChanged(dueDate);
}

void CMindMapDtTaskTool::onDueDateChanged(QString date)
{
    QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Task,
                                                          QVariant(CMindMapTaskSprite::DueDate),
                                                          QVariant(date)) );
    }
}

void CMindMapDtTaskTool::onPriorityChanged(int value)
{
    QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    if (value < 0)
    {
        onDeletePrioritySprite();
    }
    else
    {
        nodes = getAllSelectedNodes(m_d);
        if (nodes.length() > 0)
        {
            m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                              nodes,
                                                              CMindMapNode::TaskPriority,
                                                              QVariant(value)) );
        }
    }
}

void CMindMapDtTaskTool::onPriorityActivated(int value)
{
    onPriorityChanged(value - 1);
}

void CMindMapDtTaskTool::onProgressChanged(int value)
{
    QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    if (value < 0)
    {
        onDeleteProgressSprite();
    }
    else
    {
        nodes = getAllSelectedNodes(m_d);
        if (nodes.length() > 0)
        {
            m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                              nodes,
                                                              CMindMapNode::TaskProgress,
                                                              QVariant(value)) );
        }
    }
}

void CMindMapDtTaskTool::onProgressActivated(int value)
{
    onProgressChanged(value - 1);
}

void CMindMapDtTaskTool::onDeleteProgressSprite()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() < 1)
        return;

    m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                         nodes,
                                                         CMindMapNode::TaskProgress,
                                                         true));
}

void CMindMapDtTaskTool::onDeletePrioritySprite()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() < 1)
        return;

    m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                         nodes,
                                                         CMindMapNode::TaskPriority,
                                                         true));
}

void CMindMapDtTaskTool::onShowTaskEditor()
{
    if (!m_d || !m_taskEditor)
        return;

    if ( m_taskEditor->reset(m_d) )
        m_taskEditor->show();
}

void CMindMapDtTaskTool::onShowDateEditor()
{
    if (!m_d || !m_dateEditor)
        return;

    if (m_dateEditor->reset(m_d))
        m_dateEditor->show();
}

void CMindMapDtTaskTool::onClearTask()
{
    QList<CMindMapNode*>	nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Task,
                                                             true));
		m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::TaskPriority,
                                                             true));
		m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::TaskProgress,
                                                             true));
    }

    ui->m_cbAssignedTo->setCurrentIndex(0);
    ui->m_cbPriority->setCurrentIndex(0);
    ui->m_cbProgress->setCurrentIndex(0);
}

void CMindMapDtTaskTool::onResetTask(int priority, int progress, QString assignedTo, QString dueDate, bool enableTime)
{
    if (priority + 1 != ui->m_cbPriority->currentIndex())
        onPriorityChanged(priority);

    if (progress + 1 != ui->m_cbProgress->currentIndex())
        onProgressChanged(progress);

    if (assignedTo != ui->m_cbAssignedTo->currentText())
        onAssignedTo(assignedTo);

    if (dueDate != ui->m_dueDate->text())
        onDueDateChanged(dueDate);
}

bool CMindMapDtTaskTool::eventFilter(QObject *obj, QEvent *event)
{
    bool        r = false;

    if (obj == ui->m_dueDate)
    {
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
            onShowDateEditor();
            r = true;
            break;

        default:
            break;
        }; // End of switch
    }

    return r;
}
