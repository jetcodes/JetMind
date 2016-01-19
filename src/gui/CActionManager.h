/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CACTIONMANAGER_H
#define PRODRAW_CACTIONMANAGER_H

#include <QtGui>

class CDiagram;
class CDiagramFile;
class CDiagramItem;
class CWorkbench;

class CActionManager : public QObject
{
	Q_OBJECT

public:
	CActionManager(CWorkbench *bench);
	virtual ~CActionManager();

	void initActions();
	void retranslateUi();

public Q_SLOTS:
	void onClipboardChanged(QClipboard::Mode mode);
	void onFileSaveStatusChanged(bool saved);
	void onCanRedoChanged(bool value);
	void onCanUndoChanged(bool value);
	void onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect);
	void onDiagramChanged(CDiagram *curr, CDiagram *prev);
    void onPropertyEditorAnchorActionTriggered(QAction *action);
    void onPropertyEditorAutoHide(bool);
    void onPropertyEditorCollapsible(bool);
    void onSettingChanged(const QString &key);
    void onFileChanged(CDiagramFile *file);

public:
	QAction *m_actNewFile;
    QAction *m_actNewSheet;
    QAction *m_actSaveFile;
    QAction *m_actOpenFile;
    QAction *m_actSaveAs;
    QAction *m_actPrint;
    QAction *m_actPrintPreview;
    QAction *m_actExit;
    QAction *m_actRedo;
    QAction *m_actUndo;
    QAction *m_actCopy;
    QAction *m_actPaste;
    QAction *m_actDelete;
    QAction *m_actSelectAll;
    QAction *m_actTextAlignLeft;
    QAction *m_actTextAlignRight;
    QAction *m_actTextAlignCenter;
    QAction *m_actTextAlignJustify;
    QAction *m_actBoundary;
    QAction *m_actSummary;
    QAction *m_actManual;
    QAction *m_actAboutRemind;
    QAction *m_actExport;
    QAction *m_actCut;
    QAction *m_actSupport;
    QAction *m_actUpdate;
	QAction *m_actNext;
	QAction *m_actPrev;
	QAction *m_actCloseFile;

	QAction *m_actSaveAsTemplate;
	QAction *m_actLoadTemplatePackage;
	QAction *m_actBrowseTemplates;

    QAction *m_actShowToolBar;
    QAction	*m_actPresentation;
    QAction *m_actBrainStorm;

	QAction *m_actShowCanvasSetupPanel;
    QAction *m_actShowCanvasQuickView;
    QAction *m_actShowGrids;
    QAction	*m_actToolPanelAnchorLeft;
    QAction *m_actToolPanelAnchorRight;
    QAction *m_actToolPanelAnchorSelectedBymbol;
    QAction *m_actToolPanelFreeFloat;
    QAction *m_actToolPanelCollapsible;
    QAction *m_actToolPanelAutoHide;


    QActionGroup    *m_toolPanelAnchorGroup;
	QList<QAction*>	m_actRecentFiles;

}; // End of class CActionManager

#endif // PRODRAW_CACTIONMANAGER_H
