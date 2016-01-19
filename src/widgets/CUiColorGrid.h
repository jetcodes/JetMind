/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUICOLORGRID_H
#define PRODRAW_CUICOLORGRID_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiColorGrid : public QWidget
{
	Q_OBJECT

public:
	CUiColorGrid(QWidget *parent = 0);
	virtual ~CUiColorGrid();

	virtual QSize minimumSizeHint() const;

signals:
	void colorHovered(QColor c);
	void colorSelected(QColor c);
	void colorSelectionDone();

protected:
	void setupColors();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );

private:
	qreal			m_gridSize;
	qreal			m_space;
	int				m_columns;
	int				m_rows;
	int				m_currRow;
	int				m_currColumn;
	int				m_currColor;
	QList<QColor>	m_colors;

}; // End of class CUiColorGrid

#endif // PRODRAW_CUICOLORGRID_H
