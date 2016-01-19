/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>

#include "CMindMapNode.h"
#include "CMindMapNodeRenderer.h"
#include "CMindMapPathRenderer.h"

typedef void (*pf1)(QSizeF&, qreal&, qreal&, qreal, qreal, int);
typedef void (*pf2)(QSizeF&, qreal&, qreal&, qreal, qreal);

#define COLLAPSER_DISTANCE 8

// calculation method for size of organization/logic layout
// style
void calc_func_1(QSizeF &size, qreal &w, qreal &h, qreal xp, qreal yp, int i)
{
	h += size.height();
	if (i < 0)
		h += yp;
	w = qMax(w, size.width() );
}

void calc_func_2(QSizeF &size, qreal &w, qreal &h, qreal xp, qreal yp, int i)
{
	w += size.width();
	if (i < 0)
		w += xp;
	h = qMax(h, size.height());
}

void calc_func_3(QSizeF &size, qreal &w, qreal &h, qreal xp, qreal yp)
{
	h = qMax(size.height(), h);
	if (w > 0)
		w += xp;
	w += size.width();
}

void calc_func_4(QSizeF &size, qreal &w, qreal &h, qreal xp, qreal yp)
{
	w = qMax(size.width(), w);
	if (h > 0)
		h += yp;
	h += size.height();
}

QSizeF calc_logic_size(CMindMapNode *node, qreal xp, qreal yp, pf1 func1, pf2 func2)
{
	QSizeF					size;
	CMindMapNode			*child = NULL;
	QList<CMindMapNode*>	childs;
	qreal					w = 0, h = 0;

	if (!node)
		return size;

	childs = node->childs();
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		size = child->areaSize();
		func1(size, w, h, xp, yp, i - childs.length() + 1);
	}

	size = node->contentRect().size();
	func2(size, w, h, xp, yp);

	return QSizeF(w, h);
}

QPointF calc_intersect_point(const QPainterPath &path, const QPointF &p1, const QPointF &p2)
{
	QPointF		t1, t2;
	QLineF		radius(p1, p2);
	qreal		prev = 0.0, next = 1.0, percent = 0.5;

	percent = 0.5;
	t2 = p2;
	while (true)
	{
		t1 = radius.pointAt(percent);
		if (path.contains(t1))
		{
			prev = percent;
			percent = (next + percent) / 2.0;
		}
		else
		{
			next = percent;
			percent = (prev + percent) / 2.0;
		}

		if ( (t2 - t1).manhattanLength() < 0.5)
		{
			t2 = t1;
			break;
		}
		t2 = t1;
	}
	return t2;
}

//////////////////////////////////////////////////////////////
// classes definition
// Mind map layout class definition
CMindMapRenderer::CMindMapRenderer(CMindMapRenderer::Direction d)
	: m_xSpacing(128),
      m_ySpacing(64),
	  m_d(d)
{
}

void CMindMapRenderer::setDirection(CMindMapRenderer::Direction d)
{
	m_d = d;
}

CMindMapRenderer::Direction CMindMapRenderer::direction() const
{
	return m_d;
}

QSizeF CMindMapRenderer::recalc(CMindMapNode *node)
{
	qreal					w = 0, h = 0;
	QSizeF					size;
	QList<CMindMapNode*>	childs;
	
	if (!node)
		return size;

	childs = node->childs();
	foreach (CMindMapNode *n, childs)
	{
		size = n->areaSize();
		h += size.height();
		w = qMax(w, size.width());
	}
	
	return QSizeF(2*w, h);
}

