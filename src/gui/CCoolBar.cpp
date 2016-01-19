/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>

#include <core/CDiagram>
#include <core/CDiagramTextNode>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdge>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramEdgeStyleCategory>

#include <mindmap/CMindMapSettings>

#include <widgets/CUiColorPanel>
#include <widgets/CUiPenStylePicker>
#include <widgets/CUiPenWeightPicker>
#include <widgets/CUiPathStyleList>
#include <widgets/CUiPathStylePicker>
#include <widgets/CUiNodeStylePicker>
#include <widgets/CUiOpenedFilePicker>

#include "CAppSettings.h"
#include "ui_CCoolBar.h"
#include "CCoolBar.h"
#include "CWorkbench.h"
#include "CActionManager.h"

CCoolBar::CCoolBar(QWidget *parent)
	: QWidget(parent),
	  m_currMeta(NULL),
	  m_openedFileList(NULL),
	  m_ui(NULL),
	  m_bench(NULL),
      m_completer(NULL),
      m_filterTypes(NULL)
{
    int                 filterType = 0;
    bool                filterCaseSensitive = false;
	CDiagramEdgeStyle	*r = NULL;
	CActionManager		*am = NULL;
	QAbstractItemModel	*model = NULL;
	CDiagramFileManager	*fileManager = NULL;
    QAction             *act = NULL;
    QActionGroup        *actGroup = NULL;
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    filterType = setting->get(Settings::MindMapFilterType, QVariant(Qt::MatchContains)).toInt();
    filterCaseSensitive = setting->get(Settings::MindMapFilterCaseSensitive, QVariant(false)).toBool();

	m_bench = dynamic_cast<CWorkbench*>(parent);
	am = m_bench->actionManager();
	fileManager = m_bench->fileManager();

	m_ui = new Ui::CCoolBar();
	m_ui->setupUi(this);
	setMinimumHeight(34);

	m_openedFileList = new CUiOpenedFilePicker(this);
	m_openedFileList->setVisible(false);

	model = fileManager->infoModel();
	m_openedFileList->setModel(model);

	m_tools = new QButtonGroup(this);
	m_tools->setExclusive(true);
    m_tools->addButton(m_ui->m_btnCursor);
    // m_tools->addButton(m_ui->m_btnFreeTopic);
    m_tools->addButton(m_ui->m_btnConnection);
	m_tools->addButton(m_ui->m_btnText);

#if defined(__APPLE__) && defined(__MACH__)
    m_edgeStyles = new CUiPathStyleList(this);
#else
    m_edgeStyles = new CUiPathStylePicker(this);
#endif

	m_ui->m_btnConnection->setMenu( m_edgeStyles );
	r = m_edgeStyles->defaultStyle();
	if (r)
	{
		m_ui->m_btnConnection->setIcon( r->getIcon() );
		CDiagramEdgeStyleDatabase::instance()->setDefaultRouter(r);
	}

	m_completer = new QCompleter( this );
	m_completer->setCompletionMode( QCompleter::PopupCompletion );
	m_completer->setCaseSensitivity( Qt::CaseInsensitive );
    m_completer->setMaxVisibleItems(15);

    m_filterTypes = new QMenu(this);
    actGroup = new QActionGroup(this);
    actGroup->setExclusive(true);

    act = m_filterTypes->addAction(tr("Match Contains"));
    act->setData( QVariant(Qt::MatchContains) );
    act->setCheckable(true);
    act->setChecked( (act->data().toInt() == filterType) );
    actGroup->addAction(act);

    act = m_filterTypes->addAction(tr("Match Starts with"));
    act->setData( QVariant(Qt::MatchStartsWith ));
    act->setCheckable(true);
    act->setChecked( (act->data().toInt() == filterType) );
    actGroup->addAction(act);

    act = m_filterTypes->addAction(tr("Match Ends with"));
    act->setData( QVariant(Qt::MatchEndsWith ));
    act->setCheckable(true);
    act->setChecked( (act->data().toInt() == filterType) );
    actGroup->addAction(act);

    act = m_filterTypes->addAction(tr("Match Exactly"));
    act->setData( QVariant(Qt::MatchExactly ));
    act->setCheckable(true);
    act->setChecked( (act->data().toInt() == filterType) );
    actGroup->addAction(act);

    m_filterTypes->addSeparator();

    act = m_filterTypes->addAction(tr("Case Sensitive"));
    act->setData( QVariant(Qt::MatchCaseSensitive ) );
    act->setCheckable(true);
    act->setChecked( filterCaseSensitive );

    //m_ui->m_btnFilterTypes->setMenu(m_filterTypes);

	// install event filter to get the focus in event
	m_ui->m_filter->installEventFilter(this);

    m_ui->m_btnNewFile->setDefaultAction(am->m_actNewFile);
	m_ui->m_btnOpenFile->setDefaultAction(am->m_actOpenFile);
	m_ui->m_btnSaveFile->setDefaultAction(am->m_actSaveFile);
	m_ui->m_btnRedo->setDefaultAction(am->m_actRedo);
	m_ui->m_btnUndo->setDefaultAction(am->m_actUndo);
	m_ui->m_btnPrev->setDefaultAction(am->m_actPrev);
	m_ui->m_btnNext->setDefaultAction(am->m_actNext);
    m_ui->m_btnClose->setDefaultAction(am->m_actCloseFile);
	m_ui->m_btnExport->setDefaultAction( am->m_actExport );
    //m_ui->m_btnPrint->setDefaultAction( am->m_actPrint );
    //m_ui->m_btnPrintPreview->setDefaultAction( am->m_actPrintPreview );
    m_ui->m_btnFullScreen->setDefaultAction(am->m_actPresentation);
    m_ui->m_btnBrainStorm->setDefaultAction(am->m_actBrainStorm);

	m_ui->m_btnConnection->setVisible(false);

    m_ui->m_filter->clearFocus();
    m_ui->m_filter->setFocusPolicy(Qt::ClickFocus);
    m_ui->m_filter->setPlaceholderText( tr("Filter Topics...") );

    connect(m_ui->m_btnCursor, SIGNAL(clicked()), this, SIGNAL(cursorMode()));
    connect(m_ui->m_btnText, SIGNAL(clicked()), this, SLOT(onInsertText()));
    // connect(m_ui->m_btnFreeTopic, SIGNAL(clicked()), this, SLOT(onInsertFreeTopic()));
	connect(m_ui->m_btnConnection, SIGNAL(clicked()), this, SLOT(onInsertLink()));
    connect(m_ui->m_filter, SIGNAL(returnPressed()), this, SLOT(onFilterNodes()));

    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(onFilterNodes(QString)));
	connect(m_edgeStyles, SIGNAL(styleSelected(CDiagramEdgeStyle*)), this, SLOT(onRouterChanged(CDiagramEdgeStyle*)));
    connect(m_filterTypes, SIGNAL(triggered(QAction*)), this, SLOT(onFilterTypeChanged(QAction*)));

	// connect to the workbench's slots
    connect(this, SIGNAL(cursorMode()), m_bench, SLOT(onCursorMode()));
    connect(this, SIGNAL(insertLinkMode(bool)), m_bench, SLOT(onInsertLinkMode(bool)));
	connect(this, SIGNAL(insertTextMode(bool)), m_bench, SLOT(onInsertTextMode(bool)));
    // connect(this, SIGNAL(insertFreeTopic()), m_bench, SLOT(onInsertFreeTopic()));

    connect(m_openedFileList, SIGNAL(switchToFile(int)), this, SLOT(onSwitchFile(int)));
	connect(m_ui->m_btnOpenedFiles, SIGNAL(clicked()), this, SLOT(onOpenedFileList()));
	connect(am->m_actNext, SIGNAL(triggered()), this, SLOT(onViewNextFile()));
	connect(am->m_actPrev, SIGNAL(triggered()), this, SLOT(onViewPrevFile()));
    connect(am->m_actCloseFile, SIGNAL(triggered()), m_bench, SLOT(onCloseCurrentFile()));

	connect(m_bench, SIGNAL(diagramChanged(CDiagram*,CDiagram*)), this, SLOT(onDiagramChanged(CDiagram*, CDiagram*)));

    m_ui->m_btnPrev->hide();
    m_ui->m_btnNext->hide();
}

