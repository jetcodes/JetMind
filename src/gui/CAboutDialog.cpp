 /**
* Copyright (c) 2011 JetMind. All rights reserved.
*/
#include "CAboutDialog.h"
#include "ui_CAboutDialog.h"

CAboutDialog::CAboutDialog(QWidget *parent)
	: CUiAbstractDialog(parent)
{
	m_ui = new Ui::CAboutDialog();
    m_ui->setupUi(m_main);

	setTitle(tr("About JetMind"));
	connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));

    resize(420, 138);
}

CAboutDialog::~CAboutDialog()
{
    delete m_ui;
}

void CAboutDialog::onOk()
{
    accept( );
}
