/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CTITLEBAR_H
#define PRODRAW_CTITLEBAR_H

#include <QtGui>
#include "ui_CTitleBar.h"

class CTitleBar : public QWidget
{
	Q_OBJECT

public:
	CTitleBar(QWidget *parent = 0);
	virtual ~CTitleBar();

	void paintEvent(QPaintEvent *event);

	void setDocTitle(const QString &docTitle);
	QString docTitle();

Q_SIGNALS:
	void closeWindow();
	void maximize();
	void minimize();
	void restore();
	void move(QPoint);
	void openRecentFile(const QString &value);

public Q_SLOTS:
    void onDocTitleChanged(const QString &title);
	void onMaximize();
	void onRecentFileTriggered(QAction *act);

protected:
	void mousePressEvent(QMouseEvent *ev);	
	void mouseDoubleClickEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void enterEvent(QEvent *ev);
	void resizeEvent(QResizeEvent *ev);

	void setupMenus();
	void positionTitle();

protected:
	Ui::CTitleBar	*m_ui;

	QMenu			*m_file;
	QMenu			*m_edit;
	QMenu			*m_view;
	QMenu			*m_help;
	QActionGroup	*m_recentFilesGroup;

	QPoint			m_clickPos;
	QRect			m_oldGeometry;
	bool			m_maxNormal;

private:
    Q_DISABLE_COPY(CTitleBar)

}; // End of class CUiWidget

#endif // PRODRAW_CTITLEBAR_H
