/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIPAGESIZEPICKER_H
#define PRODRAW_CUIPAGESIZEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiPageSizePicker : public QMenu
{
	Q_OBJECT

public:
	CUiPageSizePicker(QWidget *parent);
	virtual ~CUiPageSizePicker();

	const QList<QIcon>& icons() { return m_icons; }
	int defaultPageSize() { return m_default; }

Q_SIGNALS:
	void pageSizeChanged(const QSizeF &);

public Q_SLOTS:
	void onActionTriggered(QAction *act);

protected:
	void setupSizes();
	void setupMenu();

	QList<QIcon>				m_icons;
	QList<QString>				m_names;
	QList<QAction*>				m_actions;
	QList<QSizeF>				m_sizes;

	int							m_default;

};

#endif // PRODRAW_CUIPAGESIZEPICKER_H
