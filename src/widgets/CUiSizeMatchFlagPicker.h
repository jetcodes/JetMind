/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUISIZEMATCHFLAGPICKER_H
#define PRODRAW_CUISIZEMATCHFLAGPICKER_H

#include <QtGui>
#include <core/CDiagram>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiSizeMatchFlagPicker : public QMenu
{
	Q_OBJECT

public:
	CUiSizeMatchFlagPicker(QWidget *parent = 0);
	virtual ~CUiSizeMatchFlagPicker();

	QIcon getIcon(Global::MatchSizeFlag value);
	const QList<QIcon>& icons() { return m_icons; }
	Global::MatchSizeFlag defaultEqualFlag() { return m_default; }

Q_SIGNALS:
	void equalFlagSelected(Global::MatchSizeFlag, QIcon);

public Q_SLOTS:
	void onActionTriggered(QAction *act);

protected:
	void setupFlags();
	void setupMenu();

	QList<Global::MatchSizeFlag>	m_flags;
	QList<QIcon>					m_icons;
	QList<QString>					m_names;
	QList<QAction*>					m_actions;
	Global::MatchSizeFlag			m_default;
};

#endif // PRODRAW_CUISIZEMATCHFLAGPICKER_H
