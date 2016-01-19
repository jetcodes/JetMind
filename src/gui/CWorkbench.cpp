/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <QSvgGenerator>

#include <core/CDiagram>
#include <core/CDiagramWidget>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>
#include <core/CDiagramNode>
#include <core/CDiagramContext>
#include <core/CDiagramContextMenuHandler>
#include <core/CDiagramTemplateManager>
#include <core/CDiagramCommands>

#include <widgets/CUiMessageBox>
#include <widgets/CUiBusyIndicator>

#include <mindmap/CMindMapNode>
#include <mindmap/CMindMapFile>
#include <mindmap/CMindMapSettings>

#include "CAppSettings.h"
#include "CCanvas.h"
#include "CCoolBar.h"
#include "CGraphMenu.h"
#include "CWorkbench.h"
#include "CAboutDialog.h"
#include "CHelpDialog.h"
#include "CNewDiagramDialog.h"
#include "CTemplateCreationDialog.h"
#include "CActionManager.h"

// runners
static void saveFileRunner(CDiagramFile *file)
{
    if (!file)
		return;

    file->save();
}

static void saveFileAsRunner(CDiagramFile *file, QString &fileName, QString &filePath)
{
    if (!file)
		return;

    file->save( QDir(filePath).absoluteFilePath(fileName) );
}

CWorkbench::CWorkbench(QWidget *parent)
	: CUiWidgetBase(parent),
	  m_toolbar(NULL),
      m_canvas(NULL),
      m_globalLayout(NULL),
      m_newDiagramDialog(NULL),
      m_aboutDialog(NULL),
      m_helpDialog(NULL),
      m_focusMode(true),
      m_addTemplateDialog(NULL),
      m_busyIndicator(NULL),
      m_futureWatcher(NULL),
	  m_file(NULL),
      m_diagram(NULL),
      m_fileManager(NULL),
      m_actionManager(NULL),
	  m_undoGroup(NULL),
      m_recentFilesLimit(5),
      m_operationMenu(NULL),
      m_demoMenu(NULL)
{
	QSettings	settings;

    m_undoGroup = new QUndoGroup(this);
	m_actionManager = new CActionManager(this);
    m_fileManager = CDiagramFileManager::instance();

	m_toolbar = new CCoolBar(this);
	m_canvas = new CCanvas(this);
	m_newDiagramDialog = new CNewDiagramDialog(this);
	m_addTemplateDialog = new CTemplateCreationDialog(this);
	m_aboutDialog = new CAboutDialog(this);
	m_helpDialog = new CHelpDialog(this);

	m_globalLayout = new QVBoxLayout();
	m_globalLayout->setMargin(0);
	m_globalLayout->setSpacing(0);

	m_globalLayout->addWidget(m_toolbar, 0);
	m_globalLayout->addWidget(m_canvas, 1);
	
	m_toolbar->setVisible( settings.value(Settings::ShowToolBar, QVariant(true)).value<bool>() );
	m_toolbar->toggleFashionMode(true);

	m_busyIndicator = new CUiBusyIndicator(this);
    m_busyIndicator->resize(128,128);

	m_futureWatcher = new QFutureWatcher<void>();

    m_operationMenu = new CGraphOperationMenu(this);
    REGISTER_CONTEXT_MENU(CDiagram::CursorMode, m_operationMenu);

    m_demoMenu = new CGraphDemoMenu(this);
    REGISTER_CONTEXT_MENU(CDiagram::TagMode, m_demoMenu);

	// initialize the layout
	setupUi(m_globalLayout, m_actionManager);

    connect(m_addTemplateDialog, SIGNAL(saveTemplate()), this, SLOT(onSaveFileAsTemplate()));
	connect(m_futureWatcher, SIGNAL(finished()), this, SLOT(onJobDone()));
}

CWorkbench::~CWorkbench()
{
	
}

CDiagramFileManager* CWorkbench::fileManager()
{
    return m_fileManager;
}

CActionManager* CWorkbench::actionManager()
{
    return m_actionManager;
}

