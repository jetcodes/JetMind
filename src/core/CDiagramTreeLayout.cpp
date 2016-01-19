/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramTreeLayout>

#define MIN_SPACING 10

struct StackRecord 
{
	StackRecord() 
	{
	}

    StackRecord(CDiagramTreeLayoutItem *i1, int i2, qreal i3, const QList<CDiagramTreeLayoutItem*> &i4)
		: item(i1),
		  index(i2),
		  size(i3),
		  items(i4)
	{
	}

	CDiagramTreeLayoutItem			*item;
	int								index;
	qreal							size;
	QList<CDiagramTreeLayoutItem*>	items;
	QList<StackRecord*>				childs;

}; // End of struct StackRecord


CDiagramTreeLayoutItem::CDiagramTreeLayoutItem(CDiagramItem *item)
	: m_self(item),
	  m_marked(false)
{
}

CDiagramTreeLayoutItem::~CDiagramTreeLayoutItem()
{
}

void CDiagramTreeLayoutItem::addChild(CDiagramTreeLayoutItem *c, CDiagramEdge *edge)
{
	m_childs.insertMulti(c, edge);
}

void CDiagramTreeLayoutItem::addParent(CDiagramTreeLayoutItem *c, CDiagramEdge *edge)
{
	m_parents.insertMulti(c, edge);
}

void CDiagramTreeLayoutItem::addSibling(CDiagramTreeLayoutItem *c, CDiagramEdge *edge)
{
	m_siblings.insertMulti(c, edge);
}

CDiagramItem* CDiagramTreeLayoutItem::self()
{
	return m_self;
}

int CDiagramTreeLayoutItem::childsCount()
{
	return m_childs.count();
}

QList<CDiagramTreeLayoutItem*> CDiagramTreeLayoutItem::childs()
{
	return m_childs.keys();
}

int CDiagramTreeLayoutItem::parentsCount()
{
	return m_parents.count();
}

QList<CDiagramTreeLayoutItem*> CDiagramTreeLayoutItem::parents()
{
	return m_parents.keys();
}

int CDiagramTreeLayoutItem::siblingsCount()
{
	return m_siblings.count();
}

QList<CDiagramTreeLayoutItem*> CDiagramTreeLayoutItem::siblings()
{
	return m_siblings.keys();
}

QList<CDiagramEdge*> CDiagramTreeLayoutItem::getEdges(CDiagramTreeLayoutItem *item)
{
	QSet<CDiagramEdge*>	edges;

	edges.unite( m_childs.values(item).toSet() );
	edges.unite( m_parents.values(item).toSet() );
	edges.unite( m_siblings.values(item).toSet() );

	return edges.toList();
}

QList<CDiagramTreeLayoutItem*> CDiagramTreeLayoutItem::getConnectedItems()
{
	QSet<CDiagramTreeLayoutItem*>	items;

	items.unite(m_childs.keys().toSet());
	items.unite(m_parents.keys().toSet());
	items.unite(m_siblings.keys().toSet());

	return items.toList();
}

void CDiagramTreeLayoutItem::setMarked(bool value)
{
	m_marked = value;
}

bool CDiagramTreeLayoutItem::marked()
{
	return m_marked;
}

QRectF CDiagramTreeLayoutItem::preLayout()
{
	
	return QRectF();
}

void CDiagramTreeLayoutItem::updatePos()
{
}

CDiagramTreeLayout::CDiagramTreeLayout()
	: CDiagramLayout(NULL),
	  m_branchOffset(30),
	  m_parentOffset(25),
	  m_siblingOffset(10),
	  m_layoutMode(CDiagramTreeLayout::FreeMode),
	  m_layoutAlign(CDiagramTreeLayout::Center),
	  m_flowDirection(CDiagramTreeLayout::Right)
{
	
}

CDiagramTreeLayout::~CDiagramTreeLayout()
{

}

void CDiagramTreeLayout::setLayoutMode(CDiagramTreeLayout::LayoutMode mode)
{
	m_layoutMode = mode;
}

CDiagramTreeLayout::LayoutMode CDiagramTreeLayout::layoutMode()
{
	return m_layoutMode;
}

