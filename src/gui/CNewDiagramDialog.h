/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/
#ifndef PRODRAW_CNEWDIAGRAMDIALOG_H
#define PRODRAW_CNEWDIAGRAMDIALOG_H

#include <QtGui>
#include <core/CDiagramTemplateManager>
#include <widgets/CUiAbstractDialog>

namespace Ui {
class CNewDiagramDialog;
}

class CNewDiagramDialog : public CUiAbstractDialog
{
	Q_OBJECT

public:
	CNewDiagramDialog(QWidget *parent = 0);
	~CNewDiagramDialog();

    void setup();
	CDiagramTemplate* selectedTemplate();

public Q_SLOTS:
	void onOk();
    void onCancel();
	void onTemplateSelected(QListWidgetItem *item);

private:
	Ui::CNewDiagramDialog				*m_ui;
    CDiagramTemplate					*m_template;

}; // End of class CNewDiagramDialog

#endif // PRODRAW_CNEWDIAGRAMDIALOG_H
