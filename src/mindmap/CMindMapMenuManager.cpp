/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>

#include "CMindMapNode.h"
#include "CMindMapNodeSprite.h"
#include "CMindMapNodeGroup.h"
#include "CMindMapMenuManager.h"

CMindMapMenuManager* CMindMapMenuManager::m_inst = NULL;

void CMindMapNodeSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
    CMindMapNode        *node = NULL;

    if (!sprite)
        return;

    node = sprite->node();
    if (node)
    {
        sprite->scene()->clearSelection();
        node->setSelected(true);
    }
}

void CMindMapNodeGroupMenu::reset(CMindMapNodeGroup *group)
{
    Q_UNUSED(group);
    // Empty
}

void CMindMapMenuManager::setNodeSpriteMenu(int type, CMindMapNodeSpriteMenu *menu)
{
	if (!menu || type < 0)
		return;

	m_nodeSpriteMenus[type] = menu;
}

CMindMapNodeSpriteMenu* CMindMapMenuManager::nodeSpriteMenu(int type)
{
	CMindMapNodeSpriteMenu		*menu = NULL;

	if (m_nodeSpriteMenus.contains(type))
		menu = m_nodeSpriteMenus[type];

	return menu;
}

void CMindMapMenuManager::setNodeGroupMenu(int type, CMindMapNodeGroupMenu *menu)
{
    if (!menu || type < 0)
        return;

    m_nodeGroupMenus[type] = menu;
}

CMindMapNodeGroupMenu* CMindMapMenuManager::nodeGroupMenu(int type)
{
    CMindMapNodeGroupMenu		*menu = NULL;

    if (m_nodeGroupMenus.contains(type))
        menu = m_nodeGroupMenus[type];

    return menu;
}
