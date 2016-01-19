/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUICORENODEPICKER_H
#define PRODRAW_CUICORENODEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <core/CDiagramItemTypeCategory>

class WIDGETS_EXPORT CUiCoreNodePicker : public QMenu 
{
	Q_OBJECT

public:
	CUiCoreNodePicker(QWidget *parent = 0);
	virtual ~CUiCoreNodePicker();

	void clearAll();
	void addMenu( CDiagramItemType *w);
	void addMenu( CDiagramItemTypeCategory *ctg);
	CDiagramItemType* defaultShape();

Q_SIGNALS:
	void shapeSelected( CDiagramItemType *w );

public Q_SLOTS:
	void onActionTriggered(QAction *act);

protected:
	void setupMenus();

	QList<QAction*>				m_actions;
	QList<CDiagramItemType*>	m_objects;

}; // end of CUiCoreNodePicker

#endif // PRODRAW_CUICORENODEPICKER_H
