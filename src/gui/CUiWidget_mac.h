/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIWIDGET_MAC_H
#define PRODRAW_CUIWIDGET_MAC_H

#include <QtGui>

class CUiWidget_mac;
class CActionManager;

class CUiWidget_mac: public QMainWindow
{
	Q_OBJECT

public:
	CUiWidget_mac(QWidget *parent = NULL);
	virtual ~CUiWidget_mac();

Q_SIGNALS:
	void fullscreen(bool);
    void geometryChanged(const QRect &);

public Q_SLOTS:
	void onFullScreen();

protected:
	void setupUi(QLayout *globalLayout, CActionManager *actionManager);
	void toggleFullScreen(bool value);
    void resizeEvent(QResizeEvent *evt);

    virtual bool beforeEnterFullScreen();
    virtual bool afterExitFullScreen();

    QWidget         *m_centralWidget;
}; // End of class CUiWidget_mac

#endif // PRODRAW_CUIWIDGET_MAC_H
