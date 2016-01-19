/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramWidget>
#include <core/CDiagramContext>
#include <mindmap/CMindMapSettings>

#include "ui_CTitleBar.h"
#include "CTitleBar.h"
#include "CWorkbench.h"
#include "CActionManager.h"
#include "CAppSettings.h"

CTitleBar::CTitleBar(QWidget *parent)
	: QWidget(parent),
	  m_ui(NULL),
	  m_maxNormal(false),
	  m_recentFilesGroup(NULL)
{
	CWorkbench	*bench = NULL;

	bench = dynamic_cast<CWorkbench*>(parent);

	m_ui = new Ui::CTitleBar();
	m_ui->setupUi(this);
	
	setupMenus();

	connect(m_ui->m_btnClose, SIGNAL(clicked()), this, SIGNAL(closeWindow()));
	connect(m_ui->m_btnMinimize, SIGNAL(clicked()), this, SIGNAL(minimize()));
	connect(m_ui->m_btnMaximize, SIGNAL(clicked()), this, SLOT(onMaximize()));

	connect(this, SIGNAL(minimize()), bench, SLOT(onMinimize()));
	connect(this, SIGNAL(maximize()), bench, SLOT(onMaximize()));
	connect(this, SIGNAL(restore()), bench, SLOT(onRestore()));
	connect(this, SIGNAL(move(QPoint)), bench, SLOT(onMove(const QPoint&)));
	connect(this, SIGNAL(closeWindow()), bench, SLOT(onClose()));
	connect(this, SIGNAL(openRecentFile(const QString &)), bench, SLOT(onLoadFile(const QString &)));
	
	if (m_recentFilesGroup)
		connect(m_recentFilesGroup, SIGNAL(triggered(QAction*)), this, SLOT(onRecentFileTriggered(QAction*)));
}

CTitleBar::~CTitleBar()
{

}

void CTitleBar::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CTitleBar::setDocTitle(const QString &docTitle)
{
	m_ui->m_docTitle->setText(docTitle);
	positionTitle();
}

void CTitleBar::positionTitle()
{
	int				textPadding = 8;
	int				textWidth = 0, maxWidth = 0, preferredWidth;
	QRect			r0, r1, r2, r3;
	QFont			ft = m_ui->m_docTitle->font();
	QFontMetrics	ftMetrics(ft);

	r0 = geometry();
	r1 = m_ui->m_menuHolder->geometry();
	r2 = m_ui->m_titleHolder->geometry();
	r3 = m_ui->m_buttonHolder->geometry();

	textWidth = ftMetrics.width( m_ui->m_docTitle->text() );

	maxWidth = qMax(r1.width(), r3.width());
	preferredWidth = r0.width() - 2 * (maxWidth + textPadding);
	if (preferredWidth > textWidth)
	{
		m_ui->m_docTitle->setIndent( r0.center().x() - textWidth / 2 - r2.left() );
	}
	else
	{
		m_ui->m_docTitle->setIndent( textPadding );
	}
}

QString CTitleBar::docTitle()
{
	return m_ui->m_docTitle->text();
}

