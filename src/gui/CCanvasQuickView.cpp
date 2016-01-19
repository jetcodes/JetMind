 /**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramEditor>

#include "CCanvasQuickView.h"
#include "ui_CCanvasQuickView.h"

CCanvasQuickView::CCanvasQuickView(QWidget *parent)
	: CUiAbstractDialog(parent),
	  m_d(NULL),
	  m_view(NULL)
{
	QBoxLayout		*layout = NULL;

    ui = new Ui::CCanvasQuickView();
	ui->setupUi(m_main);

	setModal(false);

	layout = new QVBoxLayout(NULL);
	layout->setContentsMargins(0, 0, 0, 0);
	ui->m_miniViewHolder->setLayout(layout);
	
	connect(ui->m_btnLocate, SIGNAL(clicked()), this, SIGNAL(ensureDiagramVisible()));

    setTitle(tr("Quick View"));
    resize(280, 120);
}

CCanvasQuickView::~CCanvasQuickView()
{
	delete ui;
}

void CCanvasQuickView::update(CDiagram *d)
{
	QRectF				r;
	CDiagramEditor		*editor = NULL;
	CDiagramView		*view = NULL;
	QLayout				*layout = NULL;

	layout = ui->m_miniViewHolder->layout();
	if (layout && m_view)
	{
		layout->removeWidget(m_view);
		m_view->setParent(NULL);
		m_view = NULL;
	}

	m_d = d;
	if (m_d)
	{
		editor = dynamic_cast<CDiagramEditor*>(m_d->getMainView());
		if (editor)
		{
			view = editor->diagramView();
			if (view)
			{
				view->setParent(this);
				layout->addWidget(view);
				m_view = view;
			}
		}
	}
}

void CCanvasQuickView::changeEvent (QEvent *event)
{
    if(event->type() == QEvent::ActivationChange)
    {
        if (!isActiveWindow())
            close();
    }
}
