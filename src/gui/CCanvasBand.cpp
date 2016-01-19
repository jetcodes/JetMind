/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramFile>
#include <core/CDiagramLayer>
#include <core/CDiagramEdge>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramArrow>
#include <core/CDiagramIconProvider>

#include <widgets/CUiPercentPicker>
#include <widgets/CUiMessageBox>
#include <mindmap/CMindMapFile>

#include "CCanvas.h"
#include "CCanvasBand.h"
#include "CWorkbench.h"
#include "CActionManager.h"
#include "CCanvasQuickView.h"
#include "CCanvasSettingPanel.h"
#include "ui_CCanvasBand.h"

CCanvasBand::CCanvasBand(QWidget *parent)
	: QWidget(parent),
	  m_ui(NULL),
	  m_percentageList(NULL),
      m_stylePanel(NULL),
      m_quickView(NULL),
      m_item(NULL),
      m_sheetBar(NULL),
      m_file(NULL),
      m_tabMenu(NULL),
      m_tabEditor(NULL)
{
	CWorkbench			*b = NULL;
	CActionManager		*am = NULL;
    QList<QBrush>       styles;
    QHBoxLayout         *layout = NULL;
    QString             prefix(":/res/texture/bg%1.png");
    QString             filePath;
    QFileInfo           fileInfo;

	m_ui = new Ui::CCanvasBand();
	m_ui->setupUi(this);

	m_canvas = dynamic_cast<CCanvas*>(parent);
	b = dynamic_cast<CWorkbench*>(m_canvas->m_workbench);
	am = b->actionManager();

	m_ui->m_sliderZoom->setRange(25, 400);
	m_ui->m_sliderZoom->setValue(100);
	m_ui->m_sliderZoom->setTickInterval(10);

	m_percentageList = new CUiPercentPicker(this);
	m_percentageList->setVisible(false);

    m_quickView = new CCanvasQuickView(this);
    m_quickView->setVisible(false);
	
    m_sheetBar = new QTabBar(this);
    m_sheetBar->setUsesScrollButtons(true);
    m_sheetBar->setElideMode(Qt::ElideRight);
    m_sheetBar->setMaximumWidth(360);
    m_sheetBar->setDocumentMode(true);
    m_sheetBar->setShape( QTabBar::TriangularSouth );
    m_sheetBar->setContextMenuPolicy(Qt::CustomContextMenu);
    m_sheetBar->installEventFilter(this);
    m_sheetBar->setMovable(true);

    layout = dynamic_cast<QHBoxLayout*>(m_ui->leftPart->layout());
    if (layout)
        layout->insertWidget(0, m_sheetBar);
    else
        m_ui->leftPart->layout()->addWidget(m_sheetBar);

    styles << QBrush(Qt::white)
           << QBrush(Qt::lightGray);

    for (int i = 1; ; ++i)
    {
        filePath = prefix.arg( QString::number(i).rightJustified(3, QChar('0')) );
        fileInfo.setFile(filePath);
        if ( fileInfo.exists() && fileInfo.isFile() )
        {
            QPixmap pixmap;

            pixmap.load(filePath);
            styles << QBrush(pixmap);
        }
        else
            break;
    }

    m_stylePanel = new CCanvasSettingPanel(this);
    m_stylePanel->setVisible(false);
    m_stylePanel->setStyles(styles);

    m_tabMenu = new QMenu(NULL);
    m_tabMenu->addAction(tr("New Sheet"), this, SLOT(onNewGraphClicked()));
    m_tabMenu->addSeparator();
    m_tabMenu->addAction(tr("Remove Current Sheet"), this, SLOT(onRemoveGraphClicked()));
//    m_tabMenu->addAction(tr("Hide This Sheet"), this, SLOT(onHideGraphClicked()));
//    m_tabMenu->addSeparator();
//    m_tabMenu->addAction(tr("Hidden Sheet List"), this, SLOT(onListHiddenGraphes()));
//    m_tabMenu->addAction(tr("Show All hidden Sheets"), this, SLOT(onShowAllHiddenGraphes()));

    m_tabEditor = new QLineEdit(m_sheetBar);
    m_tabEditor->setWindowFlags( m_tabEditor->windowFlags() | Qt::FramelessWindowHint );
    m_tabEditor->hide();

    connect(m_tabEditor, SIGNAL(returnPressed()), this, SLOT(onTitleEditDone()));
    connect(m_tabEditor, SIGNAL(editingFinished()), this, SLOT(onTitleEditCancel()));

    connect(m_ui->m_btnQuickView, SIGNAL(clicked()),this, SLOT(onShowCanvasQuickView()));
    connect(m_ui->m_btnStylePanel, SIGNAL(clicked()), this, SLOT(onShowCanvasStylePanel()));
    connect(m_ui->m_sliderZoom, SIGNAL(valueChanged(int)), this, SLOT(onZoom(int)));
	connect(m_ui->m_btnPercentage, SIGNAL(clicked()), this, SLOT(onPercentage()));

    connect(m_stylePanel, SIGNAL(gridsVisibleChanged(bool)), this, SIGNAL(gridsVisibleChanged(bool)));
    connect(m_stylePanel, SIGNAL(styleSelected(QBrush)), this, SIGNAL(canvasStyleSelected(QBrush)));
    connect(m_quickView, SIGNAL(ensureDiagramVisible()), this, SIGNAL(ensureDiagramVisible()));

	connect(m_percentageList, SIGNAL(percentageChanged(int)), this, SLOT(onPercentageChanged(int)));
	connect(m_ui->m_btnZoomOut, SIGNAL(clicked()), this, SLOT(onZoomOut()));
	connect(m_ui->m_btnZoomIn, SIGNAL(clicked()), this, SLOT(onZoomIn()));
    connect(m_ui->m_btnNewSheet, SIGNAL(clicked()), this, SLOT(onNewGraphClicked()));
	connect(m_ui->m_btnRemoveSheet, SIGNAL(clicked()), this, SLOT(onRemoveGraphClicked()));

    connect(m_sheetBar, SIGNAL(tabMoved(int,int)), this, SLOT(onTabMoved(int,int)));
    connect(m_sheetBar, SIGNAL(currentChanged(int)), this, SLOT(onGraphTabSelected(int)));
    connect(m_sheetBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTabMenu(QPoint)));

    connect((QObject*)m_canvas->m_workbench, SIGNAL(geometryChanged(const QRect&)), this, SLOT(onBenchResized(const QRect&)));
	connect((QObject*)m_canvas->m_workbench, SIGNAL(fullscreen(bool)), this, SLOT(onBenchFullscreen(bool)));
    connect((QObject*)m_canvas->m_workbench, SIGNAL(fileChanged(CDiagramFile*)), this, SLOT(onChangeFile(CDiagramFile*)));
    connect((QObject*)m_canvas->m_workbench, SIGNAL(diagramAdded(CDiagram*)), this, SLOT(onAddGraph(CDiagram*)));
	connect((QObject*)m_canvas->m_workbench, SIGNAL(diagramRemoved(CDiagram*)), this, SLOT(onRemoveGraph(CDiagram*)));
	connect((QObject*)m_canvas->m_workbench, SIGNAL(viewDiagram(CDiagram*)), this, SLOT(onViewGraph(CDiagram*)));
}

