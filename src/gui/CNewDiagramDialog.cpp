/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/
#include <core/CDiagramContext>
#include <core/CDiagramTemplateManager>

#include "CAppSettings.h"
#include "CNewDiagramDialog.h"
#include "ui_CNewDiagramDialog.h"

CNewDiagramDialog::CNewDiagramDialog(QWidget *parent)
	: CUiAbstractDialog(parent),
	  m_template(NULL)
{
	m_ui = new Ui::CNewDiagramDialog();
	m_ui->setupUi(m_main);

	m_ui->m_templates->setFlow(QListView::LeftToRight);
	m_ui->m_templates->setViewMode( QListView::IconMode );
	m_ui->m_templates->setIconSize( QSize(116,98) );
	m_ui->m_templates->setSpacing(3);

    setTitle(tr("New Diagram"));
    resize(400, 300);

	connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(m_ui->m_templates, SIGNAL(itemDoubleClicked (QListWidgetItem*)), this, SLOT(onTemplateSelected(QListWidgetItem*)));	
}

CNewDiagramDialog::~CNewDiagramDialog()
{
	delete m_ui;
}

void CNewDiagramDialog::setup()
{
    QStringList					diagramTypes;
	QList<CDiagramTemplate*>	tpls;
	QListWidgetItem				*item = NULL;
	CDiagramTemplateManager		*templateManager = NULL;
	
	templateManager = CDiagramTemplateManager::instance();
	if (!templateManager)
		return;
	
	diagramTypes = templateManager->diagramTypes();
	foreach (QString type, diagramTypes)
	{
		tpls.append( templateManager->getTemplates(type) );
	}

	// clear the old ones
	m_template = NULL;
	m_ui->m_templates->clear();
    foreach (CDiagramTemplate *tpl, tpls)
	{
		qDebug() << tpl->filePath << " tpl:" << tpl;
		item = new QListWidgetItem(tpl->icon, tpl->title, m_ui->m_templates);
		item->setData(Qt::UserRole, qVariantFromValue(tpl));
		m_ui->m_templates->addItem( item );
	}

	m_ui->m_templates->setCurrentRow(0);
}

CDiagramTemplate* CNewDiagramDialog::selectedTemplate()
{
	return m_template;
}

void CNewDiagramDialog::onOk()
{
	QListWidgetItem		*item = NULL;
	
	item = m_ui->m_templates->currentItem();
	if (item)
		m_template = item->data(Qt::UserRole).value<CDiagramTemplate*>();
	else
		m_template = NULL;

	accept();
}

void CNewDiagramDialog::onCancel()
{
	m_template = NULL;

	reject();
}

void CNewDiagramDialog::onTemplateSelected(QListWidgetItem *item)
{
	if (item)
		m_template = item->data(Qt::UserRole).value<CDiagramTemplate*>();
	accept();
}