void CDiagramTreeLayout::setLayoutAlignment(CDiagramTreeLayout::LayoutAlignment align)
{
	m_layoutAlign = align;
}

CDiagramTreeLayout::LayoutAlignment CDiagramTreeLayout::layoutAlignment()
{
	return m_layoutAlign;
}

void CDiagramTreeLayout::setFlowDirection(CDiagramTreeLayout::FlowDirection flow)
{
	m_flowDirection = flow;
}

CDiagramTreeLayout::FlowDirection CDiagramTreeLayout::flowDirection()
{
	return m_flowDirection;
}

void CDiagramTreeLayout::setBranchOffset(qreal value)
{
	if (value <= MIN_SPACING)
		return;

	m_branchOffset = value;
}

qreal CDiagramTreeLayout::branchOffset()
{
	return m_branchOffset;
}

void CDiagramTreeLayout::setSiblingOffset(qreal value)
{
	if (value <= MIN_SPACING)
		return;

	m_siblingOffset = value;
}

qreal CDiagramTreeLayout::siblingOffset()
{
	return m_siblingOffset;
}

void CDiagramTreeLayout::setParentOffset(qreal value)
{
	if (value <= MIN_SPACING)
		return;

	m_parentOffset = value;
}

qreal CDiagramTreeLayout::parentOffset()
{
	return m_parentOffset;
}

void CDiagramTreeLayout::layoutDiagram(CDiagram *diagram)
{
	CDiagramItem									*item = NULL;
	CDiagramNode									*node = NULL;
	QList<CDiagramItem*>							items;
	QSet<CDiagramNode*>								markedNodes;
	QSet<CDiagramEdge*>								markedEdges;
	CDiagramTreeLayoutItem							*treeItem = NULL;
	QList<QList<CDiagramTreeLayoutItem*> >			graphs;

	if (!diagram)
		return;
	
	items = diagram->topLevelItems();
	for (int i = 0; i < items.length() ; ++ i)
	{
		QMap<CDiagramItem*, CDiagramTreeLayoutItem*>	graph;
		item = items.at(i);
		node = dynamic_cast<CDiagramNode*>(item);
		if (!item ||  !node || markedNodes.contains(node) ) 
			continue;

		buildTreeFromItem(item, graph, markedNodes, markedEdges);
		graphs.push_back(graph.values());
	}

	// For different graph calculate the layout
	// O(n)
	for (int i = 0; i < graphs.length(); ++i)
	{
		// O(n)
		treeItem = getRoot(graphs.at(i));
		if (treeItem)
		{
			// O(n)
			layoutTree(treeItem);
		}
	}

	// CLEAN UP
	for (int i = 0; i < graphs.length(); ++i)
	{
		for (int j = 0; j < graphs[i].length(); ++j)
		{
			SAFE_DELETE(graphs[i][j]);
		}
	}
}

void CDiagramTreeLayout::layoutItem(CDiagramItem *item)
{
	QSet<CDiagramNode*>							 markedNodes;
	QSet<CDiagramEdge*>							 markedEdges;
	CDiagramTreeLayoutItem						 *root = NULL;
	QMap<CDiagramItem*, CDiagramTreeLayoutItem*> items;

	if (!item)
		return;

	buildTreeFromItem(item, items, markedNodes, markedEdges);
	root = items.value(item);
	if (root)
		layoutTree(root);
}

CDiagramTreeLayoutItem* CDiagramTreeLayout::getRoot(const QList<CDiagramTreeLayoutItem*> &items)
{
	int								childMarks = 4;
	int								parentMarks = -32;
	int								siblingMarks = 0;
	int								marks = 0, curr = 0;
	CDiagramTreeLayoutItem			*r = NULL;
	CDiagramTreeLayoutItem			*item = NULL;
	QList<CDiagramTreeLayoutItem*>	tops;

	for (int i = 0; i < items.length() ; ++i)
	{
		item = items.at(i);
		if (item && item->parentsCount() == 0)
		{
			tops.push_back(item);
		}
	}

	if (tops.length() == 0)
		tops = items;

	for (int i = 0; i < tops.length(); ++i)
	{
		item = tops.at(i);
		curr = item->childsCount() * childMarks + item->parentsCount() * parentMarks + item->siblingsCount() * siblingMarks;
		if (i == 0 || curr > marks)
		{
			r = item;
			marks = curr;
		}
	}

	return r;
}

