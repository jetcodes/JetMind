/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/
#ifndef PRODRAW_CHELPDIALOG_H
#define PRODRAW_CHELPDIALOG_H

#include <QtGui>
#include <widgets/CUiAbstractDialog>

namespace Ui {
    class CHelpDialog;
}

class CHelpDialog : public CUiAbstractDialog
{
	Q_OBJECT

public:
	CHelpDialog(QWidget *parent = 0);
	virtual ~CHelpDialog();

private:
	Ui::CHelpDialog		*m_ui;

}; // End of class CHelpDialog

#endif // PRODRAW_CHELPDIALOG_H