bool CWorkbench::loadFile(const QString &fullPath)
{
	QFileInfo		info(fullPath);
	CDiagramFile	*file = NULL;

	if (!m_fileManager)
		return false;

	if (m_fileManager->isOpened(fullPath))
	{
		CUiMessageBox::information(this, tr("Alert"), tr("The file has already been opened"));
		return false;
	}

	if (info.exists() && info.isFile())
	{
		file = m_fileManager->create( QString( CMindMapFile::staticMetaObject.className() ) );
		if (file)
		{
            // load file content from specified location
			file->load(fullPath);

			m_fileManager->updateFileInfo(file);
			setCurrentFile(file);

			updateRecentFileList( fullPath );
		}
		return true;
	}

	return false;
}

bool CWorkbench::createFile()
{
    CDiagramFile	*file = NULL;

	file = m_fileManager->create( QString( CMindMapFile::staticMetaObject.className() ) );
	if (file)
	{
        // use default initialization
		file->initDefault();

		m_fileManager->updateFileInfo(file);
		setCurrentFile(file);
		file->setUnsaved(false);
        return true;
    }
    else
    {
        return false;
    }
}

void CWorkbench::onNewFile()
{
	CDiagramFile			*file = NULL;
	CDiagramTemplate		*tpl = NULL;
	CDiagramTemplateManager	*tplManager = NULL;
	CMindMapFile			*mindmapFile = NULL;
	CMindMapGraph			*mindmapGraph = NULL;

	m_newDiagramDialog->setup();
    m_newDiagramDialog->setTitle(tr("Create File"));
    if (m_newDiagramDialog->exec() != QDialog::Accepted)
		return;

	file = m_fileManager->create(QString( CMindMapFile::staticMetaObject.className() ));
	tpl = m_newDiagramDialog->selectedTemplate();
	tplManager = CDiagramTemplateManager::instance();
	
	if (file)
	{
        // if the new file is create using template, then
        // nothing is done here, or we use the default initialization
        if (!tpl)
		{
            file->initDefault();
		}
		else
		{
			mindmapFile = dynamic_cast<CMindMapFile*>(file);
			if (mindmapFile)
			{
				mindmapGraph = dynamic_cast<CMindMapGraph*>( tplManager->createDiagram(tpl) );
				if (mindmapGraph)
				{
					mindmapFile->addGraph( mindmapGraph );
					mindmapGraph->onRebuildGraph();
				}
			}
		}

		m_fileManager->updateFileInfo(file);
		setCurrentFile(file);
	}
}

void CWorkbench::onNewDiagram()
{
	CMindMapFile				*file = NULL;
	CMindMapGraph				*graph = NULL;
	CDiagramTemplate			*tpl = NULL;
	CDiagramTemplateManager		*tplManager = NULL;
	
	m_newDiagramDialog->setup();
    m_newDiagramDialog->setTitle(tr("Add Sheet to Current File"));
    if (m_newDiagramDialog->exec() != QDialog::Accepted)
		return;

	tpl = m_newDiagramDialog->selectedTemplate();
	file = dynamic_cast<CMindMapFile*>(m_file);
    if (!file)
        return;

    // create from template or not
    if (tpl)
	{
		tplManager = CDiagramTemplateManager::instance();
		if (tplManager)
		{
            graph = dynamic_cast<CMindMapGraph*>( tplManager->createDiagram(tpl) );
			if (graph)
            {
                file->addGraph(graph);
				graph->onRebuildGraph();
			}
		}
	}
    else
    {
        graph = new CMindMapGraph(file);
        graph->initDefault();
    }

    if (graph)
    {
        m_undoGroup->addStack(graph->undoStack());
		file->setEditDiagram(graph);
        onViewDiagram(graph);

        emit diagramAdded(graph);
    }
}

void CWorkbench::onRemoveDiagram()
{
	int					btn = 0;
    CMindMapGraph       *graph = NULL;
    CMindMapFile        *file = NULL;

    file = dynamic_cast<CMindMapFile*>(m_file);
    graph = dynamic_cast<CMindMapGraph*>(m_diagram);
    if (!file || !graph)
        return;

    if (file->graphCount() == 1)
    {
        CUiMessageBox::information(this, tr("Notification"), tr("This is the last sheet, please keep it. ^^"));
        return;
    }
	else
	{
		btn = (int) CUiMessageBox::question(this, 
								   tr("Confirmation"), 
                                   tr("Are you sure to remove this sheet? It can't be undo."),
								   CUiMessageBox::Yes | CUiMessageBox::Cancel);
		if (btn == CUiMessageBox::Yes)
		{
			file->removeGraph(graph);
			emit diagramRemoved(graph);

			onViewDiagram(file->getEditDiagram());
		}
		else
		{
			// Nothing
		}
	}
}

