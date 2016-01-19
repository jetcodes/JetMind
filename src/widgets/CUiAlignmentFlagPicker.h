/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIALIGNMENTFLAGPICKER_H
#define PRODRAW_CUIALIGNMENTFLAGPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiAlignmentFlagPicker : public QMenu 
{
	Q_OBJECT

public:
	CUiAlignmentFlagPicker(QWidget *parent = 0);
	virtual ~CUiAlignmentFlagPicker();

	const QList<QIcon>& icons() { return m_icons; }
	Qt::AlignmentFlag defaultAlignFlag() { return m_default; }
	QIcon getIcon(Qt::AlignmentFlag flag);

Q_SIGNALS:
	void alignFlagSelected(Qt::AlignmentFlag, QIcon);

public Q_SLOTS:
	void onActionTriggered(QAction *act);

protected:
	void setupAligns();
	void setupMenu();

	QList<Qt::AlignmentFlag>	m_aligns;
	QList<QIcon>				m_icons;
	QList<QString>				m_names;
	QList<QAction*>				m_actions;
	Qt::AlignmentFlag			m_default;

}; // end of CUiAlignmentFlagPicker

#endif // PRODRAW_CUIALIGNMENTFLAGPICKER_H
