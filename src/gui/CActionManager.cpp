/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramFile>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <mindmap/CMindMapSettings>

#include "CWorkbench.h"
#include "CActionManager.h"

CActionManager::CActionManager(CWorkbench *bench)
	: QObject(NULL)
{
    QClipboard          *clipboard = QApplication::clipboard();
    CMindMapSetting     *setting = CMindMapSetting::instance();

	initActions();
	retranslateUi();

	m_actCopy->setEnabled(false);
	m_actPaste->setEnabled(false);
	m_actCut->setEnabled(false);
	m_actDelete->setEnabled(false);
	m_actSaveFile->setEnabled(false);
	m_actSaveAs->setEnabled(false);
	m_actPrint->setEnabled(false);
	m_actPrintPreview->setEnabled(false);
	m_actExport->setEnabled(false);
	m_actRedo->setEnabled(false);
	m_actUndo->setEnabled(false);
    m_actSelectAll->setEnabled(false);
    m_actTextAlignLeft->setEnabled(false);
	m_actTextAlignRight->setEnabled(false);
	m_actTextAlignCenter->setEnabled(false);
	m_actTextAlignJustify->setEnabled(false);
    m_actPresentation->setEnabled(false);
    m_actBrainStorm->setEnabled(false);
	m_actNext->setEnabled(false);
	m_actPrev->setEnabled(false);
	m_actCloseFile->setEnabled(false);
	m_actSaveAsTemplate->setEnabled(false);

    if (setting)
    {
        m_actShowGrids->setChecked( setting->get(Settings::MindMapBackgroundShowGrid, QVariant(false)).toBool());
        m_actToolPanelAutoHide->setChecked(setting->get(Settings::MindMapToolBarAutoHide, QVariant(true)).toBool());
        m_actToolPanelCollapsible->setChecked(setting->get(Settings::MindMapToolBarCollapsible, QVariant(true)).toBool());
        m_actToolPanelAnchorLeft->setChecked(
                    setting->get(Settings::MindMapToolBarAlignment,
                                 QVariant(CDiagramWidget::AnchorToSceneRight)
                                 ).toInt() == CDiagramWidget::AnchorToSceneLeft
                    );
        m_actToolPanelAnchorRight->setChecked(
                    setting->get(Settings::MindMapToolBarAlignment,
                                 QVariant(CDiagramWidget::AnchorToSceneRight)
                                 ).toInt() == CDiagramWidget::AnchorToSceneRight
                    );
        m_actToolPanelAnchorSelectedBymbol->setChecked(
                    setting->get(Settings::MindMapToolBarAlignment,
                                 QVariant(CDiagramWidget::AnchorToSceneRight)
                                 ).toInt() == CDiagramWidget::AnchorToSelectedItem
                    );
        m_actToolPanelFreeFloat->setChecked(
                    setting->get(Settings::MindMapToolBarAlignment,
                                 QVariant(CDiagramWidget::AnchorToSceneRight)
                                 ).toInt() == CDiagramWidget::FreeInScene
                    );
    }

    connect(m_actNewFile, SIGNAL(triggered()), bench, SLOT(onNewFile()));
    connect(m_actNewSheet, SIGNAL(triggered()), bench, SLOT(onNewDiagram()));
	connect(m_actOpenFile, SIGNAL(triggered()), bench, SLOT(onOpenFile()));
	connect(m_actSaveFile, SIGNAL(triggered()), bench, SLOT(onSaveFile()));
	connect(m_actUndo, SIGNAL(triggered()), bench, SLOT(onUndo()));
	connect(m_actRedo, SIGNAL(triggered()), bench, SLOT(onRedo()));
	connect(m_actCopy, SIGNAL(triggered()), bench, SLOT(onCopy()));
	connect(m_actCut, SIGNAL(triggered()), bench, SLOT(onCut()));
	connect(m_actDelete, SIGNAL(triggered()), bench, SLOT(onDelete()));
	connect(m_actPaste, SIGNAL(triggered()), bench, SLOT(onPaste()));
	connect(m_actPrint, SIGNAL(triggered()), bench, SLOT(onPrint()));
	connect(m_actPrintPreview, SIGNAL(triggered()), bench, SLOT(onPrintPreview()));
	connect(m_actExport, SIGNAL(triggered()), bench, SLOT(onExport()));
	connect(m_actSaveAs, SIGNAL(triggered()), bench, SLOT(onSaveFileAs()));
	connect(m_actExit, SIGNAL(triggered()), bench, SLOT(onExit()));
	connect(m_actSelectAll, SIGNAL(triggered()), bench, SLOT(onSelectAll()));
	connect(m_actManual, SIGNAL(triggered()), bench, SLOT(onHelp()));
	connect(m_actAboutRemind, SIGNAL(triggered()), bench, SLOT(onAbout()));
    connect(m_actSaveAsTemplate, SIGNAL(triggered()), bench, SLOT(onShowAddTemplateDialog()));
	connect(m_actLoadTemplatePackage, SIGNAL(triggered()), bench, SLOT(onLoadTemplatePackage()));
	connect(m_actShowGrids, SIGNAL(triggered(bool)), bench, SLOT(onShowGrids(bool)));
	connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(onClipboardChanged(QClipboard::Mode)));
	connect(bench, SIGNAL(diagramChanged(CDiagram*,CDiagram*)), this, SLOT(onDiagramChanged(CDiagram*, CDiagram*)));
    connect(bench, SIGNAL(fileChanged(CDiagramFile*)), this, SLOT(onFileChanged(CDiagramFile*)));

    connect(m_actSupport, SIGNAL(triggered()), bench, SLOT(onSupport()));
    connect(m_actUpdate, SIGNAL(triggered()), bench, SLOT(onUpdate()));

    connect(m_actShowToolBar, SIGNAL(toggled(bool)), bench, SLOT(onShowToolBar(bool)));
	connect(m_actShowCanvasSetupPanel, SIGNAL(triggered()), bench, SLOT(onShowCanvasSetupPanel()));
    connect(m_actShowCanvasQuickView, SIGNAL(triggered()), bench, SLOT(onShowCanvasQuickView()));
    connect(m_actPresentation, SIGNAL(triggered()), bench, SLOT(onPresentation()));
    connect(m_actBrainStorm, SIGNAL(triggered()), bench, SLOT(onBrainStorm()));

    connect(m_toolPanelAnchorGroup, SIGNAL(triggered(QAction*)), this, SLOT(onPropertyEditorAnchorActionTriggered(QAction*)));
    connect(m_actToolPanelAutoHide, SIGNAL(triggered(bool)), this, SLOT(onPropertyEditorAutoHide(bool)));
    connect(m_actToolPanelCollapsible, SIGNAL(triggered(bool)), this, SLOT(onPropertyEditorCollapsible(bool)));

    connect(setting, SIGNAL(configChanged(QString)), this, SLOT(onSettingChanged(QString)));
}

