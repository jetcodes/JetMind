/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CQBUSYINDICATOR_H
#define PRODRAW_CQBUSYINDICATOR_H

#include <QtGui>
#include <widgets/CUiGlobal>

class WIDGETS_EXPORT CUiBusyIndicator : public QWidget
{
	Q_OBJECT

public:
	CUiBusyIndicator(QWidget *parent = NULL);
	virtual ~CUiBusyIndicator();
	
	int delay();
	void setDelay(int value);

	bool isAnimated();

	void setColor(const QColor &value);
	QColor color();

	void setSize(int w, int h);
	QSize size() const;
	virtual QSize sizeHint() const;

public Q_SLOTS:
	void start(const QPoint &pos = QPoint());
	void stop();

protected:
	virtual void timerEvent(QTimerEvent *event);
	virtual void paintEvent(QPaintEvent *event);

private:
	int		m_angle;
	int		m_timer;
	int		m_delay;
	QColor	m_color;

}; // End of class CUiBusyIndicator

#endif // PRODRAW_CQBUSYINDICATOR_H
