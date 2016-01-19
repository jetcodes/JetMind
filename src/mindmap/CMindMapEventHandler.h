/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPEVENTHANDLER_H
#define PRODRAW_CMINDMAPEVENTHANDLER_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramCommand>
#include <core/CDiagramEventHandler>

class CDiagramPrivateData;
class CDiagramEventHandler;
class CMindMapEventHandler;
class CMindMapNode;
class CMindMapUiGraphMenu;

class CMindMapDragImage: public QGraphicsItem
{
public:
	CMindMapDragImage();
	virtual ~CMindMapDragImage() {}

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void updateDraggedItemsRect(CDiagram *d, QGraphicsItem *item);

protected:
	QRectF				m_rect;
	QPixmap				*m_pixmap;

}; // End of class CMindMapDragImage

class CMindMapEventHandler : public CDiagramEventHandler
{

public:
	CMindMapEventHandler();
	virtual ~CMindMapEventHandler();

	virtual bool onEvent(CDiagramPrivateData *d, QEvent *event);

protected:
	virtual bool keyPressEvent(QKeyEvent *event);
	virtual bool dragEnterEvent(QGraphicsSceneDragDropEvent *event);
	virtual bool dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool dropEvent(QGraphicsSceneDragDropEvent *event);

protected:
	CDiagramPrivateData		*m_pdata;
	QPointF					m_draggedImageInitPos;
	QGraphicsItem			*m_draggedImage;
	qreal					m_areaSize;
	QGraphicsLineItem		*m_indicatorLine;
	QGraphicsRectItem		*m_indicatorRect;
	CMindMapNode			*m_target;
    CMindMapNode            *m_tempNode;

}; // End of class CMindMapEventHandler

#endif // PRODRAW_CMINDMAPEVENTHANDLER_H