CCoolBar::~CCoolBar()
{

}

void CCoolBar::toggleFashionMode(bool value)
{
	QBoxLayout	*boxLayout = NULL;

	boxLayout = dynamic_cast<QBoxLayout*>(layout());
	if (value)
	{
		m_ui->m_btnNewFile->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnOpenFile->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnSaveFile->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnCursor->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_ui->m_btnRedo->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnUndo->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnExport->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_ui->m_btnFreeTopic->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnCursor->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_ui->m_btnText->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnConnection->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_ui->m_btnFullScreen->setToolButtonStyle(Qt::ToolButtonIconOnly);
		m_ui->m_btnBrainStorm->setToolButtonStyle(Qt::ToolButtonIconOnly);


		if (m_ui->m_docsFilter->isHidden())
			m_ui->m_docsFilter->show();
		m_ui->m_rightPartPlaceHolder->hide();
		if (boxLayout) 
			boxLayout->setStretch(0, 1);
	}
	else
	{
		m_ui->m_btnNewFile->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnOpenFile->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnSaveFile->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnExport->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnCursor->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_ui->m_btnRedo->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnUndo->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnCursor->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_ui->m_btnText->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		m_ui->m_btnConnection->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_ui->m_btnFullScreen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_ui->m_btnBrainStorm->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		m_ui->m_docsFilter->hide();
		if (m_ui->m_rightPartPlaceHolder->isHidden())
			m_ui->m_rightPartPlaceHolder->show();
		if (boxLayout) 
			boxLayout->setStretch(0, 0);
	}
}

bool CCoolBar::fashionMode()
{
	return false;
}

