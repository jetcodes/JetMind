/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUICONNECTSTYLEPICKER_H
#define PRODRAW_CUICONNECTSTYLEPICKER_H

#include <QtGui>
#include <core/CDiagramArrow>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiConnectStyle
{
public:
	CUiConnectStyle() {}

	CDiagramArrow	*startArrow;
	CDiagramArrow	*endArrow;
	Qt::PenStyle	penStyle;
	QIcon			icon;
}; // End of class CUiConnectStyle

class WIDGETS_EXPORT CUiConnectStylePicker : public QMenu
{
	Q_OBJECT

public:
	enum ArrowMode {
	StartArrow,
	EndArrow,
	BothArrows
	}; // End of ArrowMode

	CUiConnectStylePicker(QWidget *parent = 0);
	virtual ~CUiConnectStylePicker();

	CUiConnectStyle* defaultEdgeType();
	QSize sizeHint() const;

	void setArrowMode(int mode);
	int arrowMode() const;

Q_SIGNALS:
	void typeHovered(CUiConnectStyle*);
	void typeSelected(CUiConnectStyle*);

protected:
	void setupTypes();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

private:
	qreal				m_gridSize;
	qreal				m_space;
	int					m_arrowMode;
	int					m_columns;
	int					m_rows;
	int					m_currType;
	QList<CUiConnectStyle*>	m_types;
	QPointF				m_mousePos;
	CUiConnectStyle			*m_defaultType;

}; // End of class CUiConnectStylePicker

#endif // PRODRAW_CUICONNECTSTYLEPICKER_H
