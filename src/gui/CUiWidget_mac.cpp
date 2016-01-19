/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <protect/CActivateHandler>
#include "CUiWidget_mac.h"
#include "CActionManager.h"
#include "CAppSettings.h"

CUiWidget_mac::CUiWidget_mac(QWidget *parent)
	: QMainWindow(parent),
      m_centralWidget(NULL)
{
	m_centralWidget = new QWidget(this);
	setCentralWidget(m_centralWidget);

	setWindowIcon(QIcon());
}

CUiWidget_mac::~CUiWidget_mac()
{
	SAFE_DELETE(m_centralWidget);
}

void CUiWidget_mac::setupUi(QLayout *globalLayout, CActionManager *actionManager)
{
	QMenu			*menu = NULL, *t = NULL;
    QSettings		settings;
    QAction         *act = NULL;
    QStringList     recentFiles;
    QActionGroup    *group = NULL;

	// file menu
    menu = menuBar()->addMenu(tr("&File"));
	menu->addAction(actionManager->m_actNewFile);
    menu->addAction(actionManager->m_actNewSheet);
	menu->addSeparator();
	menu->addAction(actionManager->m_actOpenFile);

    t = new QMenu(tr("Recent Files"), menu);
    group = new QActionGroup(this);
    if (group)
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onRecentFileTriggered(QAction*)));
    recentFiles = settings.value(Settings::RecentFiles).toStringList();
    recentFiles.removeDuplicates();
    for (int i = 0; i < recentFiles.length(); ++i)
    {
        QFileInfo	info(recentFiles.at(i));
        if (info.exists())
        {
            act = group->addAction( info.fileName() );
            act->setData(QVariant(info.absoluteFilePath()));
            act->setToolTip(info.filePath());
        }
    }

    t->addActions(group->actions());
    menu->addMenu(t);

    menu->addSeparator();
	menu->addAction(actionManager->m_actSaveFile);
    menu->addAction(actionManager->m_actSaveAs);
	menu->addSeparator();

	menu->addAction(actionManager->m_actExport);
	menu->addSeparator();
	menu->addAction(actionManager->m_actPrint);
	menu->addAction(actionManager->m_actPrintPreview);
	menu->addSeparator();
	menu->addAction(actionManager->m_actExit);

	// edit menu
    menu = menuBar()->addMenu(tr("&Edit"));
    menu->addAction(actionManager->m_actRedo);
    menu->addAction(actionManager->m_actUndo);
    menu->addSeparator();
    menu->addAction(actionManager->m_actCut);
    menu->addAction(actionManager->m_actCopy);
    menu->addAction(actionManager->m_actPaste);
    menu->addAction(actionManager->m_actSelectAll);

	// view menu
    menu = menuBar()->addMenu(tr("&View"));
    menu->addAction(actionManager->m_actShowToolBar);
    menu->addSeparator();
    menu->addAction(actionManager->m_actShowCanvasSetupPanel);
    menu->addAction(actionManager->m_actShowCanvasQuickView);
    menu->addAction(actionManager->m_actShowGrids);
	menu->addSeparator();
    t = new QMenu(tr("Property Editor Anchors"), menu);
    t->addAction( actionManager->m_actToolPanelAnchorSelectedBymbol );
    t->addAction( actionManager->m_actToolPanelAnchorLeft );
    t->addAction( actionManager->m_actToolPanelAnchorRight );
    t->addAction( actionManager->m_actToolPanelFreeFloat );
    menu->addMenu( t );
    menu->addAction( actionManager->m_actToolPanelAutoHide );
    menu->addAction( actionManager->m_actToolPanelCollapsible );
    menu->addSeparator();
    menu->addAction(actionManager->m_actPresentation);
    menu->addAction(actionManager->m_actBrainStorm);

    actionManager->m_actShowGrids->setChecked( settings.value(Settings::MindMapBackgroundShowGrid, QVariant(true)).value<bool>() );
    actionManager->m_actShowToolBar->setChecked( settings.value(Settings::ShowToolBar, QVariant(true)).value<bool>());

	// help menu
    menu = menuBar()->addMenu(tr("&Help"));
    menu->addAction(actionManager->m_actManual);

    menu->addSeparator();

    menu->addAction(actionManager->m_actSupport);
    menu->addAction(actionManager->m_actUpdate);
    menu->addSeparator();
    menu->addAction(actionManager->m_actAboutRemind);

	// setup layout
	if (m_centralWidget)
		m_centralWidget->setLayout(globalLayout);
}


void CUiWidget_mac::onFullScreen()
{
	toggleFullScreen(!isFullScreen());
	emit fullscreen(isFullScreen());
}

void CUiWidget_mac::toggleFullScreen(bool value)
{
	if (value && beforeEnterFullScreen())
	{
		showFullScreen();
	}
	else
	{
		showNormal();
		afterExitFullScreen();
	}
}

bool CUiWidget_mac::beforeEnterFullScreen()
{
    return true;
}

bool CUiWidget_mac::afterExitFullScreen()
{
    return true;
}

void CUiWidget_mac::resizeEvent(QResizeEvent *evt)
{
    QMainWindow::resizeEvent(evt);

    emit geometryChanged( geometry() );
}