void CWorkbench::onOpenFile()
{
    QSettings		settings;
	QString			initPath, fullPath;

	initPath = settings.value(Settings::LastLocation, QVariant(".")).value<QString>();
    fullPath = getFileNameViaDialog(tr("Open JetMind File"), initPath, AppConfig::JetMindFileString, false);
	if (!fullPath.isEmpty())
	{
        // load the specified file using "loadFile" method
		loadFile(fullPath);
	}
}

void CWorkbench::onSaveFile()
{
    QPoint          p;
	QString			initPath, fileName;
	QFileInfo		info;
	CDiagramFile	*file = NULL;
    QSettings		settings;
    QGraphicsView   *view = NULL;

	file = m_fileManager->getCurrentFile();
    if (!file || !m_diagram)
		return ;

    view = m_diagram->getMainView();
    if (view)
        p = view->mapTo(this, view->viewport()->geometry().center());
    else
        p = geometry().center();

	if (file->hasPhysicalFile())
	{
        m_busyIndicator->start( p );
		m_futureWatcher->setFuture(QtConcurrent::run(saveFileRunner, file));
	}
	else
	{
		initPath = settings.value(Settings::LastLocation, QVariant(".")).value<QString>();
        fileName = getFileNameViaDialog(tr("Save File"), initPath, AppConfig::JetMindFileString, true );

		if (!fileName.isEmpty())
		{
			info.setFile(fileName);

			// save it
            m_busyIndicator->start( p );
			m_futureWatcher->setFuture(QtConcurrent::run(saveFileAsRunner, file, info.fileName(), info.path() ) );
		}
	}
}

void CWorkbench::onSaveFileAs()
{
    QPoint          p;
    QString			initPath, fileName;
	QFileInfo		info;
	CDiagramFile	*file = NULL;
    QSettings		settings;
    QGraphicsView   *view = NULL;
	 
	file = m_fileManager->getCurrentFile();
    if (!file || m_diagram)
        return;

    view = m_diagram->getMainView();
    if (view)
        p = view->mapTo(this, view->viewport()->geometry().center());
    else
        p = geometry().center();

    initPath = settings.value(Settings::LastLocation, QVariant(".")).value<QString>();
    fileName = getFileNameViaDialog(tr("Save File As..."), initPath, AppConfig::JetMindFileString, true);
    if (!fileName.isEmpty())
    {
        info.setFile(fileName);

        // save it
        m_busyIndicator->start( p );
        m_futureWatcher->setFuture(QtConcurrent::run(saveFileAsRunner, file, info.fileName(), info.path() ) );

        m_fileManager->updateFileInfo(file);
        updateRecentFileList(info.absoluteFilePath());
        updateTitles(file->fileName(), true);
    }
}

void CWorkbench::onLoadFile(const QString &fullPath)
{
	loadFile(fullPath);
}

void CWorkbench::onShowAddTemplateDialog()
{
	if (m_addTemplateDialog && m_file)
	{
		m_addTemplateDialog->reset(m_file->fileName(), CDiagramContext::env()->diagramTypes());
		m_addTemplateDialog->exec();
	}
}

void CWorkbench::onSaveFileAsTemplate()
{
    QString						title, type;
	CDiagramTemplateManager		*tplManager = NULL;

    if (!m_diagram)
        return;

    tplManager = CDiagramTemplateManager::instance();
    if (tplManager)
	{
		title = m_addTemplateDialog->getTitle();
		type = m_addTemplateDialog->getType();
        tplManager->saveAsTemplate(m_diagram, type, title);
	}
}

void CWorkbench::onLoadTemplatePackage()
{
	int						count;
	QString					initPath;
	QString					fileName;
	QSettings				settings;
	CDiagramTemplateManager	*manager = NULL;

	initPath = settings.value(Settings::LastLocation, QVariant(".")).value<QString>();
	fileName = getFileNameViaDialog(tr("Load Template Package"), initPath, tr("Template Package File (*.zip)"), false);
	if (!fileName.isEmpty())
	{
		manager = CDiagramTemplateManager::instance();
		if (manager)
			count = manager->loadPackage(fileName);
        CUiMessageBox::information(this, tr("Templates loaded"), tr("Congratulations, there are <b>%1</b> templates loaded").arg(count));
	}
}

