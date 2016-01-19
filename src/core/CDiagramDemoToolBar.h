/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMDEMOTOOLBAR_H
#define PRODRAW_CDIAGRAMDEMOTOOLBAR_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

class CDiagramDemoToolBar : public CDiagramToolBar
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramDemoToolBar();
	virtual ~CDiagramDemoToolBar();

	enum { WidgetType = CustomType + 3};
	virtual int widgetType() { return WidgetType; }

	void init(CDiagram *d);
	void uninit(CDiagram *d);

public Q_SLOTS:
	void onDiagramModeChanged(CDiagram::DiagramMode mode);
	void onDiagramVisibleRectChanged(const QRectF &);

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

}; // End of class CDiagramDemoToolBar

#endif // PRODRAW_CDIAGRAMDEMOTOOLBAR_H
