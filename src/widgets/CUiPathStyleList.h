/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIPATHSTYLELIST_H
#define PRODRAW_CUIPATHSTYLELIST_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>

class WIDGETS_EXPORT CUiPathStyleList : public QMenu
{
	Q_OBJECT

public:
	CUiPathStyleList(QWidget *parent = 0);
	virtual ~CUiPathStyleList();

	void clearAll();
	void addMenu(CDiagramEdgeStyle *style);
	void addMenu(CDiagramEdgeStyleCategory *ctg);

	CDiagramEdgeStyle* defaultStyle();
	void setDefaultStyle(CDiagramEdgeStyle *style);

Q_SIGNALS:
	void styleHovered(CDiagramEdgeStyle*);
	void styleSelected(CDiagramEdgeStyle*);

public Q_SLOTS:
	void onActionHovered(QAction *act);
	void onActionTriggered(QAction *act);

protected:
	void setupMenus();

	QList<CDiagramEdgeStyle*>	m_styles;
	QList<QAction*>			m_actions;
	CDiagramEdgeStyle		*m_defaultStyle;

}; // end of CUiPathStyleList

#endif // PRODRAW_CUIPATHSTYLELIST_H
