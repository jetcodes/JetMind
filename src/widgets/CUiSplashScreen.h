/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUISPLASHSCREEN_H
#define PRODRAW_CUISPLASHSCREEN_H

#include <QtGui>
#include <widgets/CUiGlobal>

class CUiSplashScreen;

class WIDGETS_EXPORT CUiSplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	CUiSplashScreen(const QPixmap &pixmap = QPixmap());
	virtual ~CUiSplashScreen();
	
	virtual void drawContents(QPainter *painter);
	void setImage(const QPixmap &pixmap);
	void showStatusMessage(const QString &message, const QColor &color = Qt::black);
	void setMessageRect(QRect rect, int alignment = Qt::AlignLeft);
 
private:
	QString			m_message;
	int				m_alignement;
	QColor			m_color;
	QRect			m_rect;

}; // End of class 

#endif // PRODRAW_CUISPLASHSCREEN_H
