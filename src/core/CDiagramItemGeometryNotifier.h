/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMITEMGEOMETRYNOTIFIER_H
#define PRODRAW_CDIAGRAMITEMGEOMETRYNOTIFIER_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramSprite>

class CORE_EXPORT CDiagramItemGeometryNotifier : public CDiagramSprite
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramItemGeometryNotifier();
	virtual ~CDiagramItemGeometryNotifier();
	
	enum { SpriteType = CustomType + 5 };
	virtual int spriteType() { return SpriteType; }

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect);
	void uninit(CDiagram *d);
	
public Q_SLOTS:
	void onBoundedItemsMoving(qreal, qreal);
	void onBoundedItemsDragging();
	void onBoundedItemsDraggingStop(const QRectF &rect);
	void onBoundedItemsGeometryChanged(const QRectF &rect);
	void onBoundedItemsRotationChanged(const qreal);
	void onBoundedItemsResizingStop(const QRectF &rect);
	void onBoundedItemsRotatingStop(qreal value);
	
protected:
	void reset(QList<CDiagramItem*> &items, const QRectF &rect);

	QGraphicsTextItem	*m_text;
	bool				m_triggered;

}; // End of class CDiagramItemGeometryNotifier

#endif // PRODRAW_CDIAGRAMITEMGEOMETRYNOTIFIER_H
