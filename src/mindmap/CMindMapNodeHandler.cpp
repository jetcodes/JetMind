/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramNode>
#include <core/CDiagramNodeHandler>
#include <core/CDiagramGroupHandler>

#include "CMindMapNode.h"
#include "CMindMapNodeGroup.h"
#include "CMindMapNodeHandler.h"

bool levelGreaterThan(CMindMapNode *n1, CMindMapNode *n2)
{
	return (n1->level() >= n2->level());
}

CMindMapNodeHandler::CMindMapNodeHandler()
	:CDiagramItemHandler()
{
	setPriority(1);
}

CMindMapNodeHandler::~CMindMapNodeHandler()
{
}

int CMindMapNodeHandler::spriteType()
{
	return CDiagramNodeHandler::SpriteType;
}

bool CMindMapNodeHandler::support(const QList<CDiagramItem*> &items)
{
	bool			r = false;
	CDiagramItem	*item = NULL;
	
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (dynamic_cast<CMindMapNode*>(item) || dynamic_cast<CMindMapNodeGroup*>(item))
		{
			r = true;
			break;
		}
	}
	return r;
}

void CMindMapNodeHandler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
    Q_UNUSED(painter);
}

void CMindMapNodeHandler::initDraggers()
{
}

CMindMapGroupHandler::CMindMapGroupHandler()
	: CMindMapNodeHandler()
{
}

CMindMapGroupHandler::~CMindMapGroupHandler()
{
}

int CMindMapGroupHandler::spriteType()
{
	return CDiagramGroupHandler::SpriteType;
}
