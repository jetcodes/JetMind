/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/

#ifndef PRODRAW_CTEMPLATECREATIONDIALOG_H
#define PRODRAW_CTEMPLATECREATIONDIALOG_H

#include <QtGui>
#include <widgets/CUiAbstractDialog>

namespace Ui {class CTemplateCreationDialog;};

class CTemplateCreationDialog : public CUiAbstractDialog
{
	Q_OBJECT

public:
	CTemplateCreationDialog(QWidget *parent = 0);
	virtual ~CTemplateCreationDialog();

	void reset(const QString &title, const QStringList &types);

	QString getTitle();
	QString getType();

Q_SIGNALS:
	void saveTemplate();

public Q_SLOTS:
	void onOk();
	void onCancel();

private:
	Ui::CTemplateCreationDialog *ui;

}; // End of class CTemplateCreationDialog

#endif // PRODRAW_CTEMPLATECREATIONDIALOG_H