CActionManager::~CActionManager()
{

}

void CActionManager::initActions()
{
	m_actNewFile = new QAction(this);
    m_actNewFile->setObjectName(QString::fromUtf8("m_actNewFile"));
    m_actNewFile->setIcon( QIcon(":/res/icons/document-new.png") );

    m_actNewSheet = new QAction(this);
    m_actNewSheet->setObjectName(QString::fromUtf8("m_actNewSheet"));
    m_actNewSheet->setIcon( QIcon(":/res/icons/document-new-sheet.png") );

    m_actSaveFile = new QAction(this);
    m_actSaveFile->setObjectName(QString::fromUtf8("m_actSaveFile"));
    m_actSaveFile->setIcon( QIcon(":/res/icons/document-save.png") );

    m_actOpenFile = new QAction(this);
    m_actOpenFile->setObjectName(QString::fromUtf8("m_actOpenFile"));
    m_actOpenFile->setIcon( QIcon(":/res/icons/document-open.png") );

    m_actSaveAs = new QAction(this);
    m_actSaveAs->setObjectName(QString::fromUtf8("m_actSaveAs"));
    m_actSaveAs->setIcon( QIcon(":/res/icons/document-save-as.png") );

    m_actPrint = new QAction(this);
    m_actPrint->setObjectName(QString::fromUtf8("m_actPrint"));
    m_actPrint->setIcon( QIcon(":/res/icons/document-print.png") );

    m_actPrintPreview = new QAction(this);
    m_actPrintPreview->setObjectName(QString::fromUtf8("m_actPrintPreview"));
    m_actPrintPreview->setIcon( QIcon(":/res/icons/document-print-preview.png") );

    m_actExit = new QAction(this);
    m_actExit->setObjectName(QString::fromUtf8("m_actExit"));

    m_actRedo = new QAction(this);
    m_actRedo->setObjectName(QString::fromUtf8("m_actRedo"));
    m_actRedo->setIcon( QIcon(":/res/icons/edit-redo.png") );

    m_actUndo = new QAction(this);
    m_actUndo->setObjectName(QString::fromUtf8("m_actUndo"));
    m_actUndo->setIcon( QIcon(":/res/icons/edit-undo.png") );

    m_actCopy = new QAction(this);
    m_actCopy->setObjectName(QString::fromUtf8("m_actCopy"));
    m_actCopy->setIcon( QIcon(":/res/icons/edit-copy.png") );

    m_actCut = new QAction(this);
    m_actCut->setObjectName(QString::fromUtf8("m_actCut"));
    m_actCut->setIcon( QIcon(":/res/icons/edit-cut.png") );

    m_actPaste = new QAction(this);
    m_actPaste->setObjectName(QString::fromUtf8("m_actPaste"));
    m_actPaste->setIcon( QIcon(":/res/icons/edit-paste.png") );

    m_actDelete = new QAction(this);
    m_actDelete->setObjectName(QString::fromUtf8("m_actDelete"));
    m_actDelete->setIcon( QIcon(":/res/icons/edit-delete.png") );

    m_actSelectAll = new QAction(this);
    m_actSelectAll->setObjectName(QString::fromUtf8("m_actSelectAll"));

    m_actTextAlignLeft = new QAction(this);
    m_actTextAlignLeft->setObjectName(QString::fromUtf8("m_actTextAlignLeft"));
    m_actTextAlignLeft->setIcon( QIcon(":/res/icons/edit-alignment-left.png") );

    m_actTextAlignRight = new QAction(this);
    m_actTextAlignRight->setObjectName(QString::fromUtf8("m_actTextAlignRight"));
    m_actTextAlignRight->setIcon( QIcon(":/res/icons/edit-alignment-right.png"));

    m_actTextAlignCenter = new QAction(this);
    m_actTextAlignCenter->setObjectName(QString::fromUtf8("m_actTextAlignCenter"));
    m_actTextAlignCenter->setIcon( QIcon(":/res/icons/edit-alignment-center.png") );

    m_actTextAlignJustify = new QAction(this);
    m_actTextAlignJustify->setObjectName(QString::fromUtf8("m_actTextAlignJustify"));
    m_actTextAlignJustify->setIcon( QIcon(":/res/icons/edit-alignment-justify.png") );

    m_actManual = new QAction(this);
    m_actManual->setObjectName(QString::fromUtf8("m_actManual"));

    m_actAboutRemind = new QAction(this);
    m_actAboutRemind->setObjectName(QString::fromUtf8("m_actAboutRemind"));

	m_actExport = new QAction(this);
    m_actExport->setObjectName(QString::fromUtf8("m_actExport"));
    m_actExport->setIcon( QIcon(":/res/icons/document-export.png") );

	m_actSupport = new QAction(this);
    m_actSupport->setObjectName(QString::fromUtf8("m_actSupport"));

	m_actUpdate = new QAction(this);
    m_actUpdate->setObjectName(QString::fromUtf8("m_actUpdate"));

	m_actShowToolBar = new QAction(this);
	m_actShowToolBar->setCheckable(true);
    m_actShowToolBar->setObjectName(QString::fromUtf8("m_actShowToolBar"));

	m_actShowCanvasSetupPanel = new QAction(this);
	m_actShowCanvasSetupPanel->setCheckable(false);
	m_actShowCanvasSetupPanel->setObjectName(QString::fromUtf8("m_actShowCanvasSetupPanel"));

    m_actShowCanvasQuickView = new QAction(this);
    m_actShowCanvasQuickView->setCheckable(false);
    m_actShowCanvasQuickView->setObjectName(QString::fromUtf8("m_actShowQuickView"));

    m_toolPanelAnchorGroup = new QActionGroup(this);
    m_toolPanelAnchorGroup->setExclusive(true);

    m_actToolPanelAnchorSelectedBymbol = new QAction(m_toolPanelAnchorGroup);
    m_actToolPanelAnchorSelectedBymbol->setCheckable(true);
    m_actToolPanelAnchorSelectedBymbol->setData( QVariant(CDiagramWidget::AnchorToSelectedItem));
    m_actToolPanelAnchorSelectedBymbol->setObjectName(QString::fromUtf8("m_actToolPanelAnchorSelectedBymbol"));
    m_toolPanelAnchorGroup->addAction(m_actToolPanelAnchorSelectedBymbol);

    m_actToolPanelAnchorLeft = new QAction(m_toolPanelAnchorGroup);
    m_actToolPanelAnchorLeft->setCheckable(true);
    m_actToolPanelAnchorLeft->setData( QVariant(CDiagramWidget::AnchorToSceneLeft));
    m_actToolPanelAnchorLeft->setObjectName(QString::fromUtf8("m_actToolPanelAnchorLeft"));
    m_toolPanelAnchorGroup->addAction(m_actToolPanelAnchorLeft);

    m_actToolPanelAnchorRight = new QAction(m_toolPanelAnchorGroup);
    m_actToolPanelAnchorRight->setCheckable(true);
    m_actToolPanelAnchorRight->setData( QVariant(CDiagramWidget::AnchorToSceneRight));
    m_actToolPanelAnchorRight->setObjectName(QString::fromUtf8("m_actToolPanelAnchorRight"));
    m_toolPanelAnchorGroup->addAction(m_actToolPanelAnchorRight);

    m_actToolPanelFreeFloat = new QAction(m_toolPanelAnchorGroup);
    m_actToolPanelFreeFloat->setCheckable(true);
    m_actToolPanelFreeFloat->setData( QVariant(CDiagramWidget::FreeInScene));
    m_actToolPanelFreeFloat->setObjectName(QString::fromUtf8("m_actToolPanelFreeFloat"));
    m_toolPanelAnchorGroup->addAction(m_actToolPanelFreeFloat);

    m_actToolPanelAutoHide = new QAction(this);
    m_actToolPanelAutoHide->setCheckable(true);
    m_actToolPanelAutoHide->setObjectName(QString::fromUtf8("m_actToolPanelAutoHide"));

    m_actToolPanelCollapsible = new QAction(this);
    m_actToolPanelCollapsible->setCheckable(true);
    m_actToolPanelCollapsible->setObjectName(QString::fromUtf8("m_actToolPanelCollapsible"));

	m_actPresentation = new QAction(this);
    m_actPresentation->setObjectName(QString::fromUtf8("m_actPresentation"));
    m_actPresentation->setIcon( QIcon(":/res/icons/presentation-start.png") );

    m_actBrainStorm = new QAction(this);
    m_actBrainStorm->setObjectName(QString::fromUtf8("m_actBrainStorm"));
    m_actBrainStorm->setIcon( QIcon(":/res/icons/presentation-edit.png") );

    m_actShowGrids = new QAction(this);
    m_actShowGrids->setCheckable(true);
    m_actShowGrids->setObjectName(QString::fromUtf8("m_actShowGrids"));

    for (int i = 0; i < 5; ++i)
	{
		m_actRecentFiles.push_back(new QAction(this));
		m_actRecentFiles[i]->setVisible(false);
	}

	m_actNext = new QAction(this);
    m_actNext->setObjectName(QString::fromUtf8("m_actNext"));
    m_actNext->setIcon( QIcon(":/res/icons/document-page-next.png") );

	m_actPrev = new QAction(this);
    m_actPrev->setObjectName(QString::fromUtf8("m_actPrev"));
    m_actPrev->setIcon( QIcon(":/res/icons/document-page-previous.png") );

	m_actCloseFile = new QAction(this);
    m_actCloseFile->setObjectName(QString::fromUtf8("m_actCloseFile"));
    m_actCloseFile->setIcon( QIcon(":/res/icons/document-close.png") );

    m_actSaveAsTemplate = new QAction(this);
    m_actSaveAsTemplate->setObjectName(QString::fromUtf8("m_actSaveAsTemplate"));

	m_actLoadTemplatePackage = new QAction(this);
    m_actLoadTemplatePackage->setObjectName(QString::fromUtf8("m_actLoadTemplatePackage"));

	m_actBrowseTemplates = new QAction(this);
    m_actBrowseTemplates->setObjectName(QString::fromUtf8("m_actBrowseTemplates"));
}

