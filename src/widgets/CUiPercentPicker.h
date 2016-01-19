/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIPERCENTPICKER_H
#define PRODRAW_CUIPERCENTPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiPercentPicker : public QMenu
{
	Q_OBJECT

public:
	CUiPercentPicker(QWidget *parent);
	virtual ~CUiPercentPicker();

	const QList<QIcon>& icons() { return m_icons; }
	qreal defaultPercentage() { return m_default; }

Q_SIGNALS:
	void percentageChanged(int);

public Q_SLOTS:
	void onActionTriggered(QAction *act);

protected:
	void setupPercentages();
	void setupMenu();

	QList<QIcon>				m_icons;
	QList<QString>				m_names;
	QList<QAction*>				m_actions;
	QList<int>					m_percs;

	qreal						m_default;
	
};

#endif // PRODRAW_CUIPERCENTPICKER_H
