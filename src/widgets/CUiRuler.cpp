/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <widgets/CUiRuler>

CUiRuler::CUiRuler(QWidget *parent, int mode)
	: QWidget(parent),
	  m_mode(mode),
	  m_lineHeight(4),
	  m_tick(10),
	  m_startNumber(0),
	  m_scale(1.0)
{
	
}

CUiRuler::~CUiRuler()
{

}

void CUiRuler::paintEvent(QPaintEvent *event)
{
	int			cnt = 0;
	int			margin = 15, start = 0, end = 0;
	int			textRectLength = m_tick * 5, textRectThick = height();
	int			textPadding = 2;
	QPainter	painter(this);
	QLine		line;

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setBrush(Qt::white);
	painter.setPen(Qt::NoPen);
	painter.drawRect( rect() );
	painter.setPen(Qt::gray);
	painter.setFont( QFont("Arial", 7, QFont::Normal) );
	if (m_mode == HorizontalMode)
	{
		end = height() - 1;
		cnt = (m_startNumber / 10) % 10;
		for (int i = 0; i < width() - 2; i += 10)
		{
			start = margin + i;
			if (cnt % 10 == 0 || i == 0)
				line.setPoints( QPoint(start, end), QPoint(start, 0) );
			else if (cnt % 5 == 0)
				line.setPoints( QPoint(start, end), QPoint(start, end - m_lineHeight * 2));
			else
				line.setPoints( QPoint(start, end), QPoint(start, end - m_lineHeight) );
			painter.drawLine(line);

			if (cnt % 10 == 0)
			{
				painter.drawText( QRect(start + textPadding, 0, textRectLength, textRectThick), 
								  Qt::AlignTop | Qt::AlignLeft, 
								  QString::number(m_startNumber + i));
			}
			cnt += 1;
		}
		painter.drawLine( QPoint(0, end), QPoint(width(), end));
	}
	else
	{
		end = width() - 1;
		cnt = (m_startNumber / 10) % 10;
		for (int i = 0; i < height() - 2; i += 10)
		{
			start = i;
			if (i == 0) {}
			else if (cnt % 10 == 0)
				line.setPoints( QPoint(end, start), QPoint(0, start) );
			else if (cnt % 5 == 0)
				line.setPoints( QPoint(end, start), QPoint(end - m_lineHeight * 2, start) );
			else
				line.setPoints( QPoint(end, start), QPoint(end - m_lineHeight, start) );
			painter.drawLine(line);

			if (cnt % 10 == 0)
			{
				painter.save();
				painter.rotate(-90);
				painter.drawText( QRect(-start -textRectLength, 0, textRectLength - textPadding, textRectThick), 
								  Qt::AlignRight | Qt::AlignTop, 
								  QString::number(m_startNumber + i));
				painter.restore();
			}

			cnt += 1;
		}
		painter.drawLine( QPoint(end, 0), QPointF(end, height()) );
	}
}

void CUiRuler::onScaleChanged(qreal factor)
{
	m_scale = factor;
	update();
}

void CUiRuler::onScrollChanged(int number)
{
	m_startNumber = ((number - m_min)/10) * 10;
	update();
}

void CUiRuler::onRangeChanged(int min, int max)
{
	m_min = min;
	m_max = max;
}