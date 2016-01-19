/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIITEMORDERPICKER_H
#define PRODRAW_CUIITEMORDERPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiItemOrderPicker : public QMenu
{
	Q_OBJECT

public:
	CUiItemOrderPicker(QWidget *parent);
	virtual ~CUiItemOrderPicker();

	QIcon getDefaultIcon();
	const QList<QIcon>& icons() { return m_icons; }

Q_SIGNALS:
	void sendToFront();
	void sendToBack();
	void bringForwards();
	void bringBackwards();

public Q_SLOTS:
	void emitDefaultOrder();
	void onActionTriggered(QAction *act);

protected:
	void setupMoves();
	void setupMenu();

	int						m_defaultOrder;
	QList<QIcon>			m_icons;
	QList<QString>			m_names;
	QList<QAction*>			m_actions;
};

#endif // PRODRAW_CUIITEMORDERPICKER_H