void CMindMapRenderer::layout(CMindMapNode *node)
{
    int								markNodeIndex = -1, markRegionIndex = -1;
    qreal							markNodeHeight = 0;
    QPointF							nodePosition;
    QRectF							nodeContentRect, childContentRect, areaRect;
    QSizeF							nodeAreaSize, childAreaSize;
    qreal							totalHeight = 0, halfHeight = 0;
    qreal							currTotalHeight = 0, firstPartHeight = 0, secondPartHeight = 0;
	qreal							a1, a2, b1, b2;
	qreal							x = 0, y = 0, prevY = 0;
	qreal							delta = 5.0;
	qreal							xspacing, yspacing;
	CMindMapNode					*child = NULL;
	CMindMapNodeRenderer			*renderer = NULL;
    CMindMapNode::RelativePositions	pos;
    QList<CMindMapNode*>			nodeChildNodes;
    QList<QList<int> >              nodeRegions;
    QList<qreal>                    nodeRegionsHeight;

	if (!node)
		return;
	
    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	// initialization
    nodePosition = node->pos();
    nodeContentRect = node->contentRect();
    nodeAreaSize = node->areaSize();
    nodeChildNodes = node->childs();

    // loop over all the regions, calculate
    // the total height of the regions
	totalHeight = 0;
    nodeRegions = node->nodeRegions();
    for (int i = 0; i < nodeRegions.length(); ++i)
    {
        qreal           regionHeight = 0;
        QList<int>      region;
        CMindMapNode    *n = NULL;

		region = nodeRegions.at(i);
        for (int j = 0; j < region.length(); ++j)
        {
            if (region.at(j) < nodeChildNodes.length() )
            {
                n = nodeChildNodes.at( region.at(j) );
                regionHeight += n->areaSize().height();
            }
        }
		regionHeight += yspacing * (region.length() - 1);
        nodeRegionsHeight.push_back(regionHeight);
        totalHeight += regionHeight;
    }
    halfHeight = totalHeight / 2.0;

	// determine where should be the splitted point,
	// find out the index
	currTotalHeight = 0;
    for (int i = 0; i < nodeRegions.length(); ++i)
    {
        qreal   delta1, delta2;

        currTotalHeight += nodeRegionsHeight.at(i);
        delta1 = currTotalHeight - halfHeight;
        delta2 = ( i < nodeRegions.length() - 1) ? delta1 + nodeRegionsHeight.at( i + 1) : 0;

        markNodeIndex = nodeRegions.at(i).last();
        markNodeHeight = currTotalHeight;
        markRegionIndex = i;

        if (delta1 >= 0 || (delta1 < 0 && delta2 >= 0))
        {
            if ((qAbs(delta1) > qAbs(delta2)) && (i < nodeRegions.length() - 1) )
            {
                markNodeIndex = nodeRegions.at( i+1 ).last();
                markRegionIndex = i + 1;
                markNodeHeight = currTotalHeight + nodeRegionsHeight.at(i+1);
            }
			break;
        }
    }

    // calculate the parameters for ellipse
    // style layout
    firstPartHeight = markNodeHeight + markRegionIndex * yspacing;
    secondPartHeight = (totalHeight - markNodeHeight) + (nodeRegions.length() - (markRegionIndex + 1) -1) * yspacing;

    a1 = qPow(qMax(firstPartHeight, secondPartHeight) / 2.0, 2.0);
    b1 = qPow(nodeContentRect.width() / 2.0 + xspacing, 2.0);
	
    a2 = qPow(qMax(secondPartHeight, firstPartHeight) / 2.0, 2.0);
    b2 = qPow(nodeContentRect.width() / 2.0 + xspacing, 2.0);
	
    currTotalHeight = 0;
	if (m_d == CMindMapRenderer::Clockwise)
	{
        for (int i = 0; i < nodeChildNodes.length(); ++i)
		{
            child = nodeChildNodes.at(i);
            childContentRect = child->contentRect();
            childAreaSize = child->areaSize();
            if (i <= markNodeIndex)
			{
                y = firstPartHeight / 2.0 - currTotalHeight - childAreaSize.height() / 2.0;
				delta = b1 - b1* qPow(y, 2.0)/a1;
				if (delta >= 0)
                    x = nodePosition.x() + qSqrt( delta );
				else
                    x = nodePosition.x() + qSqrt( b1 - b1* qPow((-firstPartHeight / 2.0 + prevY) / 2.0, 2.0)/a1 );
				prevY = y;
                y = nodePosition.y() - firstPartHeight / 2.0 + currTotalHeight;

				pos = CMindMapNode::East;
			}
			else
			{
                y = - secondPartHeight / 2.0 + currTotalHeight + childAreaSize.height() / 2.0;
				delta = b2 - b2 * qPow(y, 2.0 )/a2;
				if (delta >= 0)
                    x = nodePosition.x() - qSqrt( delta ) - childAreaSize.width();
				else
                    x = nodePosition.x() - nodeContentRect.width() / 2.0 - xspacing - childAreaSize.width();
                y = nodePosition.y() + secondPartHeight / 2.0 - currTotalHeight - childAreaSize.height();

				pos = CMindMapNode::West;
			}

            areaRect = QRectF(QPointF(x, y), childAreaSize);
			child->setRelativePosition(pos);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, pos);
            currTotalHeight = (i == markNodeIndex) ? 0 : currTotalHeight + childAreaSize.height() + yspacing;
		}
	}
	else if (m_d == CMindMapRenderer::AntiClockwise) // anticlockwise layout
	{
        for (int i = 0; i < nodeChildNodes.length(); ++i)
		{
            child = nodeChildNodes.at(i);
            childContentRect = child->contentRect();
            childAreaSize = child->areaSize();
            if (i <= markNodeIndex)
			{
                y = firstPartHeight / 2.0 - currTotalHeight - childAreaSize.height() / 2.0;
				delta = b1 - b1 * qPow(y, 2.0 )/a1;
				if (delta >= 0)
                    x = nodePosition.x() - qSqrt( delta ) - childAreaSize.width();
				else
                    x = nodePosition.x() - nodeContentRect.width() / 2.0 - xspacing - childAreaSize.width();
                y = nodePosition.y() - firstPartHeight / 2.0 + currTotalHeight;

				pos = CMindMapNode::West;
			}
			else
			{
                y = -secondPartHeight / 2.0 + currTotalHeight + childAreaSize.height() / 2.0;
				delta = b2 - b2* qPow(y, 2.0)/a2;
				if (delta >= 0)
                    x = nodePosition.x() + qSqrt( delta );
				else
                    x = nodePosition.x() + qSqrt( b2 - b2* qPow((-secondPartHeight / 2.0 + prevY) / 2.0, 2.0)/a2 );
				prevY = y;
                y = nodePosition.y() + secondPartHeight / 2.0 - currTotalHeight - childAreaSize.height();

				pos = CMindMapNode::East;
			}

            areaRect = QRectF(QPointF(x, y), childAreaSize);
			child->setRelativePosition(pos);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, pos );
            currTotalHeight = (i == markNodeIndex) ? 0 : currTotalHeight + childAreaSize.height() + yspacing;
		}
	}
	else
	{
        for (int i = 0; i < nodeChildNodes.length(); ++i)
		{
            child = nodeChildNodes.at(i);
            childContentRect = child->contentRect();
            childAreaSize = child->areaSize();
            if (i <= markNodeIndex)
			{
                y = firstPartHeight / 2.0 - currTotalHeight - childAreaSize.height() / 2.0;
				delta = b1 - b1 * qPow(y, 2.0 )/a1;
				if (delta >= 0)
                    x = nodePosition.x() - qSqrt( delta ) - childAreaSize.width();
				else
                    x = nodePosition.x() - nodeContentRect.width() / 2.0 - xspacing - childAreaSize.width();
                y = nodePosition.y() - firstPartHeight / 2.0 + currTotalHeight;
				pos = CMindMapNode::West;
			}
			else
			{
                y = secondPartHeight / 2.0 - currTotalHeight - childAreaSize.height() / 2.0;
				delta = b2 - b2* qPow(y, 2.0)/a2;
				if (delta >= 0)
                    x = nodePosition.x() + qSqrt( delta );
				else
                    x = nodePosition.x() + qSqrt( b2 - b2* qPow((-secondPartHeight / 2.0 + prevY) / 2.0, 2.0)/a2 );
				prevY = y;
                y = nodePosition.y() - secondPartHeight / 2.0 + currTotalHeight;

				pos = CMindMapNode::East;
			}

            areaRect = QRectF(QPointF(x, y), childAreaSize);
			child->setRelativePosition(pos);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, pos );
            currTotalHeight = (i == markNodeIndex) ? 0 : currTotalHeight + childAreaSize.height() + yspacing;
		}
	}
}

