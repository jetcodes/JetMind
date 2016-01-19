/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPPATHRENDERER_H
#define PRODRAW_CMINDMAPPATHRENDERER_H

#include <QtGui>

class CMindMapPathRenderer
{
public:
	virtual QPainterPath draw(QPainter *painter, const QList<QPointF> &points) = 0;

}; // End of class CMindMapPathRenderer

class CStraightLineRenderer: public CMindMapPathRenderer
{
public:
	CStraightLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CStraightLineRenderer

class CCurveLineRenderer: public CMindMapPathRenderer
{
public:
	CCurveLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CCurveLineRenderer

class CRoundedLineRenderer: public CMindMapPathRenderer
{
public:
	CRoundedLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CRoundedLineRenderer

class CAngledLineRenderer: public CMindMapPathRenderer
{
public:
	CAngledLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CAngledLineRenderer

class CPolygonLineRenderer: public CMindMapPathRenderer
{
public:
	CPolygonLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CPolygonLineRenderer

class CCartoonLineRenderer: public CMindMapPathRenderer
{
public:
	CCartoonLineRenderer();
	QPainterPath draw(QPainter *painter, const QList<QPointF> &points);

}; // End of class CCartoonLineRenderer

#endif // PRODRAW_CMINDMAPPATHRENDERER_H