void CWorkbench::onExport()
{
  	QString					initPath, fileName, ext;
	QString					png, jpeg, svg, pdf, filter, sep(";;");
	QStringList				filters;
    QFileInfo				info;
	QPainter				painter;
	QSettings				settings;
    QMap<QString, QString>	suffixes;

    if (!m_diagram)
        return;

	png = tr("PNG (*.png)");
	jpeg = tr("JPEG (*.jpg;*.jpeg)");
	svg = tr("Scalable Vector Graphics (*.svg)");
	pdf = tr("Portable Document File (*.pdf)");
	suffixes.insert(png, "png");
	suffixes.insert(jpeg, "jpeg");
	suffixes.insert(svg, "svg");
	suffixes.insert(pdf, "pdf");

	initPath = settings.value(Settings::LastLocation, QVariant(".")).value<QString>();
    filters << png << svg << pdf;
	fileName = QFileDialog::getSaveFileName(this, tr("Export Diagram To..."), initPath, filters.join(sep), &filter );

    if (!fileName.isEmpty() && m_diagram)
	{
		info.setFile(fileName);
		ext = info.suffix();
		if (info.suffix().isEmpty())
			ext = suffixes[filter];

		if (ext == "svg")
		{
			QSvgGenerator	svg;
			svg.setFileName(fileName);
			painter.begin(&svg);
            m_diagram->drawAllItems(&painter);
			painter.end();
		}
		else if (ext == "pdf")
		{
			QRectF			r;
			QPrinter		printer(QPrinter::ScreenResolution);
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOutputFileName(info.absoluteFilePath());

			QPainter		pdfPainter(&printer);

            r = m_diagram->printArea();

            pdfPainter.setRenderHint(QPainter::Antialiasing, false);
            m_diagram->drawArea(&pdfPainter, r);
		}
		else
		{
			QRectF		r = m_diagram->printArea();
            QImage		image(r.size().toSize(), QImage::Format_ARGB32_Premultiplied);
			
			image.fill(Qt::transparent);
			painter.begin(&image);
			painter.setRenderHint(QPainter::Antialiasing, true);
            
			m_diagram->drawArea(&painter, r);

			painter.end();
			
			image.save(fileName);
		}

		settings.setValue(Settings::LastLocation, QVariant(info.path()));
		CUiMessageBox::information(this, tr("Export Done"), tr("Exporting has been finished successfully."), CUiMessageBox::Ok);
	}
	else
	{
		// NOTE: information to be shown?
	}	
}

void CWorkbench::onPrint()
{
	QPrinter		printer(QPrinter::ScreenResolution);
	QPrintDialog	*dlg = new QPrintDialog(&printer, this);

    if (!m_diagram)
        return;

	printer.setPaperSize(QPrinter::A4);
	if (dlg->exec() != QDialog::Accepted)
		return;

	onPrint(&printer);
}

void CWorkbench::onPrintPreview()
{
	QPrinter				printer(QPrinter::ScreenResolution);
	QPrintPreviewDialog		preview(&printer, this);
	
    if (!m_diagram)
        return;

    printer.setPaperSize(QPrinter::A4);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(onPrint(QPrinter*)));
    preview.setSizeGripEnabled(true);
    preview.resize(800, 600);
    preview.exec();
}

void CWorkbench::onPrint(QPrinter *printer)
{
	QRectF				r;
    QPainter			painter(printer);

    if (!m_diagram)
        return;

    r = m_diagram->printArea();
	painter.setRenderHint(QPainter::Antialiasing, true);
    m_diagram->drawArea(&painter, r);
}

void CWorkbench::onExit()
{
	close();
}

void CWorkbench::onViewNextFile(int step)
{
    CDiagramFile		*file = NULL;

    file = m_fileManager->getNextFile(step);
    if (file)
	{
        setCurrentFile( file );
	}
}

