/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMITEMHANDLER_H
#define PRODRAW_CDIAGRAMITEMHANDLER_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramSprite>

class CDiagram;
class CDiagramItem;
class CDiagramItemHandler;

// Common used dragger for item handlers. Developers can inherits
// this class to provide more complex draggers.
class CORE_EXPORT CDiagramItemHandlerDragger: public QGraphicsItem
{
public:
	CDiagramItemHandlerDragger(CDiagramItemHandler *parent, int index, int size, int type = CDiagramItemHandlerDragger::Square);
	virtual ~CDiagramItemHandlerDragger() {}

	// Shape types
	enum ShapeType {
		Circle = 0,
		Diamond,
		Square,
		Curve
	}; // End of ShapeType

	// The type in the graphics view system
	enum { Type = UserType + Global::DiagramItemHandlerDragger };
	int type() const { return Type; }
	int index() { return m_index; }

	// Overrided member functions
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);	

	void setDragging(bool value) { m_dragging = value; }
	bool isDragging() { return m_dragging; }
	void setHovered(bool value) { m_hovered = value; }
	bool isHovered() { return m_hovered; }
	const QPainterPath path() { return m_path; }
	void setUsed(bool value);
	bool used() { return m_used; }
	void setDragPolicy(Global::DragPolicy p);
	Global::DragPolicy dragPolicy() { return m_policy; }

	void resetPen();
	void setPen(const QPen &pen);
	QPen pen();

	void resetBrush();
	void setBrush(const QBrush &brush);
	QBrush brush();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event );
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event );

	virtual const QPainterPath computePath();

protected:

	CDiagramItemHandler *m_p;
	QPen				m_pen;
	QPen				m_stroke;
	QBrush				m_brush;
	qreal				m_size;
	bool				m_used;
	Global::DragPolicy	m_policy;
	int					m_index;
	bool				m_dragging;
	bool				m_hovered;
	QPainterPath		m_path;
	int					m_type;

private:
	Q_DISABLE_COPY(CDiagramItemHandlerDragger)

public:
	static Qt::CursorShape CursorShapes[3];

}; // End of class CDiagramItemHandlerDragger

class CORE_EXPORT CDiagramItemHandler: public CDiagramSprite
{
	Q_OBJECT

public:
	CDiagramItemHandler();
	virtual ~CDiagramItemHandler();

	enum { Type = UserType + Global::DiagramItemHandler };
	int type()	const { return Type; };

	// Overrided member functions inherited from the
	// parent class.
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	
	virtual void setGeometry( const QRectF &rect );
	qreal width() { return m_width; }
	qreal height() { return m_height; }

	virtual bool isChanging();
	virtual bool support(const QList<CDiagramItem*> &items);
	virtual void init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect);
	virtual void uninit(CDiagram *d);

	void grabFocus();
	
	virtual void startDragging(CDiagramItemHandlerDragger *g);
	virtual void doDragging(CDiagramItemHandlerDragger *g);
	virtual void finishDragging(CDiagramItemHandlerDragger *g);

public Q_SLOTS:
	virtual void onBoundedItemsMoving(qreal, qreal);
	virtual void onBoundedItemsDragging();
	virtual void onBoundedItemsDraggingStop(const QRectF &rect);
	virtual void onBoundedItemsGeometryChanged(const QRectF &rect);
	virtual void onBoundedItemsRotationChanged(const qreal);

protected:
	virtual void initDraggers();
	virtual void layoutDraggers();
	virtual void layout();

protected:
	Q_DISABLE_COPY(CDiagramItemHandler)

	CDiagram				*m_d;
	qreal					m_width;
	qreal					m_height;
	qreal					m_draggerSize;
	QPen					m_pen;

	QList<CDiagramItemHandlerDragger*>	m_draggers;
	CDiagramItemHandlerDragger			*m_dragger;
	QList<CDiagramItem*>				m_items;

}; // End of class CDiagramItemHandler

#endif // PRODRAW_CDIAGRAMITEMHANDLER_H