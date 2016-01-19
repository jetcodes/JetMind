/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramWidget>

#include "../CMindMapSettings.h"
#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "CMindMapDtTextTool.h"
#include "CMindMapDtIconTool.h"
#include "CMindMapDtLineTool.h"
#include "CMindMapDtShapeTool.h"
#include "CMindMapDtStyleTool.h"
#include "CMindMapDtLayoutTool.h"
#include "CMindMapDtNewTool.h"
#include "CMindMapDtEditTool.h"
#include "CMindMapDtTaskTool.h"
#include "CMindMapDtFileTool.h"
#include "CMindMapDtLinkTool.h"
#include "CMindMapDtNoteTool.h"
#include "CMindMapDtGroupTool.h"
#include "CMindMapDtQuickTool.h"
#include "CMindMapToolPanel.h"

CMindMapToolPanel::CMindMapToolPanel(QWidget *parent)
	: CDiagramHostedWidget(parent),
      m_ui(NULL),
	  m_quickLayout(NULL),
	  m_toolsLayout(NULL),
      m_anchorGroup(NULL),
	  m_collapsed(true),
	  m_collapsible(true),
      m_settingMenu(NULL),
      m_actAutoHide(NULL),
      m_actCollapsible(NULL)
{
    QAction		                *action = NULL;
    CDiagramTool                *tool = NULL;
    CMindMapSetting             *setting = NULL;

    m_ui = new Ui::CMindMapToolPanel();
    m_ui->setupUi(this);

	m_quickLayout = new QVBoxLayout();
	m_quickLayout->setContentsMargins(0, 0, 0, 0);
	m_quickLayout->setSpacing(0);

	m_toolsLayout = new QFormLayout();
	m_toolsLayout->setContentsMargins(4, 0, 4, 2);
	m_toolsLayout->setLabelAlignment(Qt::AlignRight);
	m_toolsLayout->setHorizontalSpacing(5);
	m_toolsLayout->setVerticalSpacing(2);

	m_toolsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	m_toolsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	m_toolsLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_settingMenu = new QMenu(m_ui->m_btnSetting);

	m_anchorGroup = new QActionGroup(this);
	m_anchorGroup->setExclusive(true);
    action = m_anchorGroup->addAction( tr("Anchor Selected Symbol") );
    action->setCheckable(true);
    action->setChecked(true);
    action->setData(QVariant( (int)(CDiagramWidget::AnchorToSelectedItem) ));
	
    action = m_anchorGroup->addAction( tr("Anchor Scene Left") );
    action->setCheckable(true);
    action->setData(QVariant( (int)(CDiagramWidget::AnchorToSceneLeft) ));

    action = m_anchorGroup->addAction( tr("Anchor Scene Right") );
    action->setCheckable(true);
    action->setData(QVariant( (int)(CDiagramWidget::AnchorToSceneRight) ));

    action = m_anchorGroup->addAction( tr("Free Float") );
    action->setCheckable(true);
    action->setData(QVariant( (int)(CDiagramWidget::FreeInScene) ));

    m_settingMenu->addActions(m_anchorGroup->actions());
    m_settingMenu->addSeparator();

    m_actAutoHide = new QAction( tr("Auto hide"), NULL );
    m_actAutoHide->setCheckable(true);
    m_actAutoHide->setChecked(false);
    m_settingMenu->addAction(m_actAutoHide);

    m_actCollapsible = new QAction(tr("Collapsible"), NULL);
    m_actCollapsible->setCheckable(true);
    m_actCollapsible->setChecked(true);
    m_settingMenu->addAction(m_actCollapsible);

    m_ui->m_tools->setLayout(m_toolsLayout);
    m_ui->m_quickTools->setLayout(m_quickLayout);
    m_ui->m_expandedView->hide();
    m_ui->m_collapsedView->show();

	connect(m_anchorGroup, SIGNAL(triggered(QAction*)), this, SLOT(onAnchorTriggered(QAction*)));
    connect(m_actAutoHide, SIGNAL(triggered(bool)), this, SLOT(onAutoHideTriggered(bool)));
    connect(m_actCollapsible, SIGNAL(triggered(bool)), this, SLOT(onCollapsibleTriggered(bool)));
    connect(m_ui->m_btnExpand, SIGNAL(clicked()), this, SLOT(onCollapse()));
    connect(m_ui->m_btnCollapse, SIGNAL(clicked()), this, SLOT(onCollapse()));
	connect(this, SIGNAL(collapsible(bool)), this, SLOT(onCollapsibleTriggered(bool)));
    connect(m_ui->m_btnSetting, SIGNAL(clicked()), this, SLOT(onShowAnchorMenu()));

    initTools();
    for (int i = 0; i < m_tools.length(); ++i)
    {
        tool = m_tools.at(i);
        // quick tools
        if (tool->windowTitle() == QString("quick"))
        {
            m_quickLayout->addWidget(tool);
        }
        // normal tools
        else
        {
            m_toolsLayout->addRow(tool->windowTitle(), tool);
        }
    }

    setting = CMindMapSetting::instance();
    if (setting)
    {
        int             anchorType = -1;
        QList<QAction*> actions;

        anchorType = setting->get( Settings::MindMapToolBarAlignment, QVariant(CDiagramWidget::AnchorToSceneRight) ).toInt();
        actions = m_anchorGroup->actions();
        foreach (QAction *act, actions)
        {
            if (act->data().toInt() == anchorType)
                act->setChecked(true);
            else
                act->setChecked(false);
        }

        setCollapsible(setting->get( Settings::MindMapToolBarCollapsible, QVariant(true)).toBool() );

        // establish setting listener
        connect(setting, SIGNAL(configChanged(QString)), this, SLOT(onSettingChanged(QString)));
    }

    setPriority(1);
}