void CWorkbench::onViewNextDiagram(int step)
{
    int                     index = -1, nextIndex = -1;
    CMindMapFile            *file = NULL;
    CMindMapGraph           *graph = NULL, *nextGraph = NULL;
    QList<CMindMapGraph*>   graphes;

    if (step == 0)
        return;

    file = dynamic_cast<CMindMapFile*>(m_file);
    graph = dynamic_cast<CMindMapGraph*>(m_diagram);
    if (file && graph)
    {
        graphes = file->graphes();
        index = graphes.indexOf( graph );
        while (index >= 0)
        {
            nextIndex = (index + step);
            nextIndex = qMin(qMax(0, nextIndex), graphes.count() - 1);
            if (graphes[nextIndex] == m_diagram)
            {
                nextIndex = -1;
                break;
            }
            else if (graphes[nextIndex]->hidden())
            {
                index = nextIndex;
                if (nextIndex == 0 || nextIndex == graphes.count() - 1)
                {
                    nextIndex = -1;
                    break;
                }
                else
                {
                    continue;
                }

            }
            else
            {
                break;
            }
        }
    }

    if (nextIndex >= 0 && nextIndex <= graphes.count() - 1)
    {
        nextGraph = graphes[nextIndex];
        onViewDiagram(nextGraph);
    }
}

void CWorkbench::onCloseCurrentFile()
{
    if (!m_file)
        return;

    if (!onCloseFile(m_file))
        return;

    m_file = NULL;

    // switch to another diagram
    m_canvas->closeDiagram();
    m_diagram = NULL;

    if (m_fileManager->fileCount() > 0)
    {
        setCurrentFile(m_fileManager->files().first());
    }
}

bool CWorkbench::onCloseFile(CDiagramFile *file)
{
    int							btn;
	bool						close = true;
	QString						title(tr("Confirmation"));
    QString						tpl(tr("The file <b>[%1]</b> is unsaved from last modification, save it now?"));

    if (!file)
        return false;

    if (file->isUnsaved())
    {
        btn = (int) CUiMessageBox::question(this, title, tpl.arg( file->fileName() ), CUiMessageBox::Discard | CUiMessageBox::Save | CUiMessageBox::Cancel);
        if ( btn == CUiMessageBox::Save)
        {
            onSaveFile();
            if (file->isUnsaved())
                close = false;
        }
        else
        {
            if (!(btn == CUiMessageBox::Discard))
            {
                close = false;
            }
        }
    }

    if (close)
    {
        m_fileManager->closeFile(file);
    }

    return close;
}

void CWorkbench::onViewFile(CDiagramFile *file)
{
    if (!file || file == m_file)
        return;

    setCurrentFile(file);
}

void CWorkbench::onViewDiagram(CDiagram *diagram)
{
    CDiagram    *prev = NULL;

    if (diagram == m_diagram)
        return;

    prev = m_diagram;
    if (diagram)
    {
        if (isFullScreen() && !m_focusMode)
            diagram->setMode(CDiagram::TagMode);
        else
            diagram->setMode(CDiagram::CursorMode);

        m_canvas->viewDiagram(diagram);
        m_undoGroup->setActiveStack(diagram->undoStack());
    }

    m_diagram = diagram;

	emit viewDiagram(m_diagram);
    emit diagramChanged(m_diagram, prev);
}

void CWorkbench::onCopy()
{
    QEvent	*event = NULL;

    if (!m_diagram)
		return;

    event = new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier);
    QApplication::sendEvent(m_diagram, event);
	SAFE_DELETE(event);
}

void CWorkbench::onPaste()
{
    QEvent	*event = NULL;

    if (!m_diagram)
        return;

    event = new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier);
    QApplication::sendEvent(m_diagram, event);
    SAFE_DELETE(event);
}

void CWorkbench::onCut()
{
	QEvent	*event = NULL;

	event = new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier);
	if (!m_diagram || !event) 
		return;
	
	QApplication::sendEvent(m_diagram, event);
	SAFE_DELETE(event);
}

void CWorkbench::onDelete()
{
	QEvent	*event = NULL;

	event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
	if (!m_diagram || !event) 
		return;

	QApplication::sendEvent(m_diagram, event);
	SAFE_DELETE(event);
}

void CWorkbench::onSelectAll()
{
	if (!m_diagram) 
		return;
	m_diagram->selectAll();
}

void CWorkbench::onUndo()
{
	if (!m_diagram) 
		return;
	m_undoGroup->undo();
}

void CWorkbench::onRedo()
{
	if (!m_diagram) 
		return;
	m_undoGroup->redo();
}

