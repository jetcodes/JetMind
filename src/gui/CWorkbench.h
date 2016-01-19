/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CWORKBENCH_H
#define PRODRAW_CWORKBENCH_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramFile>
#include <core/CDiagramEditor>
#include <widgets/CUiWidget>

class CDiagramFile;
class CDiagramFileManager;
class CDiagramContextMenu;
class CDiagramNodeStyle;

class CUiBusyIndicator;

class CCanvas;
class CCoolBar;
class CActionManager;
class CAboutDialog;
class CHelpDialog;
class CTemplateCreationDialog;
class CNewDiagramDialog;

// put the style setting here
#if defined(__APPLE__) && defined(__MACH__)
	#include "CUiWidget_mac.h"
	#define CUiWidgetBase	CUiWidget_mac
#else
  #include "CTitleBar.h"
	#include "CUiWidget_win.h"
	#define CUiWidgetBase	CUiWidget_win
#endif //

class CWorkbench : public CUiWidgetBase
{
	Q_OBJECT

public:
	CWorkbench(QWidget *parent = 0);
	virtual ~CWorkbench();

	CDiagramFileManager* fileManager();
	CActionManager* actionManager();

	// load specified file to edit
	bool loadFile(const QString &fullPath);

	// create a brand new, empty file
	bool createFile();

	// styling the whole bench
  void loadStyleSheet();

	// data model
	QAbstractItemModel* dataModel(bool curentFile = true);

Q_SIGNALS:
    void fileChanged(CDiagramFile *file);
    void diagramAdded(CDiagram* d);
	void diagramRemoved(CDiagram*);
	void diagramChanged(CDiagram* cur, CDiagram *old);
	void viewDiagram(CDiagram*);
    void docTitleChanged(const QString &title);

public Q_SLOTS:
    void onSupport();
    void onUpdate();
	void onHelp();
	void onAbout();

	void onNewFile();
	void onNewDiagram();
    void onRemoveDiagram();
	void onOpenFile();
	void onSaveFile();
	void onSaveFileAs();
	void onLoadFile(const QString &);

	void onSaveFileAsTemplate();
	void onShowAddTemplateDialog();
	void onLoadTemplatePackage();
	
	void onExport();
	void onPrint();
	void onPrintPreview();
	void onPrint(QPrinter*);
	
	void onExit();

    bool onCloseFile(CDiagramFile *);
	void onCloseCurrentFile();
	void onViewFile(CDiagramFile *);
	void onViewDiagram(CDiagram *);
    void onViewNextFile(int step);
    void onViewNextDiagram(int step);

	// Diagram mode controls
    void onFilterNodes(const QString &);

	void onInsertLinkMode(bool);
	void onInsertTextMode(bool);
	void onCursorMode();
    void onInsertFreeTopic();

	void onGroup();
	void onUnGroup();
	
	void onUndo();
	void onRedo();

	void onSelectAll();
	void onCopy();
	void onPaste();
	void onCut();
	void onDelete();
	
	// Global Settings
	void onShowGrids(bool);
    void onShowToolBar(bool);
	void onShowCanvasSetupPanel();
    void onShowCanvasQuickView();
	
	// File related
	void onFileSaveStatusChanged(bool saved);
    void onRecentFileTriggered(QAction*);
	
	// View mode
    void onPresentation();
    void onBrainStorm();

	// Threading
	void onJobDone();

	// Url
	void onOpenUrl(QUrl);

protected:
	void updateRecentFileList(const QString &fullPath);

    // Diagram related inner functions
	void setCurrentFile(CDiagramFile *file);
	CDiagramFile* currentFile();
	
	// Override key event handle function
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);

	// Override function to handle full screen preparation
	// and clean job.
	bool beforeEnterFullScreen();
	bool afterExitFullScreen();

	// Utils member functions
	void updateTitles(const QString &fileName, bool saved = false);
	QString getFileNameViaDialog(const QString &title, const QString &initPath, const QString &filter, bool saveDialog = true);

private:
	// Widegts
	CCoolBar					*m_toolbar;
	CCanvas						*m_canvas;
	CNewDiagramDialog			*m_newDiagramDialog;
	CTemplateCreationDialog		*m_addTemplateDialog;
	CAboutDialog				*m_aboutDialog;
	CHelpDialog					*m_helpDialog;

    QVBoxLayout					*m_globalLayout;

	// Managers
	CDiagramFileManager			*m_fileManager;
	CActionManager				*m_actionManager;
	int							m_recentFilesLimit;

	  CDiagramFile				*m_file;
    CDiagram                    *m_diagram;
	  QUndoGroup					*m_undoGroup;

	  CUiBusyIndicator			*m_busyIndicator;
	  QFutureWatcher<void>		*m_futureWatcher;

    CDiagramContextMenu         *m_operationMenu;
    CDiagramContextMenu         *m_demoMenu;

    bool						m_focusMode;

}; // End of class CWorkbench

#endif // PRODRAW_CWORKBENCH_H
