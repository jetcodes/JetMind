/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMTREELAYOUT_H
#define PRODRAW_CDIAGRAMTREELAYOUT_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramLayout>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramPort>

class CDiagramTreeLayout;
class CDiagramTreeLayoutItem;

class CORE_EXPORT CDiagramTreeLayoutItem
{
public:
	CDiagramTreeLayoutItem(CDiagramItem *item = NULL);
	virtual ~CDiagramTreeLayoutItem();
	
	void addChild(CDiagramTreeLayoutItem *c, CDiagramEdge *edge);
	void addParent(CDiagramTreeLayoutItem *c, CDiagramEdge *edge);
	void addSibling(CDiagramTreeLayoutItem *c, CDiagramEdge *edge);

	CDiagramItem *self();

	int childsCount();
	QList<CDiagramTreeLayoutItem*> childs();

	int parentsCount();
	QList<CDiagramTreeLayoutItem*> parents();

	int siblingsCount();
	QList<CDiagramTreeLayoutItem*> siblings();

	QList<CDiagramEdge*> getEdges(CDiagramTreeLayoutItem *item);
	QList<CDiagramTreeLayoutItem*> getConnectedItems();

	void setMarked(bool value);
	bool marked();
	
	// pre-calculation of the geometry information needed.
	QRectF preLayout();

	// finally update the position of the items according to the
	// layout calculation
	void updatePos();

private:
	Q_DISABLE_COPY(CDiagramTreeLayoutItem);

	bool											m_marked;
	CDiagramItem									*m_self;
	QMap<CDiagramTreeLayoutItem*, CDiagramEdge*>	m_childs;
	QMap<CDiagramTreeLayoutItem*, CDiagramEdge*>	m_parents;
	QMap<CDiagramTreeLayoutItem*, CDiagramEdge*>	m_siblings;

}; // End of CDiagramTreeLayoutItem

class CORE_EXPORT CDiagramTreeLayout : public CDiagramLayout
{
	Q_OBJECT

public:
	CDiagramTreeLayout();
	virtual ~CDiagramTreeLayout();

	enum LayoutMode {
		TipOverMode = 0x0,
		RadialMode,
		FreeMode,
		LevelMode,
		BallonMode
	}; // End of layout mode enums

	enum FlowDirection {
		Right,
		Left,
		Bottom,
		Top
	}; // End of layout direction

	enum LayoutAlignment {
		West,
		East,
		Center
	}; // End of layout alignment

	enum TipOverStyle {
		TipRootOver,
		TipLeavesOver
	}; // End of TipOver Style

	virtual void layoutDiagram(CDiagram *diagram);
	virtual void layoutItem(CDiagramItem *item);

	void setFlowDirection(CDiagramTreeLayout::FlowDirection flow);
	CDiagramTreeLayout::FlowDirection flowDirection();

	void setLayoutMode(CDiagramTreeLayout::LayoutMode mode);
	CDiagramTreeLayout::LayoutMode layoutMode();

	void setLayoutAlignment(CDiagramTreeLayout::LayoutAlignment align);
	CDiagramTreeLayout::LayoutAlignment layoutAlignment();

	void setBranchOffset(qreal value);
	qreal branchOffset();

	void setSiblingOffset(qreal value);
	qreal siblingOffset();

	void setParentOffset(qreal value);
	qreal parentOffset();

protected:
	CDiagramTreeLayoutItem* buildTreeFromItem(CDiagramItem *item, QMap<CDiagramItem*, CDiagramTreeLayoutItem*> &items, QSet<CDiagramNode*> &markedNodes, QSet<CDiagramEdge*> &markedEdges);
	CDiagramTreeLayoutItem* getRoot(const QList<CDiagramTreeLayoutItem*> &graph);

	QRectF layoutTree(CDiagramTreeLayoutItem *item);

	QRectF tipOverLayout(CDiagramTreeLayoutItem *item);
	QRectF radialLayout(CDiagramTreeLayoutItem *item);
	QRectF freeLayout(CDiagramTreeLayoutItem *item);
	QRectF levelLayout(CDiagramTreeLayoutItem *item);
	QRectF ballonLayout(CDiagramTreeLayoutItem *item);

private:
	Q_DISABLE_COPY(CDiagramTreeLayout);

	void resetEdge(CDiagramNode *n1, CDiagramNode *n2, CDiagramEdge *edge);
	qreal computeSize(CDiagramTreeLayoutItem *item, QMap<CDiagramTreeLayoutItem*, qreal> &sizes, qreal sum);
	qreal updatePos(CDiagramTreeLayoutItem *i1, CDiagramTreeLayoutItem *i2, QMap<CDiagramTreeLayoutItem*, qreal> &sizes, qreal sum);

	qreal								m_branchOffset;
	qreal								m_parentOffset;
	qreal								m_siblingOffset;
	
	CDiagramTreeLayout::LayoutAlignment	m_layoutAlign;
	CDiagramTreeLayout::LayoutMode		m_layoutMode;
	CDiagramTreeLayout::FlowDirection	m_flowDirection;

}; // End of class CDiagramTreeLayout

#endif // CDIAGRAMTREELAYOUT_H