CDiagramTreeLayoutItem* CDiagramTreeLayout::buildTreeFromItem(CDiagramItem *item, QMap<CDiagramItem*, CDiagramTreeLayoutItem*> &items, QSet<CDiagramNode*> &markedNodes, QSet<CDiagramEdge*> &markedEdges)
{
	CDiagramEdge				*edge = NULL;
	CDiagramNode				*node = NULL;
	CDiagramNode				*n1 = NULL, *n2 = NULL;
	CDiagramPort				*p1 = NULL, *p2 = NULL;
	QList<CDiagramEdge*>		edges;
	CDiagramTreeLayoutItem		*treeItem1 = NULL, *treeItem2 = NULL;

	node = dynamic_cast<CDiagramNode*>(item);
	if (!item || !node)
		return treeItem1;

	treeItem1 = items.contains(item) ? items[item] : new CDiagramTreeLayoutItem(item);
	if (!treeItem1)
		return treeItem1;

	items.insert(item, treeItem1);
	markedNodes.insert(node);

	edges = node->getEdges().toList();
	for (int i = 0; i < edges.length() ; ++i)
	{
		edge = edges.at(i);
		if (edge && !markedEdges.contains(edge) )
		{
			markedEdges.insert(edge);

			p1 = edge->startPort();
			p2 = edge->endPort();
			if (!p1 || !p2)
				continue;

			n1 = p1->getParentNode();
			n2 = p2->getParentNode();
			if (n1 == node)
			{
				// start
				treeItem2 = buildTreeFromItem(n2, items, markedNodes, markedEdges);
				if (!treeItem2) continue;

				switch(edge->edgeDirection())
				{
				case CDiagramEdge::Forward:
					treeItem1->addChild(treeItem2, edge);
					treeItem2->addParent(treeItem1, edge);
					break;

				case CDiagramEdge::Backward:
					treeItem2->addChild(treeItem1, edge);
					treeItem1->addParent(treeItem2, edge);
					break;

				case CDiagramEdge::BothDirections:
					treeItem2->addSibling(treeItem1, edge);
					treeItem1->addSibling(treeItem2, edge);
					break;

				default:
					break;
				};
			}
			else
			{
				treeItem2 = buildTreeFromItem(n1, items, markedNodes, markedEdges);
				if (!treeItem2) continue;

				switch(edge->edgeDirection())
				{
				case CDiagramEdge::Forward:
					treeItem2->addChild(treeItem1, edge);
					treeItem1->addParent(treeItem2, edge);
					break;

				case CDiagramEdge::Backward:
					treeItem1->addChild(treeItem2, edge);
					treeItem2->addParent(treeItem1, edge);
					break;

				case CDiagramEdge::BothDirections:
					treeItem2->addSibling(treeItem1, edge);
					treeItem1->addSibling(treeItem2, edge);
					break;

				default:
					break;
				};
			}
		}
	}
	return treeItem1;
}

QRectF CDiagramTreeLayout::layoutTree(CDiagramTreeLayoutItem* root)
{
	QRectF		r;

	r = freeLayout(root);
	/*
	switch( layoutMode() )
	{
	case TipOverMode:
		r = tipOverLayout(root);
		break;
	
	case LevelMode:
		r = levelLayout(root);
		break;

	case FreeMode:
		r = freeLayout(root);
		break;

	case RadialMode:
		r = radialLayout(root);
		break;


	case BallonMode:
		r = ballonLayout(root);

	default:
		break;
	}; // End of switch
	*/
	return r;
}