void CActionManager::retranslateUi()
{
    m_actNewFile->setText(QApplication::translate("CActionManager", "New File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actNewFile->setToolTip(QApplication::translate("CActionManager", "New File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actNewFile->setShortcut(QApplication::translate("CActionManager", "Ctrl+N", 0, QApplication::UnicodeUTF8));

    m_actNewSheet->setText(QApplication::translate("CActionManager", "Add Sheet", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actNewSheet->setToolTip(QApplication::translate("CActionManager", "Add Sheet", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actNewSheet->setShortcut(QApplication::translate("CActionManager", "Ctrl+Shift+N", 0, QApplication::UnicodeUTF8));

    m_actSaveFile->setText(QApplication::translate("CActionManager", "Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actSaveFile->setToolTip(QApplication::translate("CActionManager", "Save Current File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actSaveFile->setShortcut(QApplication::translate("CActionManager", "Ctrl+S", 0, QApplication::UnicodeUTF8));
    m_actOpenFile->setText(QApplication::translate("CActionManager", "Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actOpenFile->setToolTip(QApplication::translate("CActionManager", "Open File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actOpenFile->setShortcut(QApplication::translate("CActionManager", "Ctrl+O", 0, QApplication::UnicodeUTF8));
    m_actSaveAs->setText(QApplication::translate("CActionManager", "Save As...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actSaveAs->setToolTip(QApplication::translate("CActionManager", "Save File As...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actPrint->setText(QApplication::translate("CActionManager", "Print", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actPrint->setToolTip(QApplication::translate("CActionManager", "Print Current File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actPrint->setShortcut(QApplication::translate("CActionManager", "Ctrl+P", 0, QApplication::UnicodeUTF8));
    m_actPrintPreview->setText(QApplication::translate("CActionManager", "Print Preview", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actPrintPreview->setToolTip(QApplication::translate("CActionManager", "Print Preview", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actExit->setText(QApplication::translate("CActionManager", "Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actExit->setToolTip(QApplication::translate("CActionManager", "Exit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actRedo->setText(QApplication::translate("CActionManager", "Redo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actRedo->setToolTip(QApplication::translate("CActionManager", "Redo", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actRedo->setShortcut(QApplication::translate("CActionManager", "Ctrl+Y", 0, QApplication::UnicodeUTF8));
    m_actUndo->setText(QApplication::translate("CActionManager", "Undo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actUndo->setToolTip(QApplication::translate("CActionManager", "Undo", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actUndo->setShortcut(QApplication::translate("CActionManager", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
    m_actCopy->setText(QApplication::translate("CActionManager", "Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actCopy->setToolTip(QApplication::translate("CActionManager", "Copy", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actPaste->setText(QApplication::translate("CActionManager", "Paste", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actPaste->setToolTip(QApplication::translate("CActionManager", "Paste", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actPaste->setShortcut(QApplication::translate("CActionManager", "Ctrl+V", 0, QApplication::UnicodeUTF8));
    m_actDelete->setText(QApplication::translate("CActionManager", "Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actDelete->setToolTip(QApplication::translate("CActionManager", "Delete", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actSelectAll->setText(QApplication::translate("CActionManager", "Select All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actSelectAll->setToolTip(QApplication::translate("CActionManager", "Select All Shapes", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actSelectAll->setShortcut(QApplication::translate("CActionManager", "Ctrl+A", 0, QApplication::UnicodeUTF8));
    m_actTextAlignLeft->setText(QApplication::translate("CActionManager", "Align Left", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actTextAlignLeft->setToolTip(QApplication::translate("CActionManager", "Align Left", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actTextAlignRight->setText(QApplication::translate("CActionManager", "Align Right", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actTextAlignRight->setToolTip(QApplication::translate("CActionManager", "Align Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actTextAlignCenter->setText(QApplication::translate("CActionManager", "Align Center", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actTextAlignCenter->setToolTip(QApplication::translate("CActionManager", "Align Center", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actTextAlignJustify->setText(QApplication::translate("CActionManager", "Align Justify", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actTextAlignJustify->setToolTip(QApplication::translate("CActionManager", "Align Justify", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actManual->setText(QApplication::translate("CActionManager", "Tips and Shortcuts", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actManual->setToolTip(QApplication::translate("CActionManager", "Tips and Shortcuts", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actManual->setShortcut(QApplication::translate("CActionManager", "F1", 0, QApplication::UnicodeUTF8));
    m_actAboutRemind->setText(QApplication::translate("CActionManager", "About JetMind", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actAboutRemind->setToolTip(QApplication::translate("CActionManager", "About JetMind", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actExport->setText(QApplication::translate("CActionManager", "Export", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actExport->setToolTip(QApplication::translate("CActionManager", "Export Diagram to Images", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actExport->setShortcut(QApplication::translate("CActionManager", "Ctrl+E", 0, QApplication::UnicodeUTF8));
    m_actCut->setText(QApplication::translate("CActionManager", "Cut", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actCut->setToolTip(QApplication::translate("CActionManager", "Cut", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    m_actCut->setShortcut(QApplication::translate("CActionManager", "Ctrl+X", 0, QApplication::UnicodeUTF8));

	m_actSupport->setText(QApplication::translate("CActionManager", "Support", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actSupport->setToolTip(QApplication::translate("CActionManager", "Support", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actUpdate->setText(QApplication::translate("CActionManager", "Check for Update", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actUpdate->setToolTip(QApplication::translate("CActionManager", "Check for Update", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actShowToolBar->setText(QApplication::translate("CActionManager", "Show ToolBar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actShowToolBar->setToolTip(QApplication::translate("CActionManager", "Show ToolBar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actShowCanvasSetupPanel->setText(QApplication::translate("CActionManager", "Show Canvas Setting Panel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actShowCanvasSetupPanel->setToolTip(QApplication::translate("CActionManager", "Show Canvas Setting Panel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actShowCanvasQuickView->setText(QApplication::translate("CActionManager", "Show Canvas Quick View", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actShowCanvasQuickView->setToolTip(QApplication::translate("CActionManager", "Show Canvas Quick View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelAnchorSelectedBymbol->setText(QApplication::translate("CActionManager", "Anchor Selected Symbol", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelAnchorSelectedBymbol->setToolTip(QApplication::translate("CActionManager", "Anchor Selected Symbol", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelAnchorLeft->setText(QApplication::translate("CActionManager", "Anchor Scene Left", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelAnchorLeft->setToolTip(QApplication::translate("CActionManager", "Anchor Scene Left", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelAnchorRight->setText(QApplication::translate("CActionManager", "Anchor Scene Right", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelAnchorRight->setToolTip(QApplication::translate("CActionManager", "Anchor Scene Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelFreeFloat->setText(QApplication::translate("CActionManager", "Free Float", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelFreeFloat->setToolTip(QApplication::translate("CActionManager", "Free Float", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelAutoHide->setText(QApplication::translate("CActionManager", "Property Editor Auto Hide", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelAutoHide->setToolTip(QApplication::translate("CActionManager", "Property Editor Auto Hide", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actToolPanelCollapsible->setText(QApplication::translate("CActionManager", "Property Editor Collapsible", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actToolPanelCollapsible->setToolTip(QApplication::translate("CActionManager", "Property Editor Collapsible", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actShowGrids->setText(QApplication::translate("CActionManager", "Show Grids", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actShowGrids->setToolTip(QApplication::translate("CActionManager", "Show Grids", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actPresentation->setText(QApplication::translate("CActionManager", "Toggle Presentation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actPresentation->setToolTip(QApplication::translate("CActionManager", "Toggle Presentation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_actBrainStorm->setText(QApplication::translate("CActionManager", "Toggle Brain Storm", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actBrainStorm->setToolTip(QApplication::translate("CActionManager", "Toggle Brain Storm", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actNext->setText(QApplication::translate("CActionManager", "Next", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actNext->setToolTip(QApplication::translate("CActionManager", "Next File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actPrev->setText(QApplication::translate("CActionManager", "Prev", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actPrev->setToolTip(QApplication::translate("CActionManager", "Prev File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actCloseFile->setText(QApplication::translate("CActionManager", "Close", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actCloseFile->setToolTip(QApplication::translate("CActionManager", "Close File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actSaveAsTemplate->setText(QApplication::translate("CActionManager", "Save As Template", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actSaveAsTemplate->setToolTip(QApplication::translate("CActionManager", "Save As Template", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

	m_actLoadTemplatePackage->setText(QApplication::translate("CActionManager", "Load Custom Templates", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actLoadTemplatePackage->setToolTip(QApplication::translate("CActionManager", "Load Custom Templates", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
	
	m_actBrowseTemplates->setText(QApplication::translate("CActionManager", "Manage Templates", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
    m_actBrowseTemplates->setToolTip(QApplication::translate("CActionManager", "Manage Templates", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
}

void CActionManager::onFileSaveStatusChanged(bool saved)
{
	m_actSaveFile->setEnabled(!saved);
}

void CActionManager::onCanRedoChanged(bool value)
{
	m_actRedo->setEnabled(value);
}

void CActionManager::onCanUndoChanged(bool value)
{
	m_actUndo->setEnabled(value);
}

void CActionManager::onClipboardChanged(QClipboard::Mode mode)
{
	QClipboard	*clipboard = QApplication::clipboard();

	if (mode == QClipboard::Clipboard)
	{
		if (clipboard->text() != QString("") ||
			!clipboard->image().isNull() ||
			!clipboard->pixmap().isNull())
		{
			m_actPaste->setEnabled(true);
		}
		else
		{
			m_actPaste->setEnabled(false);
		}
	}
}

void CActionManager::onSelectionChanged(QList<CDiagramItem *> &items, const QRectF &rect)
{
	int		length = 0;

	length = items.length();

	m_actCopy->setEnabled(length != 0);
	m_actCut->setEnabled(length != 0);
    m_actDelete->setEnabled(length != 0);

	m_actTextAlignLeft->setEnabled(length != 0);
	m_actTextAlignRight->setEnabled(length != 0);
	m_actTextAlignCenter->setEnabled(length != 0);
    m_actTextAlignJustify->setEnabled(length != 0);
}

void CActionManager::onDiagramChanged(CDiagram *curr, CDiagram *prev)
{
	bool	enabled = (curr != NULL);
	
	if (prev)
	{
		disconnect(prev->undoStack(), SIGNAL(canRedoChanged(bool)), this, SLOT(onCanRedoChanged(bool)));
		disconnect(prev->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(onCanUndoChanged(bool)));
		disconnect(prev, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	}
	
	if (!enabled)
	{
		m_actCopy->setEnabled(false);
		m_actPaste->setEnabled(false);
		m_actCut->setEnabled(false);
		m_actDelete->setEnabled(false);
		m_actRedo->setEnabled(false);
		m_actUndo->setEnabled(false);
		m_actSelectAll->setEnabled(false);
        m_actTextAlignLeft->setEnabled(false);
		m_actTextAlignRight->setEnabled(false);
		m_actTextAlignCenter->setEnabled(false);
		m_actTextAlignJustify->setEnabled(false);
        m_actSaveFile->setEnabled(false);
	}

	m_actSaveAs->setEnabled(enabled);
	m_actSelectAll->setEnabled(enabled);
	m_actPrint->setEnabled(enabled);
	m_actPrintPreview->setEnabled(enabled);
	m_actExport->setEnabled(enabled);
	m_actPresentation->setEnabled(enabled);
    m_actBrainStorm->setEnabled(enabled);
	m_actNext->setEnabled(enabled);
	m_actPrev->setEnabled(enabled);
	m_actCloseFile->setEnabled(enabled);
	m_actSaveAsTemplate->setEnabled(enabled);

	if (curr)
	{
		connect(curr->undoStack(), SIGNAL(canRedoChanged(bool)), this, SLOT(onCanRedoChanged(bool)));
		connect(curr->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(onCanUndoChanged(bool)));
		connect(curr, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	}
}

void CActionManager::onFileChanged(CDiagramFile *file)
{
    m_actSaveFile->setEnabled(file->isUnsaved());
}

void CActionManager::onSettingChanged(const QString &key)
{
    bool                r = false;
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (key == Settings::MindMapBackgroundShowGrid)
    {
        r = setting->get(Settings::MindMapBackgroundShowGrid, QVariant(false)).toBool();
        if (r != m_actShowGrids->isChecked())
            m_actShowGrids->setChecked(r);
    }
    else if (key == Settings::MindMapToolBarAutoHide)
    {
        r = setting->get(Settings::MindMapToolBarAutoHide, QVariant(true)).toBool();
        if (r != m_actToolPanelAutoHide->isChecked())
            m_actToolPanelAutoHide->setChecked(r);
    }
    else if (key == Settings::MindMapToolBarCollapsible)
    {
        r = setting->get(Settings::MindMapToolBarCollapsible, QVariant(true)).toBool();
        if (r != m_actToolPanelCollapsible->isChecked())
            m_actToolPanelCollapsible->setChecked(r);
    }
    else if (key == Settings::MindMapToolBarAlignment)
    {
        int                 anchor = -1;
        QList<QAction*>     actions;

        anchor = setting->get(Settings::MindMapToolBarAlignment, QVariant(CDiagramWidget::AnchorToSceneRight)).toInt();
        actions = m_toolPanelAnchorGroup->actions();
        foreach (QAction *action, actions)
        {
            if (action->data().toInt() == anchor &&
                !action->isChecked())
            {
                action->setChecked(true);
                break;
            }
        }
    }
    else
    {

    }
}

void CActionManager::onPropertyEditorAnchorActionTriggered(QAction *action)
{
    int                 type = -1;
    CMindMapSetting     *setting = NULL;

    type = action->data().toInt();
    setting = CMindMapSetting::instance();
    if (setting)
    {
        setting->set(Settings::MindMapToolBarAlignment, QVariant(type));
    }
}

void CActionManager::onPropertyEditorAutoHide(bool value)
{
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (setting)
    {
        setting->set(Settings::MindMapToolBarAutoHide, QVariant(value));
    }
}

void CActionManager::onPropertyEditorCollapsible(bool value)
{
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (setting)
    {
        setting->set(Settings::MindMapToolBarCollapsible, QVariant(value));
    }
}
