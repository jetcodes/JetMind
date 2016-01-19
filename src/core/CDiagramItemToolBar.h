/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMITEMTOOLBAR_H
#define PRODRAW_CDIAGRAMITEMTOOLBAR_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

class CDiagramItem;
class CDiagramItemToolBar;

class CDiagramItemToolBar : public CDiagramToolBar
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramItemToolBar();
	virtual ~CDiagramItemToolBar();

	enum { WidgetType = CustomType + 2};
	virtual int widgetType() { return WidgetType; }

public Q_SLOTS:
	void onAutoHide(bool);
	//void onHoverLeave();
	void onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &r);
	void onAnchorTypeChanged(CDiagramWidget::AnchorType value);

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

}; // End of class CDiagramItemToolBar

#endif // PRODRAW_CDIAGRAMITEMTOOLBAR_H
