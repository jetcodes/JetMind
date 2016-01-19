/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMPORT_H
#define PRODRAW_CDIAGRAMPORT_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramEdge;
class CDiagramNode;

/**
 * CDiagramPort provides the function of connecting
 * links together.
 */
class CORE_EXPORT CDiagramPort : public QGraphicsItem
{
public:
	CDiagramPort(CDiagramNode *parent, int side, int index);
	virtual ~CDiagramPort();

	// The type of the new item in the graphics view system
	enum { Type = UserType + Global::DiagramPort };
	int type() const { return Type; };	
	int side() const { return m_side; }
	int index() const { return m_index; }

	// Overrided member functions inherited from the
	// parent class.
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	int removeEdge(CDiagramEdge *c);
	int addEdge(CDiagramEdge *c);
	QList<CDiagramEdge*> getEdges();
	
	void setConnectPosition(CDiagramEdge *edge, qreal value);
	qreal getConnectPosition(CDiagramEdge *edge) const;
	int getConnectPosition(CDiagramEdge *edge, QPointF& out) const;
	qreal getPercent(const QPointF &p);

	void retrack();

	CDiagramNode* getParentNode();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
	QMap<CDiagramEdge*, qreal>		m_edges;
	int								m_side;
	int								m_index;
	qreal							m_width;
	qreal							m_height;
	QPen							m_pen;
	QPen							m_stroke;
	QBrush							m_brush;
	bool							m_hover;

}; // End of class CDiagramPort 

#endif // PRODRAW_CDIAGRAMCONNECTOR_H
