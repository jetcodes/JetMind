/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramEdgeStyle>

CDiagramEdgeStyle::CDiagramEdgeStyle(const QString &ctg, const QString &name, const QIcon &icon)
	: m_category(ctg),
	  m_name(name),
	  m_icon(icon)
{
	m_ctrlLength = 10;
}

void CDiagramEdgeStyle::route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const
{
	QPointF		c1 = p1, c2 = p2;
	qreal		r1, r2, dx, dy;
	qreal		delta, alpha;
	int			s1 = d1;
	int			directionVector[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
	int			s2 = d2;
	int			nextDirection[4][2] = {{1, 3}, {0, 2}, {1, 3}, {0, 2}};
	int			index = 0;

	if (d1 < 0 || d2 < 0)
		return;

	dx = p2.x() - p1.x();
	dy = p2.y() - p1.y();
	r1 = directionVector[s1][0] * dx + directionVector[s1][1] * dy;
	r2 = directionVector[s2][0] * (-dx) + directionVector[s2][1] * (-dy);

	 // c1 is in front of c2 in the direction of s1
	if (r1 < 0)
	{
		points.push_back(c1);
		index = ((directionVector[s1][0] == 0 ? dx: dy) < 0 ? 0 : 1);
		c1 = c1 + QPointF(directionVector[s1][0] * m_ctrlLength,  directionVector[s1][1] * m_ctrlLength);
		s1 = nextDirection[s1][index];

		dx = c2.x() - c1.x();
		dy = c2.y() - c1.y();
		r1 = directionVector[s1][0] * dx + directionVector[s1][1] * dy;
		r2 = directionVector[s2][0] * (-dx) + directionVector[s2][1] * (-dy);
	}

	// process the case that the c2 is in front of c1 in the direction
	// of s1
	if (s1 == s2) // same direction
	{
		delta = (directionVector[s1][0] == 0 ? qAbs(dy) : qAbs(dx)) + m_ctrlLength;
		points.push_back(c1);
		points.push_back(c1 + QPointF(directionVector[s1][0] * delta,  directionVector[s1][1] * delta));
		points.push_back(c2 + QPointF(directionVector[s2][0] * m_ctrlLength,  directionVector[s2][1] * m_ctrlLength));
		points.push_back(c2);
	}
	else if ( (s1 + s2) % 2 == 0) // reversed direction
	{
		delta = (directionVector[s1][0] == 0 ? qAbs(dy) : qAbs(dx)) / 2.0;
		points.push_back(c1);
		points.push_back(c1 + QPointF(directionVector[s1][0] * delta,  directionVector[s1][1] * delta));
		points.push_back(c2 + QPointF(directionVector[s2][0] * delta,  directionVector[s2][1] * delta));
		points.push_back(c2);
	}
	else
	{
		if (r2 > 0)
		{
			delta = (directionVector[s1][0] == 0 ? qAbs(dy) : qAbs(dx));
			points.push_back(c1);
			points.push_back(c1 + QPointF(directionVector[s1][0] * delta,  directionVector[s1][1] * delta) );
			points.push_back(c2);
		}
		else
		{
			delta = ( directionVector[s1][0] == 0 ? qAbs(dy) : qAbs(dx)) / 2.0;
			alpha = ( directionVector[s1][0] == 0 ? qAbs(dx) : qAbs(dy)) + m_ctrlLength;
			index = ((directionVector[s1][0] == 0 ? dx: dy) <= 0 ? 0 : 1);
			points.push_back(c1);
			points.push_back(c1 + QPointF(directionVector[s1][0] * delta,  directionVector[s1][1] * delta) );
			points.push_back(points.last() + QPointF(directionVector[nextDirection[s1][index]][0] * alpha,  directionVector[nextDirection[s1][index]][1] * alpha) );
			points.push_back(c2 + QPointF(directionVector[s2][0] * m_ctrlLength,  directionVector[s2][1] * m_ctrlLength) );
			points.push_back(c2);
		}
	}
}

void CDiagramEdgeStyle::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
}