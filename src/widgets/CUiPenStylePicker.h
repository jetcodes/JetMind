/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIPENSTYLEPICKER_H
#define PRODRAW_CUIPENSTYLEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiPenStylePicker : public QMenu
{
	Q_OBJECT

public:
	CUiPenStylePicker(QWidget *parent = 0, int cols = 1, int rows=10, int w = 128, int h = 24);
	virtual ~CUiPenStylePicker();

	virtual QSize minimumSizeHint() const;

signals:
	void styleSelected(Qt::PenStyle);
	void styleHovered(Qt::PenStyle);

protected:
	void setupLineStyles();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

private:
	QList<Qt::PenStyle>			m_styles;
	int							m_gridWidth;
	int							m_gridHeight;
	int							m_columns;
	int							m_rows;
	int							m_space;
	int							m_currStyleIndex;
	int							m_radius;
	QPointF						m_mousePos;
	Qt::PenStyle				m_defaultStyle;

}; // End of class CUiPenStylePicker

#endif // PRODRAW_CUIPENSTYLEPICKER_H
