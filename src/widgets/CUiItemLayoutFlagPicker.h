/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIITEMLAYOUTFLAGPICKER_H
#define PRODRAW_CUIITEMLAYOUTFLAGPICKER_H

#include <QtGui>

class CUiItemLayoutFlagPicker : public QMenu
{
	Q_OBJECT

public:
	CUiItemLayoutFlagPicker(QWidget *parent);
	virtual ~CUiItemLayoutFlagPicker();

	QIcon getIcon(Qt::Orientation value);
	const QList<QIcon>& icons() { return m_icons; }
	Qt::Orientation defaultLayoutFlag() { return m_default; }

Q_SIGNALS:
	void layoutFlagSelected(Qt::Orientation orientation, QIcon);

public Q_SLOTS:
	void onActionTriggered(QAction *act);

private:
	void setupFlags();
	void setupMenu();

	QList<Qt::Orientation>		m_flags;
	QList<QIcon>				m_icons;
	QList<QString>				m_names;
	QList<QAction*>				m_actions;
	Qt::Orientation				m_default;

}; // End of class CUiItemLayoutFlagPicker

#endif // PRODRAW_CUIITEMLAYOUTFLAGPICKER_H