CCanvasBand::~CCanvasBand()
{
}

void CCanvasBand::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CCanvasBand::setZoomValue(int value)
{
	m_ui->m_btnPercentage->setText( QString::number(value) + QString::fromUtf8("%") );
	m_ui->m_sliderZoom->setValue(value);
}

void CCanvasBand::update(CDiagram *d)
{
    m_quickView->update(d);
	setZoomValue(100);
}

bool CCanvasBand::eventFilter(QObject *obj, QEvent *evt)
{
    int             index = -1;
    bool            r = false;
    QPoint          pos;
    QRect           rect;
    QMouseEvent     *mouseEvent = NULL;

    if (obj == m_sheetBar)
    {
        if (evt->type() == QEvent::MouseButtonDblClick)
        {
            mouseEvent = dynamic_cast<QMouseEvent*>(evt);
            if (mouseEvent && (mouseEvent->buttons() & Qt::LeftButton) )
            {
                pos = mouseEvent->pos();
                for (int i = 0; i < m_sheetBar->count(); ++i)
                {
                    if (m_sheetBar->tabRect(i).contains( pos ))
                    {
                        index = i;
                        break;
                    }
                }
            }
        }
    }

    if (index >= 0 && index < m_sheetBar->count())
    {
        rect = m_sheetBar->tabRect(index);
        // rect = QRect( m_sheetBar->mapToGlobal(rect.topLeft()), rect.size() );
        m_tabEditor->setGeometry( rect );
        m_tabEditor->setText( m_sheetBar->tabText(index) );
        m_tabEditor->raise();
        m_tabEditor->show();
        m_tabEditor->selectAll();
        m_tabEditor->setFocus( Qt::MouseFocusReason );
        r = true;
    }

    return r;
}

