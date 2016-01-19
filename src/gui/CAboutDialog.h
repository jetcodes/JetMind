/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/
#ifndef PRODRAW_CABOUTDIALOG_H
#define PRODRAW_CABOUTDIALOG_H

#include <QtGui>
#include <widgets/CUiAbstractDialog>

namespace Ui {class CAboutDialog;};

class CAboutDialog : public CUiAbstractDialog
{
	Q_OBJECT

public:
	CAboutDialog(QWidget *parent = 0);
	virtual ~CAboutDialog();

public Q_SLOTS:
	void onOk();

private:
	Ui::CAboutDialog *m_ui;

}; // End of class CAboutDialog

#endif // PRODRAW_CABOUTDIALOG_H
