/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMEDGEITEM_H
#define PRODRAW_CDIAGRAMEDGEITEM_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagramItem>

class CDiagram;
class CDiagramPort;
class CDiagramText;
class CDiagramArrow;
class CDiagramEdgeStyle;
class CDiagramEdgeLabelInfo;

class CORE_EXPORT CDiagramEdgeControlPoint
{
public:	
	CDiagramEdgeControlPoint();

	void setX(qreal x, bool anchor=false);
	void setY(qreal y, bool anchor=false);

	bool anchorX() { return m_anchorX; }
	bool anchorY() { return m_anchorY; }

	void setPos(const QPointF &pos);
	QPointF& pos() { return m_pos; }

	void setUsed(bool value) { m_used = value; }
	bool used() { return m_used; }

	void setDirection(const QVector2D& dir);
	const QVector2D& direction() { return m_dir; }
		
	void clearAnchors();

	qreal x() { return m_pos.x(); }
	qreal y() { return m_pos.y(); }


protected:
	void toXml(QDomElement &n);
	void fromXml(QDomElement &n);

private:
	Q_DISABLE_COPY(CDiagramEdgeControlPoint)

	QPointF				m_pos;
	QVector2D			m_dir;
	bool				m_used;
	bool				m_anchorX;
	bool				m_anchorY;

}; // end of class CDiagramEdgeControlPoint

class CORE_EXPORT CDiagramEdge : public CDiagramItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF startPoint READ startPoint WRITE setStartPoint)
	Q_PROPERTY(QPointF endPoint READ endPoint WRITE setEndPoint)
	Q_PROPERTY(int startDirection READ startDirection WRITE setStartDirection)
	Q_PROPERTY(int endDirection READ endDirection WRITE setEndDirection)
	Q_PROPERTY(bool autoTrack READ autoTrack WRITE setAutoTrack)
	Q_PROPERTY(qreal startPercent READ startPercent WRITE setStartPercent)
	Q_PROPERTY(qreal endPercent READ endPercent WRITE setEndPercent)

public:
	Q_INVOKABLE CDiagramEdge(CDiagram *diagram);
	virtual ~CDiagramEdge(); 

	int interType() const { return Global::Edge; };

	enum LabelAnchor {
		Inline,
		Below,
		Above
	}; // End of label position

	enum EdgeDirection {
		Forward,
		Backward,
		BothDirections
	}; // End of EdgeDirection enums

	// Overrided member functions inherits from the parent classes.
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void retrack();
	void refresh();
	QPainterPath path();

	QList<CDiagramEdgeControlPoint*>& getCtrlPoints() { return m_ctrlPoints; }
	int activePoints() { return m_activePoints; }

	void setStartArrow(const CDiagramArrow *arrow);
	CDiagramArrow* startArrow() { return m_startArrow; }

	void setEndArrow(const CDiagramArrow *arrow);
	CDiagramArrow* endArrow() { return m_endArrow; }

	void setEdgeStyle(const CDiagramEdgeStyle *value);
	CDiagramEdgeStyle* edgeStyle() { return m_edgeStyle; }

	void setStartPort(CDiagramPort* conn);
	CDiagramPort* startPort() { return m_start; }

	void setEndPort(CDiagramPort* conn);
	CDiagramPort* endPort() { return m_end; }
	
	void setStartPoint(const QPointF &p);
	QPointF& startPoint() { return m_p1; }

	void setEndPoint(const QPointF &p);
	QPointF& endPoint() { return m_p2; }

	void setStartDirection(int d);
	int startDirection() { return m_d1; }

	void setEndDirection(int d);
	int endDirection() { return m_d2; }

	void addLabel(const QString &label, qreal pos = 0.5);
	void removeLabel(int index);
	QList<CDiagramText*> labels();

	void setLabelPosition(int index, qreal pos);
	qreal labelPosition(int index);

	void setLabelText(int index, const QString &value);
	QString labelText(int index);

	void setLabelAnchor(int index, CDiagramEdge::LabelAnchor value);
	CDiagramEdge::LabelAnchor labelAnchor(int index);

	void setAutoTrack(bool value);
	bool autoTrack();

	void setStartPercent(qreal value);
	qreal startPercent();

	void setEndPercent(qreal value);
	qreal endPercent();

	virtual void startGrowing(const QPointF &p);
	virtual bool onGrowing(const QPointF &p);
	virtual bool stopGrowing(const QPointF &p);

	void init(const QVariant &value);
	void defaultInit();

	int edgeDirection(CDiagramPort *port = NULL);
	void setEdgeDirection(int value);

protected:
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
	virtual void layoutControlPoints(const QList<QPointF> &points);
	virtual void layoutContent(const QPainterPath &path);

	virtual void extFromXml(const QDomElement &n);
	virtual void extToXml(QDomElement &n);

	void updateEdgeDirection();

protected:
	Q_DISABLE_COPY(CDiagramEdge)

	CDiagramPort					 *m_start;
	CDiagramPort					 *m_end;
	CDiagramArrow					 *m_startArrow;
	CDiagramArrow					 *m_endArrow;
	QPointF							 m_p1;
	QPointF							 m_p2;
	int								 m_d1;
	int								 m_d2;
	QList<CDiagramEdgeControlPoint*> m_ctrlPoints;
	int								 m_activePoints;

	qreal							 m_startPercent;
	qreal							 m_endPercent;

	QPainterPath					 m_path;
	CDiagramEdgeStyle				 *m_edgeStyle;

	bool							 m_lazyTrack;
	bool							 m_autoTrack;
	QGraphicsRectItem				 *m_hoverRect;
	QList<CDiagramEdgeLabelInfo>	 m_labels;

	int								 m_direction;
	friend class CDiagram;

}; // end of class CDiagramEdge

#endif // PRODRAW_CDIAGRAMEDGEITEM_H