void CCanvasBand::onPercentage()
{
	m_percentageList->setWindowFlags(Qt::Popup);
	m_percentageList->show();
	m_percentageList->move( m_ui->m_btnPercentage->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnPercentage->x(), m_ui->m_btnPercentage->y() - m_percentageList->height() ) ) );
}

void CCanvasBand::onZoom(int value)
{
	int		v;

	v = (int)((value + 9.0) / 10.0) * 10;
	m_ui->m_btnPercentage->setText( QString::number(v) + QString::fromUtf8("%") );
	emit zoom(v);
}

void CCanvasBand::onPercentageChanged(int value)
{
	m_ui->m_btnPercentage->setText( QString::number(value) + QString::fromUtf8("%") );
	m_ui->m_sliderZoom->setValue(value);
	emit zoom(value);
}

void CCanvasBand::onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect)
{
    //
}

void CCanvasBand::onShowCanvasQuickView()
{
	QPoint		p;
	QRect		r;

    if (m_quickView->isVisible())
	{
        m_quickView->hide();
	}
	else
	{
        m_quickView->show();
        p = m_ui->m_btnQuickView->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnQuickView->x() + m_ui->m_btnQuickView->width() - m_quickView->width(), m_ui->m_btnQuickView->y() - m_quickView->height() ) );
		p += QPoint(-16, -20);
        m_quickView->move( p );
	}
}

void CCanvasBand::onShowCanvasStylePanel()
{
    QPoint		p;
    QRect		r;

    if (m_stylePanel->isVisible())
    {
        m_stylePanel->hide();
    }
    else
    {
        m_stylePanel->show();
        p = m_ui->m_btnStylePanel->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnStylePanel->x() + m_ui->m_btnStylePanel->width() - m_stylePanel->width(), m_ui->m_btnStylePanel->y() - m_stylePanel->height() ) );
        p += QPoint(-16, -20);
        m_stylePanel->move( p );
    }
}

void CCanvasBand::onAutoSceneExpand(bool value)
{
	CDiagram	*diagram = NULL;

	diagram = m_canvas->diagram();
	if (diagram)
	{
		diagram->setAutoExpand(value);
	}
}

void CCanvasBand::onBenchResized(const QRect &rect)
{
	QPoint		p;

    if (m_quickView->isVisible())
	{
        p = m_ui->m_btnQuickView->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnQuickView->x() + m_ui->m_btnQuickView->width() - m_quickView->width(), m_ui->m_btnQuickView->y() - m_quickView->height() ) );
		p += QPoint(-16, -20);
        m_quickView->move( p );
	}

    if (m_stylePanel->isVisible())
    {
        p = m_ui->m_btnStylePanel->parentWidget()->mapToGlobal( QPoint(m_ui->m_btnStylePanel->x() + m_ui->m_btnStylePanel->width() - m_stylePanel->width(), m_ui->m_btnStylePanel->y() - m_stylePanel->height() ) );
        p += QPoint(-16, -20);
        m_stylePanel->move( p );
    }

	if (m_canvas)
		m_canvas->onMoveViewCenter(QPointF(0, 0));
}

void CCanvasBand::onBenchFullscreen(bool value)
{
	if (value)
	{
        m_quickView->hide();
	}
}

void CCanvasBand::onAddGraph(CDiagram *graph)
{
    int             index = -1;
    QString         title;
    CMindMapGraph   *mindmapGraph = NULL;

    mindmapGraph = dynamic_cast<CMindMapGraph*>(graph);
    if (!mindmapGraph)
        return;

    title = mindmapGraph->title();
    index = m_sheetBar->addTab(title);
    if (index >= 0)
    {
        m_sheetBar->setTabToolTip(index, tr("Double click to edit text"));
        m_sheetBar->setTabData(index, QVariant(mindmapGraph->id()));
        m_sheetBar->setCurrentIndex(index);
    }
}

