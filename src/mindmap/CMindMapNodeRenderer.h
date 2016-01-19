/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPNODERENDERER_H
#define PRODRAW_CMINDMAPNODERENDERER_H

#include <QtGui>
#include "CMindMapNode.h"

class CMindMapNode;
class CMindMapView;
class CMindMapPathRenderer;

class CMindMapNodeRenderer
{
public:
	// Calculate the size of the specified node, and the size of the node's children
	// should already be set.So the method should only handle one level
	virtual QSizeF recalc(CMindMapNode *node) = 0;

	// Layout the specified node. The size of the node and its children should already
	// be calculated, and the method only layou the first level of children.
	virtual void layout(CMindMapNode *node) = 0;

	// Render the specified node. Node can draw itself's content, but the edge should
	// be draw by this render. The painter should be the one from the node.
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer) = 0;

	// Position the node inside the specified rect.
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos) = 0;

    // calculate the connectable area
    // should return the valud in node's coordination space
    virtual QPainterPath connectArea(CMindMapNode * node) = 0;

    // calculate the connect insert index
    virtual int connectIndex(CMindMapNode *node, const QPointF &p) = 0;

	// Specify whether the layout strategy only availiable for root
	// nodes
	virtual bool rootOnly() = 0;

	// Whether the renderer layout is collapsible. For example
	// mind map layout is not collapsible.
	virtual bool collapsible() = 0;

}; // End of class CMindMapNodeRenderer

class CMindMapRenderer : public CMindMapNodeRenderer
{
public:
	enum Direction {
		DoubleFlow,
		Clockwise,
        AntiClockwise
	}; // End of enumeration

	CMindMapRenderer(CMindMapRenderer::Direction d = Clockwise);

	void setDirection(CMindMapRenderer::Direction d);
	CMindMapRenderer::Direction direction() const;

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

    virtual bool rootOnly();
	virtual bool collapsible();

protected:
    qreal xSpacing(CMindMapNode *node) const;
    qreal ySpacing(CMindMapNode *node) const;

	qreal						m_xSpacing;
	qreal						m_ySpacing;
	CMindMapRenderer::Direction	m_d;

}; // End of class CMindMapClockwiseRenderer

class COrganizationMapRenderer : public CMindMapNodeRenderer
{
public:
	enum Direction {
		Upwards,
		Downwards
	}; // End of enumeration

	COrganizationMapRenderer(COrganizationMapRenderer::Direction d = Downwards );

	void setDirection(COrganizationMapRenderer::Direction d);
	COrganizationMapRenderer::Direction direction() const;

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

	virtual bool rootOnly();
	virtual bool collapsible();

protected:
    qreal xSpacing(CMindMapNode *node) const;
    qreal ySpacing(CMindMapNode *node) const;

	qreal								m_xSpacing;
	qreal								m_ySpacing;
	COrganizationMapRenderer::Direction	m_d;

}; // End of class COrganizationMapRenderer

class CTreeMapRenderer : public CMindMapNodeRenderer
{
public:
	enum Direction {
		Leftwards,
		Rightwards
	}; // End of enumeration

	CTreeMapRenderer(CTreeMapRenderer::Direction d = Rightwards);

	void setDirection(CTreeMapRenderer::Direction d);
	CTreeMapRenderer::Direction direction() const;

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

    virtual bool rootOnly();
	virtual bool collapsible();

protected:
    qreal xSpacing(CMindMapNode *node) const;
    qreal ySpacing(CMindMapNode *node) const;

	qreal						m_xSpacing;
	qreal						m_ySpacing;
	CTreeMapRenderer::Direction	m_d;

}; // End of class CTreeMapRenderer

class CLogicMapRenderer : public CMindMapNodeRenderer
{
public:
	enum Direction {
		Leftwards,
		Rightwards
	}; // End of enumeration

	CLogicMapRenderer(CLogicMapRenderer::Direction d = Rightwards);
	
	void setDirection(CLogicMapRenderer::Direction d);
	CLogicMapRenderer::Direction direction() const;

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

    virtual bool rootOnly();
	virtual bool collapsible();

protected:
    qreal xSpacing(CMindMapNode *node) const;
    qreal ySpacing(CMindMapNode *node) const;

	qreal							m_xSpacing;
	qreal							m_ySpacing;
	CLogicMapRenderer::Direction	m_d;

}; // End of class CLogicMapRenderer

class CFishMapRenderer : public CMindMapNodeRenderer
{
public:
	enum Direction {
		Leftwards,
		Rightwards
	}; // End of enumeration

	CFishMapRenderer(CFishMapRenderer::Direction d = Rightwards);

	void setDirection(CFishMapRenderer::Direction d);
	CFishMapRenderer::Direction direction() const;

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

    virtual bool rootOnly();
	virtual bool collapsible();

protected:

	qreal						m_xSpacing;
	qreal						m_ySpacing;
	CFishMapRenderer::Direction	m_d;

}; // End of class CFishMapRenderer

class CCartoonMapRenderer : public CMindMapNodeRenderer
{
public:
	CCartoonMapRenderer();

	virtual QSizeF recalc(CMindMapNode *node);
	virtual void layout(CMindMapNode *node);
	virtual QPainterPath render(CMindMapNode *node, QPainter *painter, CMindMapPathRenderer *pathRenderer);
    virtual void place(CMindMapNode *node, const QRectF &rect, const CMindMapNode::RelativePositions pos);
    virtual QPainterPath connectArea(CMindMapNode *node);
    virtual int connectIndex(CMindMapNode *node, const QPointF &p);

    virtual bool rootOnly();
	virtual bool collapsible();

protected:

	qreal						m_xSpacing;
	qreal						m_ySpacing;

}; // End of class CCartoonMapRenderer

#endif // PRODRAW_CMINDMAPNODERENDERER_H