QPainterPath CMindMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	QPointF					p1, p2;
	QRectF					r1, r2;
	CMindMapNode			*child = NULL;
	QList<CMindMapNode*>	childs;
	QList<QPointF>			points;
	QPainterPath			path;

	if (!node || !painter)
		return path;

	path.setFillRule(Qt::WindingFill);

	r1 = node->contentRect();

	// determine the incoming connection point
	childs = node->childs();
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		r2 = child->contentRect();
		if (child->x() > node->x())
        {
			if (child->nodeType() == CMindMapNode::Underline)
				p2 = child->mapToItem(node, r2.bottomLeft());
			else
                p2 = child->mapToItem(node, QPointF(r2.left(), 0));
		}
		else
		{
            if (child->nodeType() == CMindMapNode::Underline)
				p2 = child->mapToItem(node, r2.bottomRight());
			else
                p2 = child->mapToItem(node, QPointF(r2.right(), 0));
		}

		p1 = calc_intersect_point(node->nodeShape(), r1.center(), p2);
		if (pathRenderer)
		{
			points << p1 << QPointF(p1.x(), p2.y()) << p2;
			path.addPath( pathRenderer->draw(painter, points) );
			points.clear();
		}
	}

	return path;
}

void CMindMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	// EMPTY
}

QPainterPath CMindMapRenderer::connectArea(CMindMapNode *node)
{
    bool                 mark = false;
    qreal                h, delta = 64;
    qreal                xspacing = 0, yspacing = 0;
    QRectF               r, prev;
    QPointF              p;
    QPainterPath         path;
    QList<CMindMapNode*> childs;

    if (node)
    {
        h = node->areaSize().height() / 2.0 + delta;
        xspacing = xSpacing(node);
        yspacing = ySpacing(node);
        childs = node->childs();
        if (childs.length() < 2)
        {
            r = node->boundingRect().adjusted(-xspacing, -yspacing, xspacing, yspacing);
            path.addEllipse( r );
        }
        else
        {
            switch(m_d)
            {
            case CMindMapRenderer::AntiClockwise:
                foreach (CMindMapNode *n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect());
                    p = node->mapFromScene(n->pos());

                    if (path.isEmpty())
                    {
                        path.moveTo( 0, -h );
                        path.lineTo( r.topLeft().x(), -h );
                    }

                    if (!mark && (p.x() > 0))
                    {
                        path.lineTo( prev.bottomLeft().x(), h);
                        path.lineTo( r.bottomRight().x(), h );
                        path.lineTo( r.bottomRight() );
                        path.lineTo(r.topRight());
                        mark = true;
                    }
                    else
                    {
                        if (p.x() < 0)
                        {
                            path.lineTo(r.topLeft());
                            path.lineTo(r.bottomLeft());
                        }
                        else
                        {
                            path.lineTo(r.bottomRight());
                            path.lineTo(r.topRight());
                        }
                    }
                    prev = r;
                }

                if (childs.length() > 1)
                    path.lineTo(r.topRight().x(), -h);

                path.closeSubpath();
                break;

            case CMindMapRenderer::Clockwise:
                foreach (CMindMapNode *n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect());
                    p = node->mapFromScene(n->pos());
                    if (path.isEmpty())
                    {
                        path.moveTo( 0, -h );
                        path.lineTo( r.topRight().x(), -h );
                    }

                    if (!mark && (p.x() < 0))
                    {
                        path.lineTo( prev.bottomRight().x(), h);
                        path.lineTo( r.bottomLeft().x(), h );
                        path.lineTo( r.bottomLeft() );
                        path.lineTo( r.topLeft() );
                        mark = true;
                    }
                    else
                    {
                        if (p.x() < 0)
                        {
                            path.lineTo(r.bottomLeft());
                            path.lineTo(r.topLeft());
                        }
                        else
                        {
                            path.lineTo(r.topRight());
                            path.lineTo(r.bottomRight());
                        }
                    }
                    prev = r;
                }

                if (childs.length() > 1)
                    path.lineTo(r.topLeft().x(), -h);

                path.closeSubpath();

                break;

            case CMindMapRenderer::DoubleFlow:
            {
                int         i = 0;
                int         j = 0;

                for (i = 0; i < childs.length(); ++i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p = node->mapFromScene(n->pos());
                    if (p.x() > 0)
                        break;
                    r = node->mapRectFromItem(n, n->contentRect());

                    if (path.isEmpty())
                    {
                        path.moveTo( 0, -h );
                        path.lineTo( r.topLeft().x(), -h );
                    }

                    path.lineTo(r.topLeft());
                    path.lineTo(r.bottomLeft());

                    prev = r;
                }

                for (j = childs.length() - 1; j >= i; --j)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(j);
                    r = node->mapRectFromItem(n, n->contentRect());
                    p = node->mapFromScene(n->pos());
                    if (!mark)
                    {
                        path.lineTo( prev.bottomLeft().x(), h);
                        path.lineTo( r.bottomRight().x(), h );
                        path.lineTo( r.bottomRight() );
                        path.lineTo( r.topRight() );
                        mark = true;
                    }
                    else
                    {
                        path.lineTo( r.bottomRight() );
                        path.lineTo( r.topRight() );
                    }
                    prev = r;
                }

                if (childs.length() > 1)
                    path.lineTo(r.topRight().x(), -h);
                path.closeSubpath();

                break;
            }
            default:
                break;

            }; // End of switch
        }
    }

    return path;
}

int CMindMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    int                     index = -1;
    int                     mark = -1;
    QPointF                 p1, p2;
    QList<CMindMapNode*>    childs;

    if (node)
    {
        childs = node->childs();
        p1 = node->mapFromScene(p);
        switch(m_d)
        {
        case CMindMapRenderer::Clockwise:
            p1 = node->mapFromScene(p);
            if (p1.x() > 0)
            {
                for (int i = 0; i < childs.length(); ++i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y())
                    {
                        index = i;
                        break;
                    }

                    if (p2.x() < 0)
                    {
                        mark = i;
                        break;
                    }
                }
            }
            else
            {
                for (int i = childs.length() - 1; i >= 0; --i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y() )
                    {
                        index = i + 1;
                        break;
                    }

                    if (p2.x() > 0)
                    {
                        mark = i + 1;
                        break;
                    }
                }
            }
            break;

        case CMindMapRenderer::AntiClockwise:
            p1 = node->mapFromScene(p);
            if (p1.x() < 0)
            {
                for (int i = 0; i < childs.length(); ++i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y())
                    {
                        index = i;
                        break;
                    }

                    if (p2.x() > 0)
                    {
                        mark = i;
                        break;
                    }
                }
            }
            else
            {
                for (int i = childs.length() - 1; i >= 0; --i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y() )
                    {
                        index = i + 1;
                        break;
                    }

                    if (p2.x() < 0)
                    {
                        mark = i + 1;
                        break;
                    }
                }
            }
            break;

        case CMindMapRenderer::DoubleFlow:
            p1 = node->mapFromScene(p);
            if (p1.x() < 0)
            {
                for (int i = 0; i < childs.length(); ++i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y())
                    {
                        index = i;
                        break;
                    }

                    if (p2.x() > 0)
                    {
                        mark = i;
                        break;
                    }
                }
            }
            else
            {
                for (int i = childs.length() - 1; i >= 0; --i)
                {
                    CMindMapNode    *n = NULL;

                    n = childs.at(i);
                    p2 = node->mapFromScene(n->pos());
                    if (p1.y() < p2.y() )
                    {
                        index = i + 1;
                        break;
                    }

                    if (p2.x() < 0)
                    {
                        mark = i + 1;
                        break;
                    }
                }
            }
            break;

        default:
            break;
        };
    }

    if (index == -1)
        index = mark;

    return index;
}

bool CMindMapRenderer::rootOnly()
{
	return true;
}

bool CMindMapRenderer::collapsible()
{
	return false;
}

qreal CMindMapRenderer::xSpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_xSpacing;
        else if (level == 1)
            r = m_xSpacing / 2.0;
        else
            r = m_xSpacing / 3.0;
    }

    return r;
}

qreal CMindMapRenderer::ySpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_ySpacing;
        else if (level == 1)
            r = m_ySpacing / 4.0;
        else
            r = m_ySpacing / 4.0;
    }

    return r;
}

////////////////////////////////
// Logic map layout definition
CLogicMapRenderer::CLogicMapRenderer(CLogicMapRenderer::Direction d)
	: m_xSpacing(64),
      m_ySpacing(64),
	  m_d(d)
{

}

void CLogicMapRenderer::setDirection(CLogicMapRenderer::Direction d)
{
	m_d = d;
}

CLogicMapRenderer::Direction CLogicMapRenderer::direction() const
{
	return m_d;
}

QSizeF CLogicMapRenderer::recalc(CMindMapNode *node)
{
	QSizeF		r;
    qreal       xspacing = 0, yspacing = 0;
	
	if (!node)
		return r;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

    r = calc_logic_size(node, xspacing, yspacing, calc_func_1, calc_func_3);

    return r;
}

void CLogicMapRenderer::layout(CMindMapNode *node)
{
	QPointF					point;
    QRectF					rect, areaRect;
	QSizeF					size, bs;
	CMindMapNode			*child = NULL;
	CMindMapNodeRenderer	*renderer = NULL;
	QList<CMindMapNode*>	childs;
	qreal					h = 0;
	qreal					xspacing, yspacing;
	QGraphicsItem			*collapser = NULL;

	if (!node)
		return;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	collapser = node->collapser();
	point = node->pos();
	rect = node->contentRect();
	size = node->areaSize();
	childs = node->childs();

	// place the collapser
	if (collapser)
	{
		if (m_d == CLogicMapRenderer::Rightwards)
		{
			if (node->nodeType() == CMindMapNode::Underline)
				collapser->setPos(rect.right() + COLLAPSER_DISTANCE, rect.bottom());
			else
				collapser->setPos(rect.right() + COLLAPSER_DISTANCE, rect.center().y());
		}
		else
		{
			if (node->nodeType() == CMindMapNode::Underline)
				collapser->setPos(rect.left() - COLLAPSER_DISTANCE, rect.bottom());
			else
				collapser->setPos(rect.left() - COLLAPSER_DISTANCE, rect.center().y());
		}
	}

    // calculate the bounds of the childs
    for (int i = 0; i < childs.length(); ++i)
    {
        child = childs.at(i);
        bs = child->areaSize();
        h += (bs.height() + yspacing);
    }
    h -= yspacing;
    if (h < size.height())
        size.setHeight(h);
    h = 0;

	// place the childs
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		bs = child->areaSize();
		if (m_d == CLogicMapRenderer::Rightwards)
        {
            if (node->nodeType() == CMindMapNode::Underline)
            {
                areaRect = QRectF(QPointF(point.x() + rect.width() / 2.0 + xspacing, point.y() - size.height() / 2.0 + h + bs.height() / 2.0), bs);
            }
            else
            {
                areaRect = QRectF(QPointF(point.x() + rect.width() / 2.0 + xspacing, point.y() - size.height() / 2.0 + h), bs);
            }
            child->setRelativePosition(CMindMapNode::East);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::East);
		}
		else
		{
            if (node->nodeType() == CMindMapNode::Underline)
            {
                areaRect = QRectF(QPointF(point.x() - rect.width() / 2.0 - xspacing - bs.width(), point.y() - size.height() / 2.0 + h + bs.height() / 2.0), bs);
            }
            else
            {
                areaRect = QRectF(QPointF(point.x() - rect.width() / 2.0 - xspacing - bs.width(), point.y() - size.height() / 2.0 + h), bs);
            }
            child->setRelativePosition(CMindMapNode::West);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::West);
		}
		h += (bs.height() + yspacing);
	}
}

