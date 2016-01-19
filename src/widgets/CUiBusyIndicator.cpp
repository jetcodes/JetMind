/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include "CUiBusyIndicator.h"

CUiBusyIndicator::CUiBusyIndicator(QWidget *parent)
	: QWidget(parent),
	  m_angle(0),
	  m_timer(-1),
	  m_delay(80),
	  m_color(Qt::black)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
	setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::Popup);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowModality(Qt::ApplicationModal);
	resize(32,32);
}

CUiBusyIndicator::~CUiBusyIndicator()
{
	
}

void CUiBusyIndicator::setSize(int w, int h)
{
	if (w != h)
		h = w;
	resize(w, h);
	update();
}

QSize CUiBusyIndicator::size() const
{
	return QSize(width(), height());
}

QSize CUiBusyIndicator::sizeHint() const
{
	return QSize(width(), width());
}

bool CUiBusyIndicator::isAnimated()
{
	return (m_timer != -1);
}

void CUiBusyIndicator::setDelay(int value)
{
	if (value <= 0)
		return;

	if (m_timer != -1)
		killTimer(m_timer);

	m_delay = value;
	if (m_timer)
		m_timer = startTimer(m_delay);
}

int CUiBusyIndicator::delay()
{
	return m_delay;
}

void CUiBusyIndicator::setColor(const QColor &value)
{
	m_color = value;
	update();
}

QColor CUiBusyIndicator::color()
{
	return m_color;
}

void CUiBusyIndicator::start(const QPoint &pos)
{
	if (!isVisible())
		show();
	raise();

	m_angle = 0;
    if (m_timer == -1)
        m_timer = startTimer(m_delay);

	if (!pos.isNull())
		move(pos);
}

void CUiBusyIndicator::stop()
{
	 if (m_timer != -1)
        killTimer(m_timer);
    m_timer = -1;
    update();
	close();
}

void CUiBusyIndicator::timerEvent(QTimerEvent *event)
{
	m_angle = (m_angle + 30 ) % 360;
	update();
}

void CUiBusyIndicator::paintEvent(QPaintEvent *event)
{
	int			w;
	int			outerRadius, innerRadius;
    int			capsuleHeight;
    int			capsuleWidth;
    int			capsuleRadius;
	QPainter	p(this);
	QColor		color;

	w = qMin(width(), height());
	p.setRenderHint(QPainter::Antialiasing);
	outerRadius = (w - 1) * 0.5;
    innerRadius = (w - 1) * 0.5 * 0.38;
	capsuleHeight = outerRadius - innerRadius;
    capsuleWidth  = (w > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    capsuleRadius = capsuleWidth / 2;

	for (int i=0; i<12; i++)
    {
		color = m_color;
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}