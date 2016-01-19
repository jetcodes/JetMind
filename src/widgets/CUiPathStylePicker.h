/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIPATHSTYLEPICKER_H
#define PRODRAW_CUIPATHSTYLEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramEdgeStyleCategory>

class WIDGETS_EXPORT CUiPathStylePicker : public QMenu
{
	Q_OBJECT

public:
	CUiPathStylePicker(QWidget *parent = 0, int cols = 3, int rows=1, int w = 64, int h = 32);
	virtual ~CUiPathStylePicker();

	virtual QSize minimumSizeHint() const;

	void clearAll();
	void addStyle(CDiagramEdgeStyle *style);
	void addStyleCategory(CDiagramEdgeStyleCategory *ctg);

	CDiagramEdgeStyle* defaultStyle();
	void setDefaultStyle(CDiagramEdgeStyle *style);

Q_SIGNALS:
	void styleHovered(CDiagramEdgeStyle*);
	void styleSelected(CDiagramEdgeStyle*);

protected:
	void setupStyles();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

	QList<CDiagramEdgeStyle*>	m_styles;
	int							m_gridWidth;
	int							m_gridHeight;
	int							m_columns;
	int							m_rows;
	int							m_space;
	int							m_currStyleIndex;
	int							m_radius;
	QPointF						m_mousePos;
	CDiagramEdgeStyle			*m_defaultStyle;

}; // end of CUiPathStylePicker

#endif // PRODRAW_CUIPATHSTYLEPICKER_H