void CCoolBar::lockFocus(bool value)
{
	if (value)
    {
        m_ui->m_filter->setFocus( Qt::MouseFocusReason );
    }
}

void CCoolBar::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CCoolBar::onRouterChanged(CDiagramEdgeStyle *r)
{
	CDiagramEdgeStyleDatabase::instance()->setDefaultRouter(r);
	m_currMeta = const_cast<QMetaObject*>(&CDiagramEdge::staticMetaObject);
	m_ui->m_btnConnection->setChecked(true);
	m_ui->m_btnConnection->setIcon(r->getIcon() );
	emit insertLinkMode(QApplication::keyboardModifiers() & Qt::ControlModifier);
}

void CCoolBar::onInsertLink()
{
	m_currMeta = const_cast<QMetaObject*>(&CDiagramEdge::staticMetaObject);
	emit insertLinkMode(QApplication::keyboardModifiers() & Qt::ControlModifier);
}

void CCoolBar::onInsertText()
{
	m_currMeta = const_cast<QMetaObject*>(&CDiagramTextNode::staticMetaObject);
	emit insertTextMode(QApplication::keyboardModifiers() & Qt::ControlModifier);
}

void CCoolBar::onInsertFreeTopic()
{
    emit insertFreeTopic();
}

void CCoolBar::onNewItemAdded(CDiagramItem *item)
{
    CDiagram	*d = NULL;

	if (item)
		d = dynamic_cast<CDiagram*>(item->scene());

	if (d)
		m_ui->m_btnCursor->setChecked( d->mode() != CDiagram::AddMultipleItemMode);
}

void CCoolBar::onDiagramModeChanged(CDiagram::DiagramMode mode)
{
	if (mode == CDiagram::CursorMode)
		m_ui->m_btnCursor->setChecked(true);
}

void CCoolBar::onDiagramChanged(CDiagram *curr, CDiagram *prev)
{
	if (prev)
	{
		disconnect(prev, SIGNAL(itemsAdded(CDiagramItem*)), this, SLOT(onNewItemAdded(CDiagramItem*)));
		disconnect(prev, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	}

	if (curr)
	{
		connect(curr, SIGNAL(itemsAdded(CDiagramItem*)), this, SLOT(onNewItemAdded(CDiagramItem*)));
		connect(curr, SIGNAL(modeChanged(CDiagram::DiagramMode)), this, SLOT(onDiagramModeChanged(CDiagram::DiagramMode)));
	}
}

void CCoolBar::onOpenedFileList()
{
	m_openedFileList->move( m_ui->m_btnOpenedFiles->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnOpenedFiles->x() - m_openedFileList->width() + m_ui->m_btnOpenedFiles->width(), m_ui->m_btnOpenedFiles->y() + m_ui->m_btnOpenedFiles->height()) ) );
	m_openedFileList->setWindowFlags(Qt::Popup);
	m_openedFileList->show();
}

void CCoolBar::onFilterNodes()
{
    onFilterNodes( m_ui->m_filter->text() );
}

void CCoolBar::onFilterNodes(QString keywords)
{
    if (m_bench)
    {
        m_bench->onFilterNodes( keywords );
    }
}

void CCoolBar::onFilterTypeChanged(QAction *action)
{
    int                 type = 0;
    bool                caseSensitive = false;
    CMindMapSetting     *setting = NULL;

    setting = CMindMapSetting::instance();
    if (!action || !setting)
        return;

    // clear it
    m_ui->m_filter->clear();

    type = action->data().toInt();
    caseSensitive = action->isChecked();
    switch(type)
    {
    case Qt::MatchCaseSensitive:
        m_completer->setCaseSensitivity( (caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) );
        setting->set(Settings::MindMapFilterCaseSensitive, QVariant(caseSensitive) );
        break;

    default:
        setting->set(Settings::MindMapFilterType, QVariant(type));
        break;
    }; // End of switch
}

void CCoolBar::onSwitchFile(int key)
{
	CDiagram			*d = NULL;
	CDiagramFile		*file = NULL;

	if (m_bench)
	{
        file = m_bench->fileManager()->getFileById(key);
        m_bench->onViewFile(file);
	}
}

void CCoolBar::onViewNextFile()
{
	if (m_bench)
        m_bench->onViewNextFile(1);
}

void CCoolBar::onViewPrevFile()
{
	if (m_bench)
        m_bench->onViewNextFile(-1);
}

void CCoolBar::relocate()
{
	// TODO
}

bool CCoolBar::eventFilter(QObject *obj, QEvent *event)
{
	if (m_bench && obj == m_ui->m_filter)
	{
		switch (event->type())
		{
		case QEvent::FocusIn:
		{
			if (m_completer)
            {
				m_completer->setModel( m_bench->dataModel() );
                m_completer->setMaxVisibleItems(15);
                m_ui->m_filter->setCompleter(m_completer);
            }
			break;
		}

		case QEvent::FocusOut:
			m_ui->m_filter->clear();
			break;

		default:
			break;
		}; // End of switch
	}

	return QWidget::eventFilter(obj, event);
}
