/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/

#include "CTemplateCreationDialog.h"
#include "ui_CTemplateCreationDialog.h"

CTemplateCreationDialog::CTemplateCreationDialog(QWidget *parent)
	: CUiAbstractDialog(parent)
{
	ui = new Ui::CTemplateCreationDialog();
	ui->setupUi(m_main);

    setTitle(tr("Save As Template"));
	connect(ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    resize(300, 128);
}

CTemplateCreationDialog::~CTemplateCreationDialog()
{
	delete ui;
}

void CTemplateCreationDialog::onOk()
{
	emit saveTemplate();
	accept();
}

void CTemplateCreationDialog::onCancel()
{
	accept();
}

QString CTemplateCreationDialog::getTitle()
{
	return ui->m_title->text();
}

QString CTemplateCreationDialog::getType()
{
	return ui->m_types->currentText();
}

void CTemplateCreationDialog::reset(const QString &title, const QStringList &types)
{
	ui->m_title->setText(title);
	ui->m_types->clear();
	ui->m_types->addItems(types);
}
