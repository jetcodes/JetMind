/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include "CUiColorGrid.h"
#include "CUiPenStylePicker.h"
#include "CUiPenWeightPicker.h"
#include "CUiEdgeStylePanel.h"
#include "ui_CUiEdgeStylePanel.h"

CUiEdgeStylePanel::CUiEdgeStylePanel(QWidget *parent)
	: QWidget(parent, Qt::Popup),
	  m_colorGrid(NULL),
	  m_penStyles(NULL),
	  m_penWidths(NULL)
{
	QVBoxLayout		*layout = NULL;

	ui = new Ui::CUiEdgeStylePanel();
	ui->setupUi(this);

	setMouseTracking(true);

	m_colorGrid = new CUiColorGrid(this);
	m_penStyles = new CUiPenStylePicker(this);
	m_penWidths = new CUiPenWeightPicker(this);

	layout = new QVBoxLayout(NULL);
	layout->setContentsMargins(2,2,2,2);
	layout->setSpacing(0);

	layout->addWidget(m_colorGrid);
	ui->m_colorGridHolder->setLayout(layout);

	ui->m_btnLineStyle->setMenu(m_penStyles);
	ui->m_btnLineWeight->setMenu(m_penWidths);

	connect(m_colorGrid, SIGNAL(colorSelectionDone()), this, SLOT(onColorSelectionDone()));
	connect(m_colorGrid, SIGNAL(colorSelected(QColor)), this, SLOT(onColorSelected(QColor)));
	connect(m_penStyles, SIGNAL(styleSelected(Qt::PenStyle)), this, SLOT(onPenStyleSelected(Qt::PenStyle)));
	connect(m_penWidths, SIGNAL(weightSelected(qreal)), this, SLOT(onPenWeightSelected(qreal)));

    layout->invalidate();
    resize(layout->sizeHint());
}

CUiEdgeStylePanel::~CUiEdgeStylePanel()
{
	delete ui;
}

void CUiEdgeStylePanel::paintEvent( QPaintEvent *event )
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiEdgeStylePanel::onColorSelected(QColor c)
{
	emit colorSelected(c);
}

void CUiEdgeStylePanel::onPenStyleSelected(Qt::PenStyle s)
{
	emit styleSelected(s);
}

void CUiEdgeStylePanel::onPenWeightSelected(qreal w)
{
	emit weightSelected(w);
}

void CUiEdgeStylePanel::onColorSelectionDone()
{
	close();
}
