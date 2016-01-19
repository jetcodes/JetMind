/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMNODEHANDLER_H
#define PRODRAW_CDIAGRAMNODEHANDLER_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramItemHandler>

class CDiagram;
class CDiagramPort;
class CDiagramNode;
class CDiagramNodeHandler;

/**
 * CDiagramNodeHandler is the box surrounding the nodes, providing the resize, rotation
 * ability for the embeded nodes via serveral ctrl buttons.
 */
class CORE_EXPORT CDiagramNodeHandler : public CDiagramItemHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramNodeHandler();
	virtual ~CDiagramNodeHandler() {}
	
	enum { SpriteType = CustomType + 2 };
	virtual int spriteType() { return SpriteType; }

	// The positions of ctrl buttons
	//
	// !IMPORTANT: Must be the order or counter-clockwise 
	// from the Top position
	enum Positions {
		Top = 0,
		LeftTop,
		Left,
		LeftBottom,
		Bottom,
		RightBottom,
		Right,
		RightTop,
		Center,
		OutsideBox
	}; // End of positions

	// Overrided member functions
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);	

	virtual bool support(const QList<CDiagramItem*> &items);
	void init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect);
	void uninit(CDiagram *d);

	void startDragging(CDiagramItemHandlerDragger *d);
	void doDragging(CDiagramItemHandlerDragger *d);
	void finishDragging(CDiagramItemHandlerDragger *d);

public Q_SLOTS:
	void onBoundedItemsDraggingStop(const QRectF &rect);
	void onBoundedItemsGeometryChanged(const QRectF &rect);
	void onBoundedItemsPropertyChanged(const QString &name, const QVariant &value);

protected:
	void doResize(CDiagramItemHandlerDragger *r);
	void doRotate(CDiagramItemHandlerDragger *r);
	void doMove(CDiagramItemHandlerDragger *r);
	void checkRatio(qreal &w, qreal &h, qreal &xratio, qreal &yratio);
	void initDraggers();
	void layoutDraggers();
	void applyLimits();
	bool keepSizeRatio();
	void setResizable(bool value);
	void setRotatable(bool value);
	void checkSizeAnchor(CDiagramItemHandlerDragger *dragger);

	virtual void updateGeometry(const QRectF &rect);
	virtual void updateItemsGeometry(qreal xratio, qreal yratio, QPointF &op, QList<QRectF> &oldGeometry, QList<QRectF> &newGeometry);

	bool				m_rotatable;
	bool				m_resizable;
	qreal				m_rotateRadius;
	QRectF				m_tmpGeometry;

	bool				m_snapGrid;
	bool				m_snapResize;
	bool				m_snapRotate;
	int					m_rotateSnap;
	int					m_resizeSnap;
	QGraphicsLineItem	*m_anchorXLine;
	QGraphicsLineItem	*m_anchorYLine;

private:
	Q_DISABLE_COPY(CDiagramNodeHandler)

public:
	static Qt::CursorShape	CursorShapes[10];

	friend class CDiagram;

}; // end of class CDiagramNodeHandler.

#endif // PRODRAW_CDIAGRAMNODEHANDLER_H
