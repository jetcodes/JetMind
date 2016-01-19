/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramLayer>
#include <core/CDiagramWidget>
#include <core/CDiagramContext>
#include <core/CDiagramCommands>

#include "CPresentationToolbar.h"
#include "ui_CPresentationToolbar.h"

CPresentationToolbar::CPresentationToolbar(QWidget *parent)
	: CDiagramHostedWidget(parent)
{
	ui = new Ui::CPresentationToolbar();
	ui->setupUi(this);

	connect(ui->m_btnClear, SIGNAL(clicked()), this, SLOT(onClear()));
	connect(ui->m_btnNext, SIGNAL(clicked()), this, SLOT(onNext()));
	connect(ui->m_btnPrev, SIGNAL(clicked()), this, SLOT(onPrev()));
	connect(ui->m_btnExit, SIGNAL(clicked()), this, SLOT(onExit()));

    setCursor( QCursor(QPixmap(":/res/icons/presentation-cursor.png"), 0, 0) );

    resize(195, 52);
}

CPresentationToolbar::~CPresentationToolbar()
{
	delete ui;
}


bool CPresentationToolbar::support(CDiagram *d)
{
	return true;
}

void CPresentationToolbar::onDraw()
{
	setViewMode(QGraphicsView::NoDrag);
	setLayerEditMode(CDiagramTagLayer::Scribble);
}

void CPresentationToolbar::onComment()
{
	setViewMode(QGraphicsView::NoDrag);
	setLayerEditMode(CDiagramTagLayer::Tagging);
}

void CPresentationToolbar::onClear()
{
	CDiagramTagLayer	*layer = NULL;

	if (!m_d) return;

	layer = dynamic_cast<CDiagramTagLayer*>(m_d->getDefaultLayer(CDiagram::TagLayer));
	if (layer)
		layer->clear();

	setViewMode(QGraphicsView::NoDrag);
	setLayerEditMode(CDiagramTagLayer::Cursor);
}

void CPresentationToolbar::onPointer()
{
	setViewMode(QGraphicsView::NoDrag);
	setLayerEditMode(CDiagramTagLayer::Cursor);
}

void CPresentationToolbar::onHand()
{
	setViewMode(QGraphicsView::ScrollHandDrag);
	setLayerEditMode(CDiagramTagLayer::Cursor);
}

void CPresentationToolbar::setLayerEditMode(CDiagramTagLayer::EditMode mode)
{
	CDiagramTagLayer	*layer = NULL;

	if (!m_d) return;

	layer = dynamic_cast<CDiagramTagLayer*>(m_d->getDefaultLayer(CDiagram::TagLayer));
	if (layer)
		layer->setEditMode(mode);
}

void CPresentationToolbar::setViewMode(QGraphicsView::DragMode mode)
{
	CDiagramTagLayer	*layer = NULL;
	QGraphicsView		*v = NULL;

	if (!m_d) return;

	v = m_d->getMainView();
	layer = dynamic_cast<CDiagramTagLayer*>( m_d->getDefaultLayer(CDiagram::TagLayer) );
	if (v && layer)
	{
		v->setDragMode(mode);
		if (mode == QGraphicsView::ScrollHandDrag)
		{
			layer->setCursor(Qt::OpenHandCursor);
		}
		else
		{
			layer->setCursor(Qt::ArrowCursor);
		}
	}
}

void CPresentationToolbar::onExit()
{
	QWidget	*widget = NULL;

	widget = CDiagramContext::env()->getMainUI();
	if (widget && QMetaObject::invokeMethod(widget, "onFullScreen", Qt::DirectConnection))
	{
		qDebug() << "Succeed calling the slot:onFullScreen";
	}
}

void CPresentationToolbar::onNext()
{
	QWidget	*widget = NULL;

	widget = CDiagramContext::env()->getMainUI();
	if (widget && QMetaObject::invokeMethod(widget, "onViewNextDiagram", Qt::DirectConnection, Q_ARG(int, 1) ))
	{
		qDebug() << "Succeed calling the slot:onViewNextDiagram";
	}
}

void CPresentationToolbar::onPrev()
{
	QWidget	*widget = NULL;

	widget = CDiagramContext::env()->getMainUI();
	if (widget && QMetaObject::invokeMethod(widget, "onViewNextDiagram", Qt::DirectConnection, Q_ARG(int, -1) ))
	{
		qDebug() << "Succeed calling the slot:onViewNextDiagram";
	}
}