void CWorkbench::onGroup()
{
	QEvent	*event = NULL;

	event = new QKeyEvent(QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier);
	if (!m_diagram || !event) 
		return;

	QApplication::sendEvent(m_diagram, event);
	SAFE_DELETE(event);
}

void CWorkbench::onUnGroup()
{
	QEvent	*event = NULL;

	event = new QKeyEvent(QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier & Qt::ShiftModifier);
	if (!m_diagram || !event) 
		return;

	QApplication::sendEvent(m_diagram, event);
	SAFE_DELETE(event);
}

void CWorkbench::onHelp()
{
	if (m_helpDialog == NULL)
	{
		m_helpDialog = new CHelpDialog(this);
	}
	m_helpDialog->show();
}

void CWorkbench::onAbout()
{
	if (m_aboutDialog == NULL)
	{
		m_aboutDialog = new CAboutDialog(this);
	}
	m_aboutDialog->exec();
}

void CWorkbench::onCursorMode()
{
	if (!m_diagram) return;
	m_diagram->setMode(CDiagram::CursorMode);
}

void CWorkbench::onInsertFreeTopic()
{
    CMindMapNode    *node = NULL;

    if (!m_diagram)
        return;

    node = new CMindMapNode(m_diagram);
    node->setMovable(true);
    m_diagram->insertItem(node, QPointF(100,100));
}

void CWorkbench::onFilterNodes(const QString &filter)
{
    CMindMapGraph   *graph = NULL;

    graph = dynamic_cast<CMindMapGraph*>(m_diagram);
    if (!graph)
        return;

	graph->filterNodes(filter);
}

void CWorkbench::onInsertLinkMode(bool multiMode)
{
	if (!m_diagram) return;
	m_diagram->setMode( multiMode ? CDiagram::AddMultipleItemMode : CDiagram::AddSingleItemMode );
	m_diagram->setNewItemMeta( m_toolbar->currItemMeta() );
}

void CWorkbench::onInsertTextMode(bool multiMode)
{
	if (!m_diagram) return;
	m_diagram->setMode( multiMode ? CDiagram::AddMultipleItemMode : CDiagram::AddSingleItemMode );
	m_diagram->setNewItemMeta( m_toolbar->currItemMeta() );
}

void CWorkbench::setCurrentFile(CDiagramFile *file)
{
    CDiagram                *prevDiagram = NULL, *diagram = NULL;
    QList<QUndoStack*>      stacks;

    if (!file || file == m_file)
    {
        // SHOULD GO TO NEXT DIAGRAM FILE
		return;
    }

    if (m_file)
    {
        stacks = m_file->undoStacks();
        prevDiagram = m_file->getEditDiagram();

        if (stacks.length() > 0)
        {
            foreach (QUndoStack *stack, stacks)
            {
                if (m_undoGroup->stacks().contains(stack))
                    m_undoGroup->removeStack( stack );
            }
        }

        disconnect(m_file, SIGNAL(saveStatusChanged(bool)), this, SLOT(onFileSaveStatusChanged(bool)));
    }

    // set current file to new file
    m_file = file;
    if (m_file)
    {
        m_fileManager->setCurrentFile(m_file);
        stacks = m_file->undoStacks();
        if (stacks.length() > 0)
        {
            foreach (QUndoStack *stack, stacks)
            {
                if (!m_undoGroup->stacks().contains(stack))
                    m_undoGroup->addStack( stack );
            }
        }
        updateTitles(m_file->fileName(), !m_file->isUnsaved());
        connect(m_file, SIGNAL(saveStatusChanged(bool)), this, SLOT(onFileSaveStatusChanged(bool)));

		emit fileChanged(file);

        // set the status to the specified diagram
        diagram = m_file->getEditDiagram();
        onViewDiagram(diagram);
    }
    else
    {
		updateTitles("");
        m_diagram = NULL;
	}

    emit diagramChanged(diagram, prevDiagram);
}

void CWorkbench::onFileSaveStatusChanged(bool saved)
{
    QString		title;
	
    if (!m_file)
        return;

    title = m_file->fileName();
    updateTitles(title, saved);

	m_actionManager->onFileSaveStatusChanged(saved);
}