CMindMapToolPanel::~CMindMapToolPanel()
{
    if (m_settingMenu)
        m_settingMenu->clear();
    SAFE_DELETE(m_settingMenu);
    SAFE_DELETE(m_anchorGroup);
    SAFE_DELETE(m_ui);
}

void CMindMapToolPanel::initTools()
{
    CMindMapDtEditTool          *editTool = NULL;
    CMindMapDtQuickTool         *quickTool = NULL;
    CMindMapDtNewTool           *newTool = NULL;
    CMindMapDtGroupTool         *groupTool = NULL;
    CMindMapDtStyleTool         *styleTool = NULL;
    CMindMapDtShapeTool         *shapeTool = NULL;
    CMindMapDtLineTool          *lineTool = NULL;
    CMindMapDtLayoutTool        *layoutTool = NULL;
    CMindMapDtTextTool          *textTool = NULL;
    CMindMapDtIconTool          *iconTool = NULL;
    CMindMapDtTaskTool          *taskTool = NULL;
    CMindMapDtLinkTool          *linkTool = NULL;
    CMindMapDtNoteTool          *noteTool = NULL;
    CMindMapDtFileTool          *fileTool = NULL;
    CMindMapUiGraphMenu         *graphMenu = NULL;

    // clear the tools storage
    m_tools.clear();
    
    editTool = new CMindMapDtEditTool(NULL);
    quickTool = new CMindMapDtQuickTool(NULL);
    newTool = new CMindMapDtNewTool(NULL);
    groupTool = new CMindMapDtGroupTool(NULL);
    styleTool = new CMindMapDtStyleTool(NULL);
    shapeTool = new CMindMapDtShapeTool(NULL);
    lineTool = new CMindMapDtLineTool(NULL);
    layoutTool = new CMindMapDtLayoutTool(NULL);
    textTool = new CMindMapDtTextTool(NULL);
    iconTool = new CMindMapDtIconTool(NULL);
    taskTool = new CMindMapDtTaskTool(NULL);
    linkTool = new CMindMapDtLinkTool(NULL);
    noteTool = new CMindMapDtNoteTool(NULL);
    fileTool = new CMindMapDtFileTool(NULL);

    connect(quickTool, SIGNAL(insertTopicAfter()), newTool, SLOT(onInsertAfter()));
    connect(quickTool, SIGNAL(insertTopicBefore()), newTool, SLOT(onInsertBefore()));
    connect(quickTool, SIGNAL(insertTopicChild()), newTool, SLOT(onInsertChild()));
    connect(quickTool, SIGNAL(insertTopicParent()), newTool, SLOT(onInsertParent()));
    connect(quickTool, SIGNAL(addBoundary()), groupTool, SLOT(onAddBoundary()));
    connect(quickTool, SIGNAL(fillStyleHovered(CDiagramNodeStyle)), styleTool, SLOT(onFillStyleHovered(CDiagramNodeStyle)));
    connect(quickTool, SIGNAL(fillStyleReset()), styleTool, SLOT(onFillStyleReset()));
    connect(quickTool, SIGNAL(fillStyleSelected(CDiagramNodeStyle)), styleTool, SLOT(onFillStyleSelected(CDiagramNodeStyle)));
    connect(quickTool, SIGNAL(shapeSelected(int)), shapeTool, SLOT(onShapeSelected(int)));
    connect(quickTool, SIGNAL(pathSelected(int)), lineTool, SLOT(onPathSelected(int)));
    connect(quickTool, SIGNAL(layoutSelected(int)), layoutTool, SLOT(onLayoutSelected(int)));
    connect(quickTool, SIGNAL(iconSelected(QIcon,int)), iconTool, SLOT(onIconSelected(QIcon, int)));
    connect(quickTool, SIGNAL(showLinkEditor()), linkTool, SLOT(onShowLinkEditor()));
    connect(quickTool, SIGNAL(showNoteEditor()), noteTool, SLOT(onShowNoteEditor()));
    connect(quickTool, SIGNAL(showTaskEditor()), taskTool, SLOT(onShowTaskEditor()));
    connect(quickTool, SIGNAL(browseIconLibrary()), iconTool, SLOT(onBrowseIconLibrary()));
    connect(quickTool, SIGNAL(browsePixmap()), iconTool, SLOT(onBrowsePixmap()));
    connect(quickTool, SIGNAL(showAttachFileDialog()), fileTool, SLOT(onAttachFile()));

    addTool(quickTool);
    addTool(editTool);
    addTool(newTool);
    addTool(groupTool);
    addTool(styleTool);
    addTool(shapeTool);
    addTool(lineTool);
    addTool(layoutTool);
    addTool(textTool);
    addTool(iconTool);
    addTool(taskTool);
    addTool(fileTool);
    addTool(linkTool);
    addTool(noteTool);

    // special ui tool
    graphMenu = new CMindMapUiGraphMenu();
    connect(graphMenu, SIGNAL(insertTopicAfter()), newTool, SLOT(onInsertAfter()));
    connect(graphMenu, SIGNAL(insertTopicBefore()), newTool, SLOT(onInsertBefore()));
    connect(graphMenu, SIGNAL(insertTopicChild()), newTool, SLOT(onInsertChild()));
    connect(graphMenu, SIGNAL(insertTopicParent()), newTool, SLOT(onInsertParent()));
    connect(graphMenu, SIGNAL(addBoundary()), groupTool, SLOT(onAddBoundary()));
    connect(graphMenu, SIGNAL(shapeSelected(int)), shapeTool, SLOT(onShapeSelected(int)));
    connect(graphMenu, SIGNAL(pathSelected(int)), lineTool, SLOT(onPathSelected(int)));
    connect(graphMenu, SIGNAL(layoutSelected(int)), layoutTool, SLOT(onLayoutSelected(int)));
    REGISTER_CONTEXT_MENU(CDiagram::CursorMode, graphMenu);
}