void CCanvasBand::onChangeFile(CDiagramFile *file)
{
    int                     index = -1, count = 0;
    CMindMapGraph           *graph = NULL;
    CMindMapFile            *graphFile = NULL;
    QList<CMindMapGraph*>   graphes;

    graphFile = dynamic_cast<CMindMapFile*>(file);
    if (!graphFile || !m_sheetBar)
        return;

    m_file = graphFile;
    graphes = graphFile->graphes();
    if (graphes.length() == 0)
        return;

    count = m_sheetBar->count();
    for (int i = 0; i < count; ++i)
        m_sheetBar->removeTab(0);

    foreach (graph, graphes)
    {
        if (graph->hidden())
            continue;

        index = m_sheetBar->addTab(graph->title());
        if (index >= 0)
        {
            m_sheetBar->setTabToolTip(index, tr("Double click to edit sheet title"));
            m_sheetBar->setTabData(index, QVariant(graph->id()));
        }
    }

    m_sheetBar->setCurrentIndex(0);
}

void CCanvasBand::onGraphTabSelected(int index)
{
    int             id;
    QVariant        data;
    CMindMapGraph   *graph = NULL;

    if (index < 0 || index > m_sheetBar->count() - 1)
        return;

    data = m_sheetBar->tabData(index);
    id = data.value<int>();
    if (m_file)
    {
        graph = m_file->getGraphById(id);
        if (graph)
        {
            m_canvas->m_workbench->onViewDiagram(graph);
		}
    }
}

void CCanvasBand::onNewGraphClicked()
{
    if (m_canvas && m_canvas->m_workbench)
        m_canvas->m_workbench->onNewDiagram();
}

void CCanvasBand::onRemoveGraphClicked()
{
	if (m_canvas && m_canvas->m_workbench)
		m_canvas->m_workbench->onRemoveDiagram();
}

void CCanvasBand::onRemoveGraph(CDiagram *diagram)
{
	int				id = 0, index = -1;
	CMindMapGraph	*graph = NULL;

	if (!m_sheetBar)
		return;

	graph = dynamic_cast<CMindMapGraph*>(diagram);
	if (graph)
	{
		id = graph->id();
		for (int i = 0; i < m_sheetBar->count(); ++i)
		{
			if (id == m_sheetBar->tabData(i).value<int>())
			{
				index = i;
				break;
			}
		}
	}

	if (index >= 0 && index < m_sheetBar->count())
	{
		m_sheetBar->removeTab(index);
	}
}

void CCanvasBand::onViewGraph(CDiagram *diagram)
{
	int				id = 0;
	int				index = -1;
	CMindMapGraph	*graph = NULL;

	graph = dynamic_cast<CMindMapGraph*>(diagram);
	if (!diagram || !m_sheetBar || !graph)
		return;
	
	id = graph->id();
	for (int i = 0; i < m_sheetBar->count(); ++i)
	{
		if (id == m_sheetBar->tabData(i).value<int>())
		{
			index = i;
			break;
		}
	}

    if (index >= 0 && index < m_sheetBar->count() && m_sheetBar->currentIndex() != index)
		m_sheetBar->setCurrentIndex(index);
}

void CCanvasBand::onZoomOut()
{
	onZoom(m_ui->m_sliderZoom->value() - 10);
}

void CCanvasBand::onZoomIn()
{
	onZoom(m_ui->m_sliderZoom->value() + 1);
}

void CCanvasBand::onTabMenu(QPoint pos)
{
    if (m_tabMenu)
    {
        m_tabMenu->exec(m_sheetBar->mapToGlobal(pos));
    }
}

void CCanvasBand::onTitleEditDone()
{
    int                id = -1, index = -1;
    CMindMapGraph      *graph = NULL;

    index = m_sheetBar->currentIndex();
    m_sheetBar->setTabText(index, m_tabEditor->text() );
    if (m_file)
    {
        id = m_sheetBar->tabData( index ).toInt();
        graph = m_file->getGraphById( id );
        if (graph)
        {
            graph->setTitle( m_tabEditor->text() );
        }
    }

    m_tabEditor->clearFocus();
    m_tabEditor->hide();
}

void CCanvasBand::onTitleEditCancel()
{
    m_tabEditor->clearFocus();
    m_tabEditor->hide();
}

void CCanvasBand::onTabMoved(int from, int to)
{
    int     maxIndex = 0;

    if (!m_file)
        return;

    maxIndex = m_file->graphCount() - 1;
    if (from < 0 || to < 0 || from > maxIndex || to > maxIndex)
        return;

    m_file->moveGraph(from, to);
}

void CCanvasBand::onHideGraphClicked()
{
    // todo
}

void CCanvasBand::onListHiddenGraphes()
{
    // todo
}

void CCanvasBand::onShowAllHiddenGraphes()
{
    // todo
}