QPainterPath CLogicMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	QPointF					p1, p2;
	QRectF					r1, r2;
	CMindMapNode			*child = NULL;
	QList<CMindMapNode*>	childs;
	QList<QPointF>			points;
	QPainterPath			path;
	qreal					delta = 4;
	qreal					xspacing, yspacing;

	if (!node || !painter)
		return path;

	delta =  (node->isRoot() ? 0 : delta);
    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	path.setFillRule(Qt::WindingFill);

	// should use content's rect here instead of contentRect
	r1 = node->contentRect();

	// determine the outcoming connection point
	if (node->nodeType() == CMindMapNode::Underline)
	{
		if (m_d == CLogicMapRenderer::Rightwards)
			p1 = r1.bottomRight() + QPointF(delta, 0);
		else
			p1 = r1.bottomLeft() - QPointF(delta, 0);
	}
	else
	{
		if (m_d == CLogicMapRenderer::Rightwards)
			p1 = r1.center() + QPointF(r1.width() / 2.0 + delta, 0);
		else
			p1 = r1.center() - QPointF(r1.width() / 2.0 + delta, 0);
	}

	// determine the incoming connection point
	childs = node->childs();
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		r2 = child->contentRect();
		if (child->nodeType() == CMindMapNode::Underline)
			if (m_d == CLogicMapRenderer::Rightwards)
				p2 = child->mapToItem(node, r2.bottomLeft());
			else
				p2 = child->mapToItem(node, r2.bottomRight());
		else
			if (m_d == CLogicMapRenderer::Rightwards)
				p2 = child->mapToItem(node, QPointF(r2.left(), 0));
			else
				p2 = child->mapToItem(node, QPointF(r2.right(), 0));

		if (pathRenderer)
		{
			if (m_d == CLogicMapRenderer::Rightwards)
				points << p1 << QPointF(p1.x() + xspacing / 2.0, p1.y()) << QPointF( p1.x() + xspacing / 2.0, p2.y()) << p2;
			else
				points << p1 << QPointF(p1.x() - xspacing / 2.0, p1.y()) << QPointF( p1.x() - xspacing / 2.0, p2.y()) << p2;
			path.addPath( pathRenderer->draw(painter, points) );
			points.clear();
		}
	}

	return path;
}

void CLogicMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	QRectF		r;
	QPointF		cp;

	if (!node)
		return;

	r = node->contentRect();
	cp = rect.center();
	if (m_d == CLogicMapRenderer::Rightwards)
	{
        if (node->nodeType() == CMindMapNode::Underline)
            node->setPos( rect.left() + r.width() / 2.0, cp.y() - r.height() / 2.0 );
        else
		    node->setPos( rect.left() + r.width() / 2.0, cp.y() );
	}
	else
	{
        if (node->nodeType() == CMindMapNode::Underline)
            node->setPos( rect.right() - r.width() / 2.0, cp.y() - r.height() / 2.0);
        else
		    node->setPos( rect.right() - r.width() / 2.0, cp.y() );
	}
}

QPainterPath CLogicMapRenderer::connectArea(CMindMapNode *node)
{
    qreal                   w, h;
    qreal                   delta = 180;
    qreal                   xspacing, yspacing;
    QRectF                  r;
    QPainterPath            path;
    CMindMapNode            *n = NULL;
    QList<CMindMapNode*>    childs;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

    if (node)
    {
        h = node->areaSize().height() / 2.0 + yspacing;
        w = node->boundingRect().width() / 2.0;
        childs = node->childs();
        if (m_d == CLogicMapRenderer::Rightwards)
        {
            path.moveTo( 0, 0 );
            path.lineTo( 0, -h );

            if (childs.length() < 2)
            {
                path.lineTo( w + delta, -h);
                path.lineTo( w + delta, h);
            }
            else
            {
                n = childs.first();
                r = node->mapRectFromItem(n, n->contentRect() );
                path.lineTo( r.topRight().x(), -h );
                foreach (n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect());
                    path.lineTo( r.topRight() );
                    path.lineTo( r.bottomRight() );
                }

                n = childs.last();
                r = node->mapRectFromItem(n, n->contentRect() );
                path.lineTo( r.bottomRight().x(), h);
            }

            path.lineTo(0, h);
            path.closeSubpath();
        }
        else
        {
            path.moveTo( 0, 0 );
            path.lineTo( 0, -h );

            if (childs.length() < 2)
            {
                path.lineTo( -w - delta, -h);
                path.lineTo( -w - delta, h);
            }
            else
            {
                n = childs.first();
                r = node->mapRectFromItem(n, n->contentRect() );
                path.lineTo( r.topLeft().x(), -h );
                foreach (CMindMapNode *n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect());
                    path.lineTo( r.topLeft() );
                    path.lineTo( r.bottomLeft() );
                }

                n = childs.last();
                r = node->mapRectFromItem(n, n->contentRect() );
                path.lineTo( r.bottomLeft().x(), h);
            }

            path.lineTo(0, h);
            path.closeSubpath();
        }
    }

    return path;
}

int CLogicMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    int                     index = -1;
    QPointF                 p1, p2;
    QList<CMindMapNode*>    childs;

    if (node)
    {
        p1 = node->mapFromScene(p);
        childs = node->childs();
        for (int i = 0; i < childs.length(); ++i)
        {
            CMindMapNode    *n = NULL;

            n = childs.at(i);
            p2 = node->mapFromScene(n->pos());
            if (p2.y() > p1.y())
            {
                index = i;
                break;
            }
        }
    }

    return index;
}