void CTitleBar::setupMenus()
{
	QMenu				*t = NULL;
	QAction				*a = NULL;
	CWorkbench			*bench = NULL;
	CActionManager		*am = NULL;
	QSettings			settings;
	QStringList			recentFiles;
	QList<QFileInfo>	fileInfos;

	bench = dynamic_cast<CWorkbench*>(parent());
	am = bench->actionManager();

	m_file = new QMenu(this);
	m_file->addAction(am->m_actNewFile);
	m_file->addAction(am->m_actNewSheet);
	m_file->addSeparator();
	m_file->addAction(am->m_actOpenFile);
	//m_file->addAction(am->m_actSaveAsTemplate);
	t = new QMenu(tr("Recent Files"), m_file);
	m_recentFilesGroup = new QActionGroup(this);
	recentFiles = settings.value(Settings::RecentFiles).toStringList();
	recentFiles.removeDuplicates();
	for (int i = 0; i < recentFiles.length(); ++i)
	{
		QFileInfo	info(recentFiles.at(i));
		if (info.exists())
		{
			if (!fileInfos.contains(info))
			{
				a = m_recentFilesGroup->addAction( info.fileName() );
				a->setData(QVariant(info.absoluteFilePath()));
				a->setToolTip(info.filePath());
			}
		}
	}

	t->addActions(m_recentFilesGroup->actions());
	m_file->addMenu(t);
	m_file->addSeparator();

	m_file->addAction(am->m_actSaveFile);
	m_file->addAction(am->m_actSaveAs);

	m_file->addSeparator();

	m_file->addAction(am->m_actExport);
	m_file->addSeparator();
	m_file->addAction(am->m_actPrint);
	m_file->addAction(am->m_actPrintPreview);
	m_file->addSeparator();
	m_file->addAction(am->m_actExit);
	m_ui->m_fileMenu->setMenu(m_file);

	m_edit = new QMenu(this);
	m_edit->addAction(am->m_actRedo);
	m_edit->addAction(am->m_actUndo);
	m_edit->addSeparator();
	m_edit->addAction(am->m_actCut);
	m_edit->addAction(am->m_actCopy);
	m_edit->addAction(am->m_actPaste);
	m_edit->addAction(am->m_actSelectAll);
	m_ui->m_editMenu->setMenu(m_edit);

	m_view = new QMenu(this);
	m_view->addAction(am->m_actShowToolBar);
	m_view->addSeparator();
    m_view->addAction(am->m_actShowCanvasSetupPanel);
    m_view->addAction(am->m_actShowCanvasQuickView);
    m_view->addAction(am->m_actShowGrids);
	m_view->addSeparator();

	t = new QMenu(tr("Property Editor Anchors"), m_view);
    t->addAction( am->m_actToolPanelAnchorSelectedBymbol );
    t->addAction( am->m_actToolPanelAnchorLeft );
    t->addAction( am->m_actToolPanelAnchorRight );
    t->addAction( am->m_actToolPanelFreeFloat );
    m_view->addMenu( t );
    m_view->addAction( am->m_actToolPanelAutoHide );
    m_view->addAction( am->m_actToolPanelCollapsible );
    m_view->addSeparator();

	m_view->addAction(am->m_actPresentation);
	m_view->addAction(am->m_actBrainStorm);
	
	am->m_actShowGrids->setChecked( settings.value(Settings::MindMapBackgroundShowGrid, QVariant(true)).value<bool>() );
    am->m_actShowToolBar->setChecked( settings.value(Settings::ShowToolBar, QVariant(true)).value<bool>());

	m_ui->m_viewMenu->setMenu(m_view);

	m_help = new QMenu(this);
	m_help->addAction(am->m_actManual);
	
	// 
	m_help->addSeparator();
	m_help->addAction(am->m_actSupport);
	m_help->addAction(am->m_actUpdate);
	m_help->addSeparator();
	m_help->addAction(am->m_actAboutRemind);
	m_ui->m_helpMenu->setMenu(m_help);
}

void CTitleBar::onMaximize()
{
	if (m_maxNormal) 
	{
		m_ui->m_btnMaximize->setToolTip(tr("Maximize the window"));
		m_ui->m_btnMaximize->setIcon(QIcon(QString::fromUtf8(":/res/icons/app-window-maximize.png")));
		emit restore();
	} 
	else 
	{
		m_ui->m_btnMaximize->setToolTip(tr("Restore the window"));
		m_ui->m_btnMaximize->setIcon(QIcon(QString::fromUtf8(":/res/icons/app-window-restore.png")));
		emit maximize();
	}
	m_maxNormal = !m_maxNormal;
}

void CTitleBar::onDocTitleChanged(const QString &title)
{
    setDocTitle(title);
}

void CTitleBar::mousePressEvent(QMouseEvent *ev)
{
	m_clickPos = mapToParent(ev->pos());
}

void CTitleBar::mouseDoubleClickEvent(QMouseEvent *ev)
{
	onMaximize();
}

void CTitleBar::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_maxNormal) 
		return;
	emit move(ev->globalPos() - m_clickPos);
}

void CTitleBar::resizeEvent(QResizeEvent *ev)
{
	QWidget::resizeEvent(ev);

	positionTitle();
}

void CTitleBar::enterEvent(QEvent *ev)
{
	setCursor(Qt::ArrowCursor);
}

void CTitleBar::onRecentFileTriggered(QAction *act)
{
	QString		fullPath;
	
	if (!act)
		return;
	
	fullPath = act->data().toString();
	emit openRecentFile(fullPath);
}