/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMCORE_H
#define PRODRAW_CDIAGRAMCORE_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramArrow>
#include <core/CDiagramEventHandler>
#include <core/CDiagramEdge>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramPathNode>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramEdgeStyleCategory>

#ifndef PI
#define PI 3.1415926
#endif // PI

class CORE_EXPORT CCoreNodes : public QObject, public CDiagramItemTypeCategory
{
	Q_OBJECT
public:
	CCoreNodes();
	void load();
};

class CORE_EXPORT CCoreArrows: public QObject, public CDiagramArrowCategory
{
	Q_OBJECT
	Q_INTERFACES(CDiagramArrowCategory)
public:
	CCoreArrows() : CDiagramArrowCategory("Core") { load(); }
	void load();
};

class CORE_EXPORT CCoreEdges : public QObject, public CDiagramEdgeStyleCategory
{
	Q_OBJECT
	Q_INTERFACES(CDiagramEdgeStyleCategory)
public:
	CCoreEdges() : CDiagramEdgeStyleCategory("Core") { load(); }
	void load();
};

/////////////////////////////////////////////////////////////
// Core Nodes
/////////////////////////////////////////////////////////////
class CORE_EXPORT Rect : public CDiagramPathNode
{
	Q_OBJECT
public:
	Q_INVOKABLE Rect(CDiagram *parent);
protected:
	QPainterPath computePath();
}; // End of class Rect

class CORE_EXPORT RoundedRect : public CDiagramPathNode
{
	Q_OBJECT
public:
	Q_INVOKABLE RoundedRect(CDiagram *parent);
protected:
	QPainterPath computePath();
}; // End of class RoundedRect

class CORE_EXPORT Square : public Rect
{
	Q_OBJECT
public:
	Q_INVOKABLE Square(CDiagram *parent = 0);
}; // End of class Square

class CORE_EXPORT RoundedSquare: public RoundedRect
{
	Q_OBJECT
public:
	Q_INVOKABLE RoundedSquare(CDiagram *parent = 0);
}; // End of class RoundedSquare

class CORE_EXPORT Circle : public CDiagramPathNode
{
	Q_OBJECT
public:
	Q_INVOKABLE Circle(CDiagram *parent = 0);
protected:
	QPainterPath computePath();
}; // End of class CCoreCircleShape

class CORE_EXPORT Ellipse : public Circle
{
	Q_OBJECT
public:
	Q_INVOKABLE Ellipse(CDiagram *parent = 0);
}; // End of class CCoreEllipseShape

/////////////////////////////////////////////////////////////
// Core Edge Styles
/////////////////////////////////////////////////////////////
class CORE_EXPORT OrthogonalEdge : public CDiagramEdgeStyle
{
public:
	OrthogonalEdge(const QString &ctg, const QString &name, const QIcon &icon) : CDiagramEdgeStyle(ctg, name, icon) {}
	void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;
}; // End of OrthogonalEdge

class CORE_EXPORT RoundedEdge : public CDiagramEdgeStyle
{
public:
	RoundedEdge(const QString &ctg, const QString &name, const QIcon &icon) : CDiagramEdgeStyle(ctg, name, icon) {}
	void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;
}; // End of RoundedEdge

class CORE_EXPORT CurveEdge : public CDiagramEdgeStyle
{
public:
	CurveEdge(const QString &ctg, const QString &name, const QIcon &icon) : CDiagramEdgeStyle(ctg, name, icon) {}
	void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;
}; // End of CurveEdge

class CORE_EXPORT StraightEdge : public CDiagramEdgeStyle
{
public:
	StraightEdge(const QString &ctg, const QString &name, const QIcon &icon) : CDiagramEdgeStyle(ctg, name, icon) {}
	void route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const;
	void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;
}; // End of StraightEdge

class CORE_EXPORT ArcEdge: public CDiagramEdgeStyle
{
public:
	ArcEdge(const QString &ctg, const QString &name, const QIcon &icon) : CDiagramEdgeStyle(ctg, name, icon) {}

	void route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const;
	void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;

}; // End of CArcEdge

/////////////////////////////////////////////////////////////
// Core Arrows
/////////////////////////////////////////////////////////////
class CORE_EXPORT NoneArrow: public CDiagramArrow
{
public:
	NoneArrow() { m_direction = BothDirections; m_name = QString("None"); }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
}; // End of class NoneArrow

class CORE_EXPORT DiamondArrow: public CDiagramArrow
{
public:
	DiamondArrow() { m_direction = Outward; m_name = QString("Empty Diamond"); m_width = m_length = 8; }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
}; // End of class DiamondArrow

class CORE_EXPORT TriangleArrow: public CDiagramArrow
{
public:
	TriangleArrow() { m_name = QString( "Empty Triangle"); m_width = 8; m_length = 6;  }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
}; // End of class TriangleArrow


class CORE_EXPORT FilledDiamondArrow: public CDiagramArrow
{
public:
	FilledDiamondArrow() { m_direction = Outward; m_name = QString("Filled Diamond"); m_width = m_length = 8; }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
}; // End of class FilledDiamondArrow

class CORE_EXPORT FilledTriangleArrow: public CDiagramArrow
{
public:
	FilledTriangleArrow() { m_name = QString( "Filled Triangle"); m_width = 8; m_length = 6;  }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
}; // End of class FilledTriangleArrow

class CORE_EXPORT AngleArrow: public CDiagramArrow
{
public:
	AngleArrow() { m_name = QString( "Angle"); m_width = 8; m_length = 6;  }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
};  // End of class AngleArrow

class CORE_EXPORT CurveArrow: public CDiagramArrow
{
public:
	CurveArrow() { m_name = QString( "Curve"); m_width = 8; m_length = 6;  }
	QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const;
};  // End of class CurveArrow

#endif // PRODRAW_CDIAGRAMCORE_H