bool CLogicMapRenderer::rootOnly()
{
	return false;
}

bool CLogicMapRenderer::collapsible()
{
	return true;
}

qreal CLogicMapRenderer::xSpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_xSpacing;
        else if (level == 1)
            r = m_xSpacing / 2.0;
        else
            r = m_xSpacing / 3.0;
    }

    return r;
}

qreal CLogicMapRenderer::ySpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_ySpacing;
        else if (level == 1)
            r = m_ySpacing / 2.0;
        else
            r = m_ySpacing / 4.0;
    }

    return r;
}

//////////////////////////
//organization map layout
COrganizationMapRenderer::COrganizationMapRenderer(COrganizationMapRenderer::Direction d)
    : m_xSpacing(64),
      m_ySpacing(78),
	  m_d(d)
{
}

void COrganizationMapRenderer::setDirection(COrganizationMapRenderer::Direction d)
{
	m_d = d;
}

COrganizationMapRenderer::Direction COrganizationMapRenderer::direction() const
{
	return m_d;
}

QSizeF COrganizationMapRenderer::recalc(CMindMapNode *node)
{
	QSizeF		r;
    qreal       xspacing = 0, yspacing = 0;

	if (!node)
		return r;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

    r = calc_logic_size(node, xspacing, yspacing, calc_func_2, calc_func_4);

	return r;
}

void COrganizationMapRenderer::layout(CMindMapNode *node)
{
	QPointF					point;
    QRectF					rect, br, areaRect;
	QSizeF					size, bs;
	CMindMapNode			*child = NULL;
	CMindMapNodeRenderer	*renderer = NULL;
	QList<CMindMapNode*>	childs;
	qreal					w = 0;
	qreal					xspacing, yspacing;
	QGraphicsItem			*collapser = NULL;

	if (!node)
		return;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	collapser = node->collapser();
	point = node->pos();
	rect = node->contentRect();
	size = node->areaSize();
	childs = node->childs();

	// place the collapser
	if (collapser)
	{
		if (m_d == COrganizationMapRenderer::Downwards)
			collapser->setPos(rect.center().x(), rect.center().y() + rect.height() / 2.0 + COLLAPSER_DISTANCE);
		else
			collapser->setPos(rect.center().x(), rect.center().y() - rect.height() / 2.0 - COLLAPSER_DISTANCE);
	}

    // calculate the bounds of the childs
    for (int i = 0; i < childs.length(); ++i)
    {
        child = childs.at(i);
        bs = child->areaSize();
        w += (bs.width() + xspacing);
    }
    w -= xspacing;
    if (w < size.width())
        size.setWidth(w);
    w = 0;


	// place the childs
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		br = child->contentRect();
		bs = child->areaSize();
		if (m_d == COrganizationMapRenderer::Downwards)
		{
            areaRect = QRectF(QPointF(point.x() - size.width() / 2.0 + w, point.y() + rect.height() / 2.0 + yspacing), bs);
			child->setRelativePosition(CMindMapNode::South);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::South);
		}
		else
		{
            areaRect = QRectF(QPointF(point.x() - size.width() / 2.0 + w, point.y() - rect.height() / 2.0 - yspacing - bs.height() ), bs);
			child->setRelativePosition(CMindMapNode::North);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::North);
		}
		w += (bs.width() + xspacing);
	}
}

QPainterPath COrganizationMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	QPointF					p1, p2;
	QRectF					r1, r2;
	CMindMapNode			*child = NULL;
	QList<CMindMapNode*>	childs;
	QList<QPointF>			points;
	QPainterPath			path;
	qreal					xspacing, yspacing;
	qreal					delta = 4;

	if (!node || !painter)
		return path;

	delta =  (node->isRoot() ? 0 : delta);
    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	path.setFillRule(Qt::WindingFill);

	// should use content's rect here instead of contentRect
	r1 = node->contentRect();
	if (m_d == COrganizationMapRenderer::Downwards)
		p1 = r1.center() + QPointF(0, r1.height() / 2.0 + delta);
	else
		p1 = r1.center() - QPointF(0, r1.height() / 2.0 + delta);

	// determine the incoming connection point
	childs = node->childs();
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		r2 = child->contentRect();
		if (m_d == COrganizationMapRenderer::Downwards)
		{
			p2 = child->mapToItem(node, QPointF(0, r2.top()));
			points << p1 << QPointF(p1.x(),p1.y() + yspacing / 2.0)  << QPointF(p2.x(), p1.y() + yspacing / 2.0) << p2;
		}
		else
		{
			p2 = child->mapToItem(node, QPointF(0, r2.bottom()));
			points << p1 << QPointF(p1.x(),p1.y() - yspacing / 2.0)  << QPointF(p2.x(), p1.y() - yspacing / 2.0) << p2;
		}

		if (pathRenderer)
			path.addPath( pathRenderer->draw(painter, points) );
		points.clear();
	}

	return path;
}

void COrganizationMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	QPointF		cp;

	cp = rect.center();
	if (m_d == COrganizationMapRenderer::Downwards)
	{
		node->setPos( cp.x(), rect.top() + node->contentRect().height() / 2.0 );
	}
	else
	{
		node->setPos( cp.x(), rect.bottom() - node->contentRect().height() / 2.0 );	
	}
}

