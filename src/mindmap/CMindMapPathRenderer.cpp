/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>

#include "CMindMapPathRenderer.h"

CStraightLineRenderer::CStraightLineRenderer()
{
}

QPainterPath CStraightLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	QPainterPath	path;

	if (!painter || points.length() < 2)
		return path;

	path.moveTo(points.first());
	path.lineTo(points.last());

	painter->strokePath( path, painter->pen() );

	return path;
}

CCurveLineRenderer::CCurveLineRenderer()
{
}

QPainterPath CCurveLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	qreal			deg = 5;
	QPointF			p1, p2, t;
	QList<QPointF>	ps;
	QPainterPath	stroke;

	if (!painter || points.length() < 2)
		return stroke;

	p1 = points.first();
	p2 = points.last();
	if (points.length() > 3 && points[1].x() == p1.x())
		t = QPointF(p2.x(), p1.y());
	else
		t = QPointF(p1.x(), p2.y());

	ps << p1 << t << p2;

	stroke.moveTo( p1 );
	for (int i = 1; i < ps.length() - 1; ++i)
	{
		if (i == ps.length() - 2)
			t = p2;
		else
			t = (ps[i] + ps[i+1]) / 2.0;
		stroke.quadTo(ps[i], t);
	}
	painter->strokePath(stroke, painter->pen());

	return stroke;
}

CRoundedLineRenderer::CRoundedLineRenderer()
{
}

QPainterPath CRoundedLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	qreal			radius = 6;
	qreal			deg, mark;
	QPointF			p1, p2, t1, t2, prev;
	QVector2D		v1, v2;
	QPainterPath	stroke;
	QList<QPointF>	ps;

	if (!painter || points.length() < 2)
		return stroke;
	
	for (int i = points.length() - 1; i >= 0; --i)
		ps.append(points.at(i));

	p1 = ps.first();
	p2 = ps.last();

	stroke.moveTo( p1 );
	prev = p1;
	mark = radius;
	for (int i = 1; i < ps.length() - 1; ++i)
	{
		v1 = QVector2D(ps[i-1] - ps[i]);
		v2 = QVector2D(ps[i+1] - ps[i]);
		deg = qMin(radius, qMax(qAbs(ps[i].x() - ps[i+1].x()), qAbs(ps[i].y() - ps[i+1].y()) ) / 2.0 );
		v1.normalize();
		v2.normalize();
		if (deg <= mark)
		{
			t1 = ps[i] + (v1 * deg).toPointF();
		}
		else
		{
			t1 = prev;
			deg = mark;
		}		
		t2 = ps[i] + (v2 * deg).toPointF();
		stroke.lineTo(t1);
		stroke.quadTo(ps[i], t2);
		prev = t2;
		mark = deg;

		if (ps.length() == 3)
			break;
		else
			if (i == ps.length() - 3)
			{
				stroke.lineTo(ps[i+1]);
				break;
			}
	}

	stroke.lineTo( p2 );
	painter->strokePath(stroke, painter->pen());

	return stroke;
}

CAngledLineRenderer::CAngledLineRenderer()
{
}

QPainterPath CAngledLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	qreal			deg = 5;
	qreal			width = 6, length = 8;
	QPen			pen;
	QPointF			p1, p2, t;
	QPointF			t1, t2;
	QVector2D		v1, v2;
	QPainterPath	stroke;

	if (!painter || points.length() < 2)
		return stroke;

	p1 = points.first();
	p2 = points.last();

	stroke.moveTo( p1 );
	for (int i = 1; i < points.length() - 1; ++i)
	{
		if (i == points.length() - 2)
			t = p2;
		else
			t = (points[i] + points[i+1]) / 2.0;
		stroke.quadTo(points[i], t);
	}

	// get the arrow
	p1 = stroke.pointAtPercent(0.9);
	p2 = points.last();
	pen = painter->pen();

	v1 = QVector2D( (p2 - p1) ).normalized() * length;
	v2 = v1;
	t1 = p2 - v1.toPointF();
	t2 = p2;

	v1 = QVector2D( t2.y() - t1.y(), t1.x() - t2.x() ).normalized() * ((width + pen.widthF()) / 2.0);

	stroke.moveTo( t1 + v1.toPointF() );
	stroke.lineTo(t2);
	stroke.lineTo( t1 - v1.toPointF() );

	painter->strokePath(stroke, painter->pen());

	return stroke;
}

CPolygonLineRenderer::CPolygonLineRenderer()
{
}

QPainterPath CPolygonLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	QPointF			p1, p2;
	QPainterPath	stroke;

	if (!painter || points.length() < 2)
		return stroke;

	p1 = points.first();
	p2 = points.last();

	stroke.moveTo( p1 );
	for (int i = 1; i < points.length() - 1; ++i)
	{
		stroke.lineTo( points[i] );
	}
	stroke.lineTo( p2 );

	painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
	painter->strokePath(stroke, painter->pen());
	painter->restore();

	return stroke;
}

CCartoonLineRenderer::CCartoonLineRenderer()
{
}

QPainterPath CCartoonLineRenderer::draw(QPainter *painter, const QList<QPointF> &points)
{
	return QPainterPath();
}