QRectF CDiagramTreeLayout::tipOverLayout(CDiagramTreeLayoutItem *root)
{
	int										index = 0;
	qreal									start = 0, r = 0, width = 0;
	QRectF									rect;
	StackRecord								*currStackRecord = NULL, *nextStackRecord = NULL;
	QStack<StackRecord*>					virtualStack;
	QSet<CDiagramTreeLayoutItem*>			visitedItems;
	QList<CDiagramTreeLayoutItem*>			childItems;
	CDiagramTreeLayoutItem					*item = NULL;
	QMap<CDiagramTreeLayoutItem*, qreal>	sizes;
	CDiagramEdge							*edge = NULL;
	CDiagramNode							*n1 = NULL, *n2 = NULL;
	CDiagramPort							*p1 = NULL, *p2 = NULL;
	QList<CDiagramEdge*>					edges;

	if (!root)
		return rect;

	childItems = root->getConnectedItems();
    currStackRecord = new StackRecord(root, 0, 0.0, QList<CDiagramTreeLayoutItem*>() );
	virtualStack.push( currStackRecord );
	visitedItems.insert(root);
	for (int i = 0; i < childItems.length(); ++i)
	{
		item = childItems.at(i);
		if (!visitedItems.contains(item))
		{
			visitedItems.insert(item);
			currStackRecord->items.push_back(item);
		}
	}
	
	while (!virtualStack.empty())
	{
		currStackRecord = virtualStack.top();
		index = currStackRecord->index;
		currStackRecord->index += 1;
		if (index >= 0 && index < currStackRecord->items.length())
		{
			item = currStackRecord->items.at(index);
			childItems = item->getConnectedItems();
            nextStackRecord = new StackRecord(item, 0, 0.0, QList<CDiagramTreeLayoutItem*>());
			virtualStack.push( nextStackRecord );
			
			currStackRecord->childs.push_back(nextStackRecord);
			for (int i = 0; i < childItems.length(); ++i)
			{
				item = childItems.at(i);
				if (!visitedItems.contains(item))
				{
					visitedItems.insert(item);
					nextStackRecord->items.push_back(item);
				}
			}
			continue;
		}

		currStackRecord = virtualStack.pop();
		r = computeSize(currStackRecord->item, sizes, currStackRecord->size);
		if (!virtualStack.empty())
		{
			currStackRecord = virtualStack.top();
			currStackRecord->size += r;
		}
	}

	// perform layout
	if (currStackRecord)
	{
		currStackRecord->index = 0;
		currStackRecord->size = 0;
		virtualStack.push(currStackRecord);
		while (!virtualStack.empty())
		{
			currStackRecord = virtualStack.top();
			index = currStackRecord->index;
			currStackRecord->index += 1;

			if (index >= 0 && index < currStackRecord->childs.length())
			{
				nextStackRecord = currStackRecord->childs.at(index);
				virtualStack.push( nextStackRecord );

				// re-assign the ports to the edges
				edges = currStackRecord->item->getEdges(nextStackRecord->item);
				for (int i = 0; i < edges.length(); ++i)
				{
					edge = edges.at(i);
					n1 = dynamic_cast<CDiagramNode*>( currStackRecord->item->self() );
					n2 = dynamic_cast<CDiagramNode*>( nextStackRecord->item->self() );
					resetEdge(n1, n2, edge);
				}

				r = updatePos(currStackRecord->item, nextStackRecord->item, sizes, currStackRecord->size);
				currStackRecord->size += r;
				nextStackRecord->index = 0;
				nextStackRecord->size = 0;

				continue;
			}

			currStackRecord = virtualStack.pop();
			SAFE_DELETE(currStackRecord);
		}
	}
	return rect;
}

QRectF CDiagramTreeLayout::levelLayout(CDiagramTreeLayoutItem *root)
{
	
	return QRectF();
}

