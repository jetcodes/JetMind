/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramContext>
#include <core/CDiagramItem>
#include <core/CDiagramNode>

#include <widgets/CUiOpenedFilePicker>
#include <widgets/CUiRuler>

#include "CCanvas.h"
#include "ui_CCanvas.h"
#include "CWorkbench.h"
#include "CCanvasBand.h"
#include "CActionManager.h"

CCanvas::CCanvas(QWidget *parent)
	: QWidget(NULL),
	  m_ui(NULL),
	  m_editor(NULL),
	  m_d(NULL),
	  m_workbench(NULL),
      m_showGrids(false),
      m_showRulers(false),
	  m_autoAlignment(true)
{
	QAbstractItemModel	*model = NULL;
	CWorkbench			*bench = NULL;
	CDiagramContext		*context = NULL;

	m_ui = new Ui::CCanvas();
	m_ui->setupUi(this);

	context = CDiagramContext::env();
	bench = dynamic_cast<CWorkbench*>(parent);
	m_workbench = bench;

	m_band = new CCanvasBand(this);
    m_holder = new QWidget();
	m_hRuler = new CUiRuler(this);
	m_vRuler = new CUiRuler(this, CUiRuler::VerticalMode);
	m_hRuler->setMinimumHeight(16);
	m_vRuler->setMinimumWidth(16);

	m_editor = context->getMainEditor();

	QVBoxLayout *insideLayout = new QVBoxLayout();
	insideLayout->setMargin(0);
	insideLayout->setSpacing(0);
	insideLayout->addWidget(m_editor);
	m_holder->setLayout(insideLayout);

	QHBoxLayout	*centerLayout = new QHBoxLayout();
	centerLayout->setMargin(0);
	centerLayout->setSpacing(0);
	centerLayout->addWidget(m_vRuler, 0);
	centerLayout->addWidget(m_holder, 1);

	QVBoxLayout	*layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_hRuler, 0);
	layout->addLayout(centerLayout, 1);
	layout->addWidget(m_band, 0);
	setLayout(layout);

	connect(m_editor->horizontalScrollBar(), SIGNAL(valueChanged(int)), m_hRuler, SLOT(onScrollChanged(int)));
	connect(m_editor->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), m_hRuler, SLOT(onRangeChanged(int, int)));
	connect(m_editor->verticalScrollBar(), SIGNAL(valueChanged(int)), m_vRuler, SLOT(onScrollChanged(int)));
	connect(m_editor->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), m_vRuler, SLOT(onRangeChanged(int, int)));

	connect(m_band, SIGNAL(zoom(int)), this, SLOT(onZoom(int)));
    connect(m_band, SIGNAL(gridsVisibleChanged(bool)), this, SIGNAL(gridsVisibleChanged(bool)));
    connect(m_band, SIGNAL(ensureDiagramVisible()), this, SLOT(onEnsureDiagramVisible()));
    connect(m_band, SIGNAL(canvasStyleSelected(QBrush)), this, SLOT(onCanvasStyleSelected(QBrush)));

	connect(this, SIGNAL(gridsVisibleChanged(bool)), this, SLOT(onGridsVisibleChanged(bool)));
    connect(this, SIGNAL(rulersVisibleChanged(bool)), this, SLOT(onRulersVisibleChanged(bool)));
    connect(this, SIGNAL(autoAlignmentChanged(bool)), this, SLOT(onAutoAlignmentChanged(bool)));

	// without diagrams to edit, just hide the canvas band.
	m_band->hide();
}

CCanvas::~CCanvas()
{

}

void CCanvas::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CCanvas::setGridsVisible(bool value)
{
	onGridsVisibleChanged(value);
}

bool CCanvas::gridsVisible() const
{
	return m_showGrids;
}

void CCanvas::setRulersVisible(bool value)
{
    onRulersVisibleChanged(value);
}

bool CCanvas::rulersVisible() const
{
	return m_showRulers;
}

void CCanvas::setAutoAlignment(bool value)
{
	m_autoAlignment = value;
	if (m_d)
		m_d->setAutoAlignment(value);
}

bool CCanvas::autoAlignment() const
{
	return m_autoAlignment;
}

void CCanvas::closeDiagram()
{
	m_editor->setDiagram(NULL);
	if (m_d)
	{
		disconnect(m_d, SIGNAL(scaleChanged(qreal)), this, SLOT(onScaleChanged(qreal)));
		disconnect(m_d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), m_band, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	}
	m_d = NULL;

	// just hide the band when there is no diagram to edit
	m_band->hide();
}

void CCanvas::viewDiagram(CDiagram *d, const QPointF &center)
{
	if (m_d == d)
		return;

	if (m_d)
	{
		disconnect(m_d, SIGNAL(scaleChanged(qreal)), this, SLOT(onScaleChanged(qreal)));
		disconnect(m_d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), m_band, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	}

	if (d)
	{
		m_editor->setDiagram( d, center );
		d->setGridVisible(m_showGrids);
		d->setAutoAlignment(m_autoAlignment);
        if (!m_d && m_band->isHidden())
            m_band->show();
		m_band->update(d);

		connect(d, SIGNAL(scaleChanged(qreal)), this, SLOT(onScaleChanged(qreal)));
        connect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*>&, const QRectF &)), m_band, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF &)));
	}
	else
	{
		m_band->hide();
	}
	m_d = d;
}

void CCanvas::onZoom(int val)
{
	if (m_editor)
	{
		m_editor->scaleView(val / 100.0);
	}
}

void CCanvas::onScaleChanged(qreal factor)
{
	m_band->setZoomValue(qRound(factor * 100));
}

void CCanvas::setFullScreenMode(bool value)
{
	if (value)
	{
        m_band->hide();
	}
	else
	{
        m_band->show();
	}
    onEnsureDiagramVisible();
}

void CCanvas::onGridsVisibleChanged(bool value)
{
	m_showGrids = value;
	if (m_d)
		m_d->setGridVisible(value);
}

void CCanvas::onRulersVisibleChanged(bool value)
{
	m_showRulers = value;
	m_hRuler->setVisible(value);
	m_vRuler->setVisible(value);
}

void CCanvas::onAutoAlignmentChanged(bool value)
{
	setAutoAlignment(value);
}

void CCanvas::onEnsureDiagramVisible()
{
    QRectF		r;

	if (m_d && m_editor)
	{
        r = m_d->getItemsBoundingRect();
        m_editor->centerOn( r.center() );
        update();
	}
}

void CCanvas::onMoveViewCenter(const QPointF &point)
{
	if (m_d && m_editor)
	{
		m_editor->centerOn(point);
        update();
	}
}

void CCanvas::onShowCanvasSetupPanel()
{
	if (m_band)
        m_band->onShowCanvasStylePanel();
}

void CCanvas::onShowCanvasQuickView()
{
    if (m_band)
        m_band->onShowCanvasQuickView();
}

void CCanvas::onCanvasStyleSelected(QBrush brush)
{
    if (m_d)
    {
        m_d->setBackground(brush);
    }
}
