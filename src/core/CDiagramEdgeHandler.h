/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMEDGEHANDLER_H
#define PRODRAW_CDIAGRAMEDGEHANDLER_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramItemHandler>

class CDiagram;
class CDiagramItem;
class CDiagramEdge;
class CDiagramPort;
class CDiagramItemHandler;
class CDiagramEdgeHandler;
class CDiagramItemHandlerDragger;

/**
* The class provides the control points of the links. The users can drag the
* ctrl button of the line to drag it around
*/
class CORE_EXPORT CDiagramEdgeDragger: public CDiagramItemHandlerDragger
{
public:
	CDiagramEdgeDragger(CDiagramEdgeHandler *parent, int index, int size);
	virtual ~CDiagramEdgeDragger() {}

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);
	
	bool isOverPort();
	CDiagramPort *currentPort();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);	

private:
	CDiagramEdgeDragger();
	Q_DISABLE_COPY(CDiagramEdgeDragger);

	bool			m_overPort;
	qreal			m_overPortSize;
	CDiagramPort	*m_port;

}; // End of class CDiagramEdgeDragger;

/**
 * CDiagramEdgeHandler is the box surrounding the nodes, providing the resize, rotation
 * ability for the embeded nodes via serveral ctrl buttons.
 */
class CORE_EXPORT CDiagramEdgeHandler : public CDiagramItemHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramEdgeHandler();
	virtual ~CDiagramEdgeHandler() {}

	enum { SpriteType = CustomType + 1 };
	virtual int spriteType() { return SpriteType; }

	// Overrided member functions inherited from the
	// parent class.
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	
	bool support(const QList<CDiagramItem*> &items);
	void init(CDiagram *d, QList<CDiagramItem *> &items, const QRectF &rect);

protected:
	void updateConnectableItems(bool linking);
	virtual void initDraggers();
	virtual void layoutDraggers();
	virtual void startDragging(CDiagramItemHandlerDragger *d);
	virtual void doDragging(CDiagramItemHandlerDragger *d);
	virtual void finishDragging(CDiagramItemHandlerDragger *d);

private:
	Q_DISABLE_COPY(CDiagramEdgeHandler)

	int					m_draggerNum;
	int					m_usedDraggersNum;
	bool				m_snapGrid;
	CDiagramEdge		*m_currEdge;

	friend class CDiagram;
	friend class CDiagramEdgeDragger;

public:
	static Qt::CursorShape  CursorShapes[3];

}; // end of class CDiagramEdgeHandler.

#endif // PRODRAW_CDIAGRAMEDGEHANDLER_H