QPainterPath COrganizationMapRenderer::connectArea(CMindMapNode *node)
{
    qreal                   delta = 132;
    qreal                   w, h;
    qreal                   xspacing, yspacing;
    QRectF                  r;
    QPainterPath            path;
    CMindMapNode            *n = NULL;
    QList<CMindMapNode*>    childs;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

    if (node)
    {
        w = node->boundingRect().width() / 2.0;
        h = node->areaSize().height() / 2.0;
        childs = node->childs();

        if (m_d == COrganizationMapRenderer::Downwards)
        {
            path.moveTo( 0, 0 );
            path.lineTo( -w, 0 );

            if (childs.length() < 2)
            {
                path.lineTo( -w, h + delta);
                path.lineTo( w, h + delta);
            }
            else
            {
                n = childs.first();
                r = node->mapRectFromItem(n, n->contentRect() );

                // start position
                path.lineTo(r.topLeft().x(), 0);
                path.lineTo(r.topLeft());

                // connect over all the child nodes
                foreach (n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect() );
                    path.lineTo( r.bottomLeft() );
                    path.lineTo( r.bottomRight() );
                }

                path.lineTo( r.topRight() );
                path.lineTo( r.topRight().x(), 0);
                path.closeSubpath();
            }
        }
        else
        {
            path.moveTo( 0, 0 );
            path.lineTo( -w, 0 );

            if (childs.length() < 2)
            {
                path.lineTo( -w, -h - delta);
                path.lineTo( w, -h - delta);
            }
            else
            {
                n = childs.first();
                r = node->mapRectFromItem(n, n->contentRect() );

                // start position
                path.lineTo(r.bottomLeft().x(), 0);
                path.lineTo(r.bottomLeft());

                // connect over all the child nodes
                foreach (n, childs)
                {
                    r = node->mapRectFromItem(n, n->contentRect() );
                    path.lineTo( r.topLeft() );
                    path.lineTo( r.topRight() );
                }

                path.lineTo( r.bottomRight() );
                path.lineTo( r.bottomRight().x(), 0);
                path.closeSubpath();
            }
        }
    }

    return path;
}

int COrganizationMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    int                     index = -1;
    QPointF                 p1, p2;
    QList<CMindMapNode*>    childs;

    if (node)
    {
        p1 = node->mapFromScene(p);
        childs = node->childs();
        for (int i = 0; i < childs.length(); ++i)
        {
            CMindMapNode    *n = NULL;

            n = childs.at(i);
            p2 = node->mapFromScene(n->pos());
            if (p2.x() > p1.x())
            {
                index = i;
                break;
            }
        }
    }

    return index;
}

bool COrganizationMapRenderer::rootOnly()
{
	return false;
}

bool COrganizationMapRenderer::collapsible()
{
	return true;
}

qreal COrganizationMapRenderer::xSpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_xSpacing;
        else if (level == 1)
            r = m_xSpacing / 2.0;
        else
            r = m_xSpacing / 4.0;
    }

    return r;
}

qreal COrganizationMapRenderer::ySpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_ySpacing;
        else if (level == 1)
            r = m_ySpacing / 2.0;
        else
            r = m_ySpacing / 3.0;
    }

    return r;
}

//////////////////////////
//tree map layout
CTreeMapRenderer::CTreeMapRenderer(CTreeMapRenderer::Direction d)
    : m_xSpacing(64),
      m_ySpacing(64),
	  m_d(d)
{

}

void CTreeMapRenderer::setDirection(CTreeMapRenderer::Direction d)
{
	m_d = d;
}

CTreeMapRenderer::Direction CTreeMapRenderer::direction() const
{
	return m_d;
}

QSizeF CTreeMapRenderer::recalc(CMindMapNode *node)
{
	qreal					w = 0, h = 0, halfWidth = 0;
	QRectF					rect;
	QSizeF					tmpSize;
	CMindMapNode			*n = NULL;
	QList<CMindMapNode*>	childs;
	qreal					xspacing, yspacing;

	if (!node)
		return QSizeF();

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	childs = node->childs();
	rect = node->contentRect();
	w = rect.width();
	h = rect.height();
	halfWidth = w / 2.0;
	for (int i = 0; i < childs.length(); ++i)
	{
		n = childs.at(i);
		tmpSize = n->areaSize();
		w  = qMax(w, halfWidth + qMax(tmpSize.width() + xspacing, halfWidth));
		h += tmpSize.height();
	}
	
	h += childs.length() * yspacing;

	return QSizeF(w, h);
}

void CTreeMapRenderer::layout(CMindMapNode *node)
{
	QPointF					point;
    QRectF					rect, br, areaRect;
	QSizeF					size, bs;
	QGraphicsItem			*collapser = NULL;
	CMindMapNode			*child = NULL;
	CMindMapNodeRenderer	*renderer = NULL;
	QList<CMindMapNode*>	childs;
	qreal					h = 0;
	qreal					xspacing, yspacing;

	if (!node)
		return;

    xspacing = xSpacing(node);
    yspacing = ySpacing(node);

	collapser = node->collapser();
	point = node->pos();
	rect = node->contentRect();
	size = node->areaSize();
	childs = node->childs();

	// place the collapser
	if (collapser)
	{
		collapser->setPos( QPointF(rect.center().x(), rect.center().y() + rect.height() / 2.0 + COLLAPSER_DISTANCE ) );
	}

	// place the childs
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		br = child->contentRect();
		bs = child->areaSize();
		if (m_d == CTreeMapRenderer::Rightwards)
		{
            areaRect = QRectF(QPointF(point.x() + xspacing, point.y() + rect.height() / 2.0 + yspacing + h), bs);
			child->setRelativePosition(CMindMapNode::East);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::East);
		}
		else
		{
            areaRect = QRectF(QPointF(point.x() - xspacing - bs.width(), point.y() + rect.height() / 2.0 + yspacing + h), bs);
			child->setRelativePosition(CMindMapNode::West);
            child->setAreaRect(areaRect);
			renderer = child->nodeRenderer();
            renderer->place(child, areaRect, CMindMapNode::West);
		}
		h += (bs.height() + yspacing);
	}
}