QRectF CDiagramTreeLayout::freeLayout(CDiagramTreeLayoutItem *root)
{
	int										index = 0;
	qreal									start = 0, r = 0, width = 0;
	QRectF									rect;
	StackRecord								*currStackRecord = NULL, *nextStackRecord = NULL;
	QStack<StackRecord*>					virtualStack;
	QSet<CDiagramTreeLayoutItem*>			visitedItems;
	QList<CDiagramTreeLayoutItem*>			childItems;
	CDiagramTreeLayoutItem					*item = NULL;
	QMap<CDiagramTreeLayoutItem*, qreal>	sizes;
	CDiagramEdge							*edge = NULL;
	CDiagramNode							*n1 = NULL, *n2 = NULL;
	CDiagramPort							*p1 = NULL, *p2 = NULL;
	QList<CDiagramEdge*>					edges;

	if (!root)
		return rect;

	childItems = root->getConnectedItems();
    currStackRecord = new StackRecord(root, 0, 0.0, QList<CDiagramTreeLayoutItem*>() );
	virtualStack.push( currStackRecord );
	visitedItems.insert(root);
	for (int i = 0; i < childItems.length(); ++i)
	{
		item = childItems.at(i);
		if (!visitedItems.contains(item))
		{
			visitedItems.insert(item);
			currStackRecord->items.push_back(item);
		}
	}
	
	while (!virtualStack.empty())
	{
		currStackRecord = virtualStack.top();
		index = currStackRecord->index;
		currStackRecord->index += 1;
		if (index >= 0 && index < currStackRecord->items.length())
		{
			item = currStackRecord->items.at(index);
			childItems = item->getConnectedItems();
            nextStackRecord = new StackRecord(item, 0, 0, QList<CDiagramTreeLayoutItem*>());
			virtualStack.push( nextStackRecord );

			currStackRecord->childs.push_back(nextStackRecord);
			for (int i = 0; i < childItems.length(); ++i)
			{
				item = childItems.at(i);
				if (!visitedItems.contains(item))
				{
					visitedItems.insert(item);
					nextStackRecord->items.push_back(item);
				}
			}
			continue;
		}

		currStackRecord = virtualStack.pop();
		r = computeSize(currStackRecord->item, sizes, currStackRecord->size);
		if (!virtualStack.empty())
		{
			currStackRecord = virtualStack.top();
			currStackRecord->size += r;
		}
	}

	// perform layout
	if (currStackRecord)
	{
		currStackRecord->index = 0;
		currStackRecord->size = 0;
		virtualStack.push(currStackRecord);
		while (!virtualStack.empty())
		{
			currStackRecord = virtualStack.top();
			index = currStackRecord->index;
			currStackRecord->index += 1;

			if (index >= 0 && index < currStackRecord->childs.length())
			{
				nextStackRecord = currStackRecord->childs.at(index);
				virtualStack.push( nextStackRecord );

				// re-assign the ports to the edges
				edges = currStackRecord->item->getEdges(nextStackRecord->item);
				for (int i = 0; i < edges.length(); ++i)
				{
					edge = edges.at(i);
					n1 = dynamic_cast<CDiagramNode*>( currStackRecord->item->self() );
					n2 = dynamic_cast<CDiagramNode*>( nextStackRecord->item->self() );
					resetEdge(n1, n2, edge);
				}

				r = updatePos(currStackRecord->item, nextStackRecord->item, sizes, currStackRecord->size);
				currStackRecord->size += r;
				nextStackRecord->index = 0;
				nextStackRecord->size = 0;

				continue;
			}

			currStackRecord = virtualStack.pop();
			SAFE_DELETE(currStackRecord);
		}
	}
	return QRectF();
}

QRectF CDiagramTreeLayout::radialLayout(CDiagramTreeLayoutItem *root)
{

	return QRectF();
}

QRectF CDiagramTreeLayout::ballonLayout(CDiagramTreeLayoutItem *root)
{
	return QRectF();
}

void CDiagramTreeLayout::resetEdge(CDiagramNode *n1, CDiagramNode *n2, CDiagramEdge *edge)
{
	CDiagramPort	*p1 = NULL, *p2 = NULL;

	switch(layoutMode())
	{
	case FreeMode:
		switch(flowDirection())
		{
		case Bottom:
			p1 = n1->getPortsBySide(Global::South).last();
			p2 = n2->getPortsBySide(Global::North).last();
			break;

		case Top:
			p1 = n1->getPortsBySide(Global::North).last();
			p2 = n2->getPortsBySide(Global::South).last();
			break;

		case Left:
			p1 = n1->getPortsBySide(Global::West).last();
			p2 = n2->getPortsBySide(Global::East).last();
			break;

		case Right:
			p1 = n1->getPortsBySide(Global::East).last();
			p2 = n2->getPortsBySide(Global::West).last();
			break;

		default:
			break;
		};
		break;

	case TipOverMode:
		p1 = n1->getPortsBySide(Global::South).last();
		p2 = n2->getPortsBySide(Global::West).last();
		break;

	default:
		break;
	};

	edge->setAutoTrack(false);
	if (n1 == edge->startPort()->getParentNode())
	{
		if (edge->startPort() != p1)
		{
			edge->startPort()->removeEdge(edge);
			edge->setStartPort(p1);
			p1->addEdge(edge);
		}
		if (edge->endPort() != p2)
		{
			edge->endPort()->removeEdge(edge);
			edge->setEndPort( p2 );
			p2->addEdge(edge);
		}
	}
	else
	{
		if (edge->startPort() != p2)
		{
			edge->startPort()->removeEdge(edge);
			edge->setStartPort(p2);
			p2->addEdge(edge);
		}
		if (edge->endPort() != p1)
		{
			edge->endPort()->removeEdge(edge);
			edge->setEndPort( p1 );
			p1->addEdge(edge);
		}
	}
}