void CWorkbench::onRecentFileTriggered(QAction *act)
{
    QString         filePath;

    if (!act)
        return;

    filePath = act->data().toString();
    loadFile(filePath);
}

bool CWorkbench::beforeEnterFullScreen()
{
    CMindMapFile                *file = NULL;
    CMindMapGraph               *graph = NULL;
    QList<CMindMapGraph*>       graphes;

    CUiWidgetBase::beforeEnterFullScreen();

    file = dynamic_cast<CMindMapFile*>(m_file);
    if (!m_focusMode && file)
	{
        m_toolbar->hide();
        m_canvas->setFullScreenMode(true);

        graphes = file->graphes();
        foreach (graph, graphes)
        {
            graph->setMode( CDiagram::TagMode );
        }
	}
	else if (m_focusMode)
	{
		m_toolbar->lockFocus(true);
	}

	return true;
}

bool CWorkbench::afterExitFullScreen()
{
    CMindMapFile            *file = NULL;
    CMindMapGraph           *graph = NULL;
    QList<CMindMapGraph*>   graphes;
    CDiagram                *diagram = NULL;
    CDiagramEditor			*editor = NULL;

    CUiWidgetBase::afterExitFullScreen();

	m_toolbar->show();
	m_toolbar->lockFocus(true);
	m_canvas->setFullScreenMode(false);

    file = dynamic_cast<CMindMapFile*>(m_file);
    if (!file)
        return true;

    // reset all the graphes
    graphes = file->graphes();
    foreach (graph, graphes)
    {
        graph->setMode( CDiagram::CursorMode );
    }

    // reset the editor
    diagram = file->getEditDiagram();
    if (diagram)
    {
        editor = dynamic_cast<CDiagramEditor*>( diagram->getMainView() );
        if (editor)
        {
            editor->setDragMode(QGraphicsView::NoDrag);
            editor->setCursor(Qt::ArrowCursor);
        }
    }

	m_focusMode = false;
	return true;
}

