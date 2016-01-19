/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CQRULER_H
#define PRODRAW_CQRULER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiRuler : public QWidget
{
	Q_OBJECT

public:
	CUiRuler(QWidget *parent, int mode = CUiRuler::HorizontalMode );
	virtual ~CUiRuler();

	enum Mode {
		HorizontalMode = 0x01,
		VerticalMode = 0x02
	}; // End of Mode

	void paintEvent(QPaintEvent *event);

public Q_SLOTS:
	void onScaleChanged(qreal factor);
	void onScrollChanged(int step);
	void onRangeChanged(int min, int max);

private:
	int			m_lineHeight;
	int			m_tick;
	int			m_mode;
	int			m_startNumber;
	int			m_min;
	int			m_max;
	qreal		m_scale;

}; // End of class CUiRuler

#endif // PRODRAW_CQRULER_H