bool CMindMapToolPanel::support(CDiagram *d)
{
    bool                        r = false;
    QList<CDiagramItem*>        items;

    items = d->selectedTopLevelItems();
    if (items.length() > 0)
    {
//      foreach (CDiagramItem *item, items)
//      {
//            if (dynamic_cast<CMindMapNode*>(item))
//          {
//                r = true;
//              break;
//          }
//      }

        r = true;
    }

    return r;
}

void CMindMapToolPanel::onSettingChanged(const QString &key)
{
    int                 anchor = -1;
    bool                r = false;
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (!setting)
        return;

    if (key == Settings::MindMapToolBarAutoHide)
    {
        r = setting->get(key, QVariant(true)).toBool();
        if (r != m_actAutoHide->isChecked())
        {
            m_actAutoHide->setChecked(r);
            emit autoHide(r);
        }
    }
    else if (key == Settings::MindMapToolBarCollapsible)
    {
        r = setting->get(key, QVariant(true)).toBool();
        if (r != m_actCollapsible->isChecked())
        {
            m_actCollapsible->setChecked(r);
            emit collapsible(r);
        }
    }
    else if (key == Settings::MindMapToolBarAlignment)
    {
        QAction             *action = NULL;
        QList<QAction*>     actions;

        anchor = setting->get(key, QVariant(CDiagramWidget::AnchorToSceneRight)).toInt();
        actions = m_anchorGroup->actions();
        foreach (action, actions)
        {
            if (action->data().toInt() == anchor && !action->isChecked())
            {
                action->setChecked(true);
                emit anchorTypeSelected( (CDiagramWidget::AnchorType) anchor);
                break;
            }
        }
    }
    else
    {
        // nothing
    }
}

void CMindMapToolPanel::setTitle(const QString &value)
{
    //
}

void CMindMapToolPanel::init(CDiagramWidget *widget, CDiagram *d)
{
    if (widget->anchorType() == CDiagramWidget::AnchorToSelectedItem)
    {
        setCollapsible(true);
    }

	connect(this, SIGNAL(anchorTypeSelected(CDiagramWidget::AnchorType)), widget, SLOT(onAnchorTypeChanged(CDiagramWidget::AnchorType)));
	connect(this, SIGNAL(autoHide(bool)), widget, SLOT(onAutoHide(bool)));

	CDiagramHostedWidget::init(widget, d);
}

void CMindMapToolPanel::uninit(CDiagramWidget *widget, CDiagram *d)
{
	disconnect(this, SIGNAL(anchorTypeSelected(CDiagramWidget::AnchorType)), widget, SLOT(onAnchorTypeChanged(CDiagramWidget::AnchorType)));
	disconnect(this, SIGNAL(autoHide(bool)), widget, SLOT(onAutoHide(bool)));
	
	CDiagramHostedWidget::uninit(widget, d);
}

