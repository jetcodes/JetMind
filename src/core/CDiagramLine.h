/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMLINEITEM_H
#define PRODRAW_CDIAGRAMLINEITEM_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramItem;

// CDiagramLine provides a line in the diagram.
class CORE_EXPORT CDiagramLine: public CDiagramItem
{
public:
	Q_INVOKABLE CDiagramLine(CDiagram *parent = NULL);
	Q_INVOKABLE CDiagramLine(const QPointF &p1, const QPointF &p2, CDiagram *parent = NULL);
	Q_INVOKABLE CDiagramLine(const QLineF &line, CDiagram *parent = NULL);
	Q_INVOKABLE CDiagramLine(qreal x1, qreal y1, qreal x2, qreal y2, CDiagram *parent = NULL);
	virtual ~CDiagramLine() {}

	int interType() { return Global::Line; }

	// Overrided member functions inherited from the parent classes.
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void setLine(const QPointF &p1, const QPointF &p2);
	void setLine(const QLineF &line);
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);
	const QLineF& line() { return m_line; }

protected:
	QLineF			m_line;
	
}; // End of class CDiagramLine

#endif // PRODRAW_CDIAGRAMLINEITEM_H
