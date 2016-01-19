/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CQMESSAGEBOX_H
#define PRODRAW_CQMESSAGEBOX_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <widgets/CUiAbstractDialog>

namespace Ui {class CUiMessageBox;};

class WIDGETS_EXPORT CUiMessageBox : public CUiAbstractDialog
{
	Q_OBJECT

public:
	CUiMessageBox(QWidget *parent = 0);
	virtual ~CUiMessageBox();

	enum Button {
		Ok = 0,
		Cancel = 1,
		Yes = 2,
		No = 4,
		Save = 8,
		Discard = 16
	}; // End of enum buttons
	Q_DECLARE_FLAGS(Buttons, Button);

	void information(const QString &title, const QString &text, CUiMessageBox::Buttons buttons);
	void question(const QString &title, const QString &text, CUiMessageBox::Buttons buttons);

	virtual void paintEvent(QPaintEvent *event);
	int result();

public:
	static int information(QWidget *parent, const QString &title, const QString &text, CUiMessageBox::Buttons buttons = Ok);
	static int question(QWidget *parent, const QString &title, const QString &text, CUiMessageBox::Buttons buttons = Ok);

protected Q_SLOTS:
	void onButtonClicked(int);

private:
	Ui::CUiMessageBox *ui;

	int								m_result;
	QButtonGroup					*m_btnGroup;
	QMap<QAbstractButton*, int>		m_buttons;

}; // End of class CUiMessageBox

Q_DECLARE_OPERATORS_FOR_FLAGS(CUiMessageBox::Buttons);

#endif // PRODRAW_CQMESSAGEBOX_H
