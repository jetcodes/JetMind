/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/

#include "CHelpDialog.h"
#include "ui_CHelpDialog.h"

CHelpDialog::CHelpDialog(QWidget *parent)
    : CUiAbstractDialog(parent),
      m_ui(NULL)
{
    m_ui = new Ui::CHelpDialog();
	m_ui->setupUi(m_main);

    resize(450, 600);
	setTitle(tr("Help"));
	setModal(false);
}

CHelpDialog::~CHelpDialog()
{
	delete m_ui;
}
