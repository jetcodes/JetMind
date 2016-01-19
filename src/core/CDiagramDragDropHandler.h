/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMDRAGDROPHANDLER_H
#define PRODRAW_CDIAGRAMDRAGDROPHANDLER_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramEventHandler>

class CDiagramPrivateData;
class CDiagramDragDropHandler;

class CDiagramDragDropHandler : public CDiagramEventHandler
{
public:
	CDiagramDragDropHandler();
	virtual ~CDiagramDragDropHandler();

	bool onEvent(CDiagramPrivateData *p, QEvent *event);

protected:
	virtual bool dragEnterEvent(QGraphicsSceneDragDropEvent *event);
	virtual bool dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	virtual bool dropEvent(QGraphicsSceneDragDropEvent *event);

private:
	Q_DISABLE_COPY(CDiagramDragDropHandler);

	void acceptResourceDrop(QGraphicsSceneDragDropEvent* event);
	void acceptNewItemDrop(QGraphicsSceneDragDropEvent* event);
	void acceptDraggingDrop(QGraphicsSceneDragDropEvent* event);
	void acceptFormatDrop(QGraphicsSceneDragDropEvent* event);

	void scrollView(const QPointF &p);
	void alignSymbols(const QPointF &startPos, const QPointF &endPos);
	void checkModifiers(QGraphicsSceneDragDropEvent *event, const QPointF &startPos, const QPointF &endPos);

	QPointF					m_draggedImageInitPos;
	QGraphicsItem			*m_draggedImage;
	QGraphicsLineItem		*m_anchorXLine;
	QGraphicsLineItem		*m_anchorYLine;
	QGraphicsRectItem		*m_rectIndicator;
	QGraphicsLineItem		*m_lineIndicator;
	CDiagramPrivateData		*m_pdata;

}; // End of class CDiagramDragDropHandler

#endif // PRODRAW_CDIAGRAMDRAGDROPHANDLER_H
