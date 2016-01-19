/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIPENWEIGHTPICKER_H
#define PRODRAW_CUIPENWEIGHTPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiPenWeightPicker : public QMenu
{
	Q_OBJECT

public:
	CUiPenWeightPicker(QWidget *parent = 0, int cols = 1, int rows = 10, int w = 128, int h = 24);
	virtual ~CUiPenWeightPicker();

	virtual QSize minimumSizeHint() const;

signals:
	void weightSelected(qreal);
	void weightHovered(qreal);

protected:
	void setupLineWeights();
	
	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

private:
	QList<qreal>		m_widths;
	int					m_gridWidth;
	int					m_gridHeight;
	int					m_columns;
	int					m_rows;
	int					m_space;
	int					m_currWidthIndex;
	int					m_radius;
	QPointF				m_mousePos;
	qreal				m_defaultWidth;

}; // End of class CUiPenWeightPicker

#endif // PRODRAW_CUIPENWEIGHTPICKER_H