qreal CDiagramTreeLayout::updatePos(CDiagramTreeLayoutItem *i1, CDiagramTreeLayoutItem *i2, QMap<CDiagramTreeLayoutItem*, qreal> &sizes, qreal sum)
{
	qreal		r;
	qreal		start;

	switch(layoutMode())
	{
	case FreeMode:

		switch(flowDirection())
		{
		case Bottom:
			r = sizes[i1];
			start = i1->self()->x() - r / 2.0 + sum;

			r = sizes[i2];
			i2->self()->setPos(start + r / 2.0, i1->self()->y() + (i1->self()->height() + i2->self()->height()) / 2.0 + m_parentOffset);
			r += m_siblingOffset;
			break;

		case Top:
			r = sizes[i1];
			start = i1->self()->x() - r / 2.0 + sum;

			r = sizes[i2];
			i2->self()->setPos(start + r / 2.0, i1->self()->y() - (i1->self()->height() + i2->self()->height()) / 2.0 - m_parentOffset);
			r += m_siblingOffset;
			break;

		case Left:
			r = sizes[i1];
			start = i1->self()->y() - r / 2.0 + sum;

			r = sizes[i2];
			i2->self()->setPos( i1->self()->x() - (i1->self()->width() + i2->self()->width())/2.0 - m_parentOffset, start + r / 2.0);
			r += m_siblingOffset;
			break;

		case Right:
			r = sizes[i1];
			start = i1->self()->y() - r / 2.0 + sum;

			r = sizes[i2];
			i2->self()->setPos( i1->self()->x() + (i1->self()->width() + i2->self()->width())/2.0 + m_parentOffset, start + r / 2.0);
			r += m_siblingOffset;
			break;

		default:
			break;
		};
		break;

	case TipOverMode:
		r = sizes[i1];
		start = i1->self()->y() + i1->self()->height() / 2.0 + m_parentOffset + sum;
		
		r = sizes[i2];
		i2->self()->setPos( i1->self()->x() + m_parentOffset + i2->self()->width() / 2.0, start + i2->self()->height() / 2.0 );
		r += m_siblingOffset;
		break;

	default:
		break;
	};

	return r;
}

qreal CDiagramTreeLayout::computeSize(CDiagramTreeLayoutItem *item, QMap<CDiagramTreeLayoutItem*, qreal> &sizes, qreal sum)
{
	qreal		r;

	switch(layoutMode())
	{
	case FreeMode:
		switch(flowDirection())
		{
		case Bottom:
		case Top:
			r = qMax(item->self()->width(), sum);
			if (r == item->self()->width())
				sizes.insert(item, r);
			else
				sizes.insert(item, r - m_siblingOffset);
			r = (r + m_siblingOffset);
			break;

		case Left:
		case Right:
			r = qMax(item->self()->height(), sum);
			if (r == item->self()->height())
				sizes.insert(item, r);
			else
				sizes.insert(item, r - m_siblingOffset);
			r = (r + m_siblingOffset);
			break;

		default:
			break;
		};
		break;

	case TipOverMode:
		// r = qMax(item->self()->height(), sum);
		/*
		if (r == item->self()->height())
			sizes.insert(item, r);
		else
			sizes.insert(item, r - m_vSpacing);
		*/
		r = item->self()->height() + sum;
		r = (r + m_siblingOffset);
		sizes.insert(item, r);
		break;

	default:
		break;
	};

	return r;
}
