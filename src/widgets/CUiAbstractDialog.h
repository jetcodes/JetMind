/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIABSTRACTDIALOG_H
#define PRODRAW_CUIABSTRACTDIALOG_H

#include <QtGui>
#include <widgets/CUiGlobal>

namespace Ui {class CUiAbstractDialog;};

class WIDGETS_EXPORT CUiAbstractDialog : public QDialog
{
	Q_OBJECT

public:
    enum StyleMode {
        Normal,
        Custom
    }; // End of style mode

	CUiAbstractDialog(QWidget *parent = 0);
	virtual ~CUiAbstractDialog();

	void setTitle(const QString &value);
	QString title();

	virtual void paintEvent(QPaintEvent *event);

public Q_SLOTS:
	void onClose();

protected:
    void setStyleMode(CUiAbstractDialog::StyleMode mode);
    int styleMode() const;

	void resizeEvent(QResizeEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);

protected:
	Ui::CUiAbstractDialog		*ui;

	bool					m_dragging;
	QPoint					m_initPos;
	QPoint					m_clickPos;
	QWidget					*m_main;
    int                     m_styleMode;

}; // End of class CUiAbstractDialog

#endif // PRODRAW_CUIABSTRACTDIALOG_H
