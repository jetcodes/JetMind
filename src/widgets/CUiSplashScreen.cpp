/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <widgets/CUiSplashScreen>

CUiSplashScreen::CUiSplashScreen(const QPixmap& pixmap)
{
	QSplashScreen::setPixmap(pixmap);
}
 
CUiSplashScreen::~CUiSplashScreen()
{
}

void CUiSplashScreen::drawContents(QPainter *painter)
{
	QPixmap textPix = QSplashScreen::pixmap();
	painter->setPen(m_color);
	painter->drawText(m_rect, m_alignement, m_message);
}

void CUiSplashScreen::setImage(const QPixmap &pixmap)
{
	setPixmap(pixmap);
}

void CUiSplashScreen::showStatusMessage(const QString &message, const QColor &color)
{
	m_message = message;
	m_color = color;
	showMessage(m_message, m_alignement, m_color);
}
 
void CUiSplashScreen::setMessageRect(QRect rect, int alignement)
{
	m_rect = rect;
	m_alignement = alignement;
}