void CWorkbench::keyPressEvent(QKeyEvent *event)
{
	if (isFullScreen() && event->key() == Qt::Key_Escape)
	{
        onFullScreen();
	}
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void CWorkbench::closeEvent(QCloseEvent *event)
{
	bool					exited = true;
	int						answer;
	QString					title(tr("Confirmation"));
    QString					tpl(tr("The file <b>[%1]</b> is unsaved from last modification, save it now?"));
	CDiagramFile			*file = NULL;
	QList<CDiagramFile*>	files;

    if (m_fileManager->hasUnsavedFile())
	{
		files = m_fileManager->files();
		for (int i = 0;i < files.length(); ++i)
		{
			file = files.at(i);
			m_fileManager->setCurrentFile(file);
			if (file->isUnsaved())
			{
				answer = (int) CUiMessageBox::question(this, title, tpl.arg( file->fileName() ), CUiMessageBox::Discard | CUiMessageBox::Save | CUiMessageBox::Cancel);
				if ( answer == CUiMessageBox::Save)
				{
					onSaveFile();
					if (file->isUnsaved())
					{
						exited = false;
						break;
					}
				}
				else
				{
					 if (!(answer == CUiMessageBox::Discard))
					 {
						exited = false;
						break;
					 }
				}
			}
            m_fileManager->closeFile(file);
		}
	}
	else
	{
        m_fileManager->closeAllFiles();
	}
	
	// Now quit the application
	if (exited)
	{
		event->accept();
	}
	else
		event->ignore();
}

void CWorkbench::onSupport()
{
	QSettings	settings;

    QDesktopServices::openUrl( settings.value(Settings::SupportUrl, QVariant(QUrl("mailto:support@jetcodes.io")) ).value<QUrl>() );
}

void CWorkbench::onUpdate()
{
	QSettings	settings;

    QDesktopServices::openUrl( settings.value(Settings::DownloadUrl, QVariant(QUrl("http://jetcodes.io/downloads")) ).value<QUrl>() );
}

void CWorkbench::onShowGrids(bool value)
{
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (setting)
    {
        setting->set(Settings::MindMapBackgroundShowGrid, QVariant(value));
        m_canvas->setGridsVisible(value);
    }
}

void CWorkbench::onShowToolBar(bool value)
{
	QSettings	settings;

	settings.setValue(Settings::ShowToolBar, QVariant(value));
	if (value != m_actionManager->m_actShowToolBar->isChecked())
		m_actionManager->m_actShowToolBar->setChecked(value);

	if (m_toolbar)
		m_toolbar->setVisible(value);
}

void CWorkbench::onShowCanvasSetupPanel()
{
	m_canvas->onShowCanvasSetupPanel();
}

void CWorkbench::onShowCanvasQuickView()
{
    m_canvas->onShowCanvasQuickView();
}

void CWorkbench::onPresentation()
{
    m_focusMode = false;
    onFullScreen();
    m_canvas->onMoveViewCenter( QPointF(0, 0) );
}

void CWorkbench::onBrainStorm()
{
    m_focusMode = true;
    onFullScreen();
	m_canvas->setFocus( Qt::MouseFocusReason );
    m_canvas->onMoveViewCenter( QPointF(0, 0) );
}

void CWorkbench::updateTitles(const QString &fileName, bool saved)
{
	QString		unsavedTpl = tr("[Unsaved] %1*");
    QString		windowsTpl = tr("JetMind - %1");

	if (fileName.trimmed().isEmpty())
	{
        emit docTitleChanged(tr(""));
        setWindowTitle( tr("JetMind") );
		return;
	}

	if (saved)
	{
        emit docTitleChanged( fileName );
		setWindowTitle(windowsTpl.arg( fileName ) );
	}
	else
	{
        emit docTitleChanged( unsavedTpl.arg(fileName) );
		setWindowTitle(windowsTpl.arg( unsavedTpl.arg(fileName) ) );
	}
}

QString CWorkbench::getFileNameViaDialog(const QString &title, const QString &initPath, const QString &filter, bool saveDialog)
{
	QString		path;

	if (saveDialog)
		path = QFileDialog::getSaveFileName(this, title, initPath, filter);
	else
		path = QFileDialog::getOpenFileName(this, title, initPath, filter);

	return path;
}

void CWorkbench::loadStyleSheet()
{
	QFile		file;

    m_canvas->setRulersVisible(false);
    m_toolbar->toggleFashionMode(true);
    file.setFileName(":/res/fashion.qss");
	file.open(QFile::ReadOnly);
    qApp->setStyleSheet(QLatin1String(file.readAll()));
}

QAbstractItemModel* CWorkbench::dataModel(bool currentFile)
{
	CMindMapGraph		*graph = NULL;

	graph = dynamic_cast<CMindMapGraph*>( m_diagram );
	if (!graph)
		return NULL;

	return graph->dataModel();
}

void CWorkbench::onJobDone()
{
	m_busyIndicator->stop();

    if (m_file)
    {
        m_fileManager->updateFileInfo(m_file);
        updateRecentFileList( QDir(m_file->path()).absoluteFilePath(m_file->fileName()) );
        updateTitles(m_file->fileName(), !m_file->isUnsaved());
    }
}

void CWorkbench::onOpenUrl(QUrl url)
{
	int				graphId = -1, nodeId = -1;
	QString			sep(":");
	QString			urlString;
	QStringList		tokens;
	CMindMapFile	*file = NULL;
	CMindMapGraph	*graph = NULL;

	urlString = url.toString();
	tokens = urlString.split(sep, QString::SkipEmptyParts);
	
	if (tokens.length() != 3)
		return;

	graphId = tokens[1].toInt();
	nodeId = tokens[2].toInt();

	file = dynamic_cast<CMindMapFile*>(m_file);
	if (file)
	{
		graph = file->getGraphById(graphId);
		if (graph && graph != m_diagram)
		{
			onViewDiagram(graph);
		}
		graph->locateNode(nodeId);
	}
}

void CWorkbench::updateRecentFileList(const QString &fullPath)
{	
	QSettings			settings;
	QFileInfo			info(fullPath);
	QStringList			recentFiles;

	recentFiles = settings.value(Settings::RecentFiles).toStringList();
	if (!recentFiles.contains(info.absoluteFilePath(), Qt::CaseInsensitive))
		recentFiles.prepend(info.absoluteFilePath());

	while (recentFiles.size() > m_recentFilesLimit)
		recentFiles.removeLast();

	settings.setValue(Settings::LastLocation, QVariant(info.path()));
	settings.setValue(Settings::RecentFiles, QVariant(recentFiles) );
}