QPainterPath CTreeMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	qreal					delta = 4;
	QPointF					p1, p2;
	QRectF					r1, r2;
	CMindMapNode			*child = NULL;
	QList<CMindMapNode*>	childs;
	QList<QPointF>			points;
	QPainterPath			path;

	if (!node || !painter)
		return path;

	path.setFillRule(Qt::WindingFill);

	delta =  (node->isRoot() ? 0 : delta);
	r1 = node->contentRect();
	p1 = r1.center() + QPointF(0, r1.height() / 2.0 + delta);

	// determine the incoming connection point
	childs = node->childs();
	for (int i = 0; i < childs.length(); ++i)
	{
		child = childs.at(i);
		r2 = child->contentRect();
		if (child->nodeType() == CMindMapNode::Underline)
			if (m_d == CTreeMapRenderer::Rightwards)
				p2 = child->mapToItem(node, QPointF(r2.left(), r2.bottom()));
			else
				p2 = child->mapToItem(node, QPointF(r2.right(), r2.bottom()));
		else
			if (m_d == CTreeMapRenderer::Rightwards)
				p2 = child->mapToItem(node, QPointF(r2.left(), 0));
			else
				p2 = child->mapToItem(node, QPointF(r2.right(), 0));

		if (pathRenderer)
		{
			points << p1 << QPointF(p1.x(),p2.y()) << p2;
			path.addPath( pathRenderer->draw(painter, points) );
			points.clear();
		}
	}

	return path;
}

void CTreeMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	QRectF		r;

	r = node->contentRect();
	if (m_d == CTreeMapRenderer::Rightwards)
		node->setPos( rect.topLeft() + QPointF(r.width() / 2.0, r.height() / 2.0 ) );
	else
		node->setPos( rect.topRight() + QPointF(-r.width() / 2.0, r.height() / 2.0) );
}

QPainterPath CTreeMapRenderer::connectArea(CMindMapNode *node)
{
    QRectF                  r1, r2;
    QPainterPath            path, rev;
    QList<CMindMapNode*>    childs;
    qreal                   xspacing = 0, yspacing = 0;

    if (node)
    {
        xspacing = xSpacing(node);
        yspacing = ySpacing(node);

        r1 = node->boundingRect();
        if (m_d == CTreeMapRenderer::Leftwards)
            r1 = r1.adjusted(0, r1.height() / 2.0, xspacing, yspacing);
        else
            r1 = r1.adjusted(-xspacing, r1.height() / 2.0, 0, yspacing);

        path.moveTo(0, 0);
        childs = node->childs();
        foreach (CMindMapNode *c, childs)
        {
            path.lineTo(node->mapFromScene(c->pos()));
        }
        r2 = path.boundingRect();

        r2 = r2.united(r1);
        r2.setHeight(qMax(r1.height(), node->areaSize().height()) );

        rev.addRect(r2);
    }

    return rev;
}

int CTreeMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    int                     index = -1;
    QPointF                 p1, p2;
    QList<CMindMapNode*>    childs;

    if (node)
    {
        p1 = node->mapFromScene(p);
        childs = node->childs();
        for (int i = 0; i < childs.length(); ++i)
        {
            CMindMapNode    *n = NULL;

            n = childs.at(i);
            p2 = node->mapFromScene(n->pos());
            if (p2.y() > p1.y())
            {
                index = i;
                break;
            }
        }
    }

    return index;
}

bool CTreeMapRenderer::rootOnly() 
{
	return false;
}

bool CTreeMapRenderer::collapsible()
{
	return true;
}

qreal CTreeMapRenderer::xSpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_xSpacing;
        else if (level == 1)
            r = m_xSpacing / 2.0;
        else
            r = m_xSpacing / 4.0;
    }

    return r;
}

qreal CTreeMapRenderer::ySpacing(CMindMapNode *node) const
{
    int     level = 0;
    qreal   r = 0;

    if (node)
    {
        level = node->level();
        if (level == 0)
            r = m_ySpacing;
        else if (level == 1)
            r = m_ySpacing / 2.0;
        else
            r = m_ySpacing / 4.0;
    }

    return r;
}

/////////////////////////////////
// fish map layout
CFishMapRenderer::CFishMapRenderer(CFishMapRenderer::Direction d)
	: m_xSpacing(12),
	  m_ySpacing(12),
	  m_d(d)
{

}

void CFishMapRenderer::setDirection(CFishMapRenderer::Direction d)
{
	m_d = d;
}

CFishMapRenderer::Direction CFishMapRenderer::direction() const
{
	return m_d;
}

QSizeF CFishMapRenderer::recalc(CMindMapNode *node)
{
	return QSizeF();
}

void CFishMapRenderer::layout(CMindMapNode *node)
{
}

QPainterPath CFishMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	return QPainterPath();
}

void CFishMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	QPointF		cp;

	cp = rect.center();
	if (m_d == CFishMapRenderer::Rightwards)
		node->setPos( rect.left(), cp.y() );
	else
		node->setPos( rect.right(), cp.y() );
}

QPainterPath CFishMapRenderer::connectArea(CMindMapNode *node)
{
    return QPainterPath();
}

int CFishMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    return -1;
}

bool CFishMapRenderer::rootOnly()
{
	return false;
}

bool CFishMapRenderer::collapsible()
{
	return true;
}

//////////////////////////////
// cartoon layout
CCartoonMapRenderer::CCartoonMapRenderer()
	: m_xSpacing(12),
	  m_ySpacing(12)
{

}

QSizeF CCartoonMapRenderer::recalc(CMindMapNode *node)
{
	return QSizeF();
}

void CCartoonMapRenderer::layout(CMindMapNode *node)
{
}

QPainterPath CCartoonMapRenderer::render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer)
{
	return QPainterPath();
}

void CCartoonMapRenderer::place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos)
{
	// EMPTY
}

QPainterPath CCartoonMapRenderer::connectArea(CMindMapNode *node)
{
    return QPainterPath();
}

int CCartoonMapRenderer::connectIndex(CMindMapNode *node, const QPointF &p)
{
    return -1;
}

bool CCartoonMapRenderer::rootOnly()
{
	return true;
}

bool CCartoonMapRenderer::collapsible()
{
	return false;
}
