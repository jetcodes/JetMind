/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMPROXYWIDGET_H
#define PRODRAW_CDIAGRAMPROXYWIDGET_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramNode>

class CORE_EXPORT CDiagramProxyWidget : public CDiagramNode
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramProxyWidget(CDiagram *d = NULL);
	virtual ~CDiagramProxyWidget();

	void setWidget(QWidget *widget);
	QWidget* widget();

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0); 

public Q_SLOTS:
	virtual void onGeometryChanged();

protected:
	virtual bool sceneEventFilter( QGraphicsItem * watched, QEvent * event);
	virtual void layout();
	
	QWidget					*m_widget;
	QGraphicsProxyWidget	*m_proxy;

private:
	Q_DISABLE_COPY(CDiagramProxyWidget)

}; // End of class CDiagramProxyWidget

#endif // PRODRAW_CDIAGRAMPROXYWIDGET_H