void CMindMapToolPanel::reset()
{
	QWidget					*w = NULL;
	CDiagramTool			*t = NULL;
	CDiagramTool			*q = NULL;
	QList<CDiagramItem*>	items;
	
	items = m_d->selectedTopLevelItems();
	for (int i = 0; i < m_tools.length(); ++i)
	{
		t = m_tools.at(i);
		w = m_toolsLayout->labelForField(t);
		if (w)
			w->hide();
		t->hide();
        t->setHostWidget(NULL);
	}

	for (int i = 0; i < m_tools.length(); ++i)
	{
		t = m_tools.at(i);
		if (t && t->support(items) )
		{
			t->show();
			t->reset();
            t->setHostWidget(this);
			w = m_toolsLayout->labelForField(t);
			if (w)
				w->show();
		}
	}

	if (isCollapsible())
	{
		setCollapsed(true);
	}
	else
	{
        m_ui->m_collapsedView->hide();
        m_ui->m_expandedView->show();

		// both activations are needed here. Only by
		// activate two layout, the resize can be correct.
		m_toolsLayout->activate();
	}
	layout()->activate();
	resize(layout()->sizeHint());
}

void CMindMapToolPanel::onAnchorTriggered(QAction *action)
{
	int	type = action->data().toInt();

    if (type == CDiagramWidget::AnchorToSelectedItem)
    {
        m_ui->m_btnCollapse->setVisible(true);
        emit collapsible(true);
    }

	emit anchorTypeSelected( (CDiagramWidget::AnchorType) type);
}

void CMindMapToolPanel::onAutoHideTriggered(bool value)
{
	emit autoHide(value);
}

void CMindMapToolPanel::onCollapsibleTriggered(bool value)
{
	setCollapsible(value);
}

void CMindMapToolPanel::setCollapsed(bool value)
{
	if (!isCollapsible())
		return;

	m_collapsed = value;
	if (!m_collapsed)
	{
        m_ui->m_collapsedView->hide();
        m_ui->m_expandedView->show();
		m_toolsLayout->activate();
        m_ui->m_expandedView->layout()->activate();
	}
	else
	{
        m_ui->m_collapsedView->show();
        m_ui->m_expandedView->hide();
		m_quickLayout->activate();
        m_ui->m_collapsedView->layout()->activate();
	}
	layout()->activate();
    resize(layout()->sizeHint());

    if (m_d)
        m_d->emitItemsGeometryChanged();
}

bool CMindMapToolPanel::isCollapsed()
{
	return m_collapsed;
}

void CMindMapToolPanel::setCollapsible(bool value)
{
	m_collapsible = value;
	if (!value)
	{
        m_ui->m_expandedView->show();
        m_ui->m_collapsedView->hide();
	}
	else
	{
        m_ui->m_expandedView->hide();
        m_ui->m_collapsedView->show();
	}
	layout()->activate();
	resize(layout()->sizeHint());
}

bool CMindMapToolPanel::isCollapsible()
{
	return m_collapsible;
}

void CMindMapToolPanel::onCollapse()
{
	setCollapsed(!isCollapsed());
}

void CMindMapToolPanel::onShowAnchorMenu()
{
    QRect			menuRect;
    QRectF			sc, r0;
    QPoint			p0, p1, p2, p3, pt;
    QGraphicsView	*v = NULL;

    if (!m_d || !m_parent || !m_settingMenu)
        return;

    v = m_d->getMainView();
    if (!v)
        return;

    menuRect = m_settingMenu->geometry();
    p0 = QPoint(m_ui->m_btnSetting->x() - m_settingMenu->width(), m_ui->m_btnSetting->y() + m_ui->m_btnSetting->height());
    p1 = QPoint(m_ui->m_btnSetting->x() - m_settingMenu->width(), m_ui->m_btnSetting->y() - m_settingMenu->height() );
    p2 = QPoint(m_ui->m_btnSetting->x(), m_ui->m_btnSetting->y() + m_ui->m_btnSetting->height());
    p3 = QPoint(m_ui->m_btnSetting->x(), m_ui->m_btnSetting->y() - m_settingMenu->height() );

    sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
    r0 = m_parent->mapToScene( m_parent->shape() ).boundingRect();

    if (r0.right() + menuRect.width() > sc.right())
    {
        if (r0.top() + menuRect.height() > sc.bottom())
            pt = p1;
        else
            pt = p0;
    }
    else
    {
        if (r0.top() + menuRect.height() > sc.bottom())
            pt = p3;
        else
            pt = p2;
    }

    m_settingMenu->exec(mapToGlobal(pt));
}
