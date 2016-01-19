/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMSHAPEITEM_H
#define PRODRAW_CDIAGRAMSHAPEITEM_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagramItem>
#include <core/CDiagramText>

class CDiagram;
class CDiagramPort;
class CDiagramEdge;
class CDiagramNode;

// CDiagramNodeStyle is used to style the shape items
// of the diagrams.
class CORE_EXPORT CDiagramNodeStyle
{
public:
	CDiagramNodeStyle();
	CDiagramNodeStyle(const QPen &pen, const QBrush &brush);

	void setBrush(const QBrush &brush) { m_brush = brush; }
	const QBrush& brush() { return m_brush; }

	void setPen(const QPen &pen) { m_pen = pen; }
	const QPen& pen() { return m_pen; }

	bool isGradient();

	void apply(CDiagramNode *item);

	bool operator==(const CDiagramNodeStyle &style) const;
	bool operator!=(const CDiagramNodeStyle &style) const;

private:
	QBrush			m_brush;
	QPen			m_pen;

}; // End of class CDiagramNodeStyle

// The abstract shape item
class CORE_EXPORT CDiagramNode: public CDiagramItem
{
	Q_OBJECT
	Q_PROPERTY(int labelPosition READ labelPosition WRITE setLabelPosition)
	Q_PROPERTY(bool connectable READ connectable WRITE setConnectable)

public:
	CDiagramNode(CDiagram* parent = NULL);
	virtual ~CDiagramNode();

	// The mode of the node
	enum Mode { 
		Normal = 0, 
		Linking, 
		Editing 
	}; // End of CDiagramNode mode
	
	// The position of the label text
	enum LabelPosition {
		Center = 0,
		Above,
		Below,
		Left,
		Right,
		InvalidLabelPosition
	}; // End of LabelPosition

	void setMode(CDiagramNode::Mode mode);
	CDiagramNode::Mode mode() { return m_mode; }

	int interType() const { return Global::Node; }

	// Overrided member functions inherited from the parent classes.
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	virtual QList<CDiagramPort*> getPorts() const;
	virtual QList<CDiagramPort*> getPortsBySide(int side) const;
	virtual CDiagramPort* getPortByIndex(int index) const;
	virtual CDiagramPort* getClosestPort(CDiagramNode* item) const;

	QSet<CDiagramEdge*> getEdges();
	void retrack();

	void setConnectable(bool value);
	bool connectable() const;

	void setLabelPosition(int value);
	int labelPosition();
	virtual bool supportLabelPlacement();

	virtual void anchorPoints(QList<qreal> &left, QList<qreal> &right, QList<qreal> &top, QList<qreal> &bottom, qreal &cx, qreal &cy);

public Q_SLOTS:
	virtual void onGeometryChanged();
	virtual void onChildResizeRequest(QGraphicsLayoutItem *source);
	virtual void layoutContent();

protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);	
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );

	virtual void initPorts();
	virtual void layoutPorts();
	void showPorts(bool value);
	
protected:
	QList<CDiagramPort*>		m_ports;

	CDiagramNode::Mode			m_mode;
	bool						m_connectable;
	int							m_labelPosition;
	qreal						m_padding;

	friend class CDiagram;

}; // end of class CDiagramItem

#endif // PRODRAW_CDIAGRAMSHAPEITEM_H