/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramCommand>
#include <core/CDiagramCommands>
#include <core/CDiagramClipboard>

#include "CMindMapNode.h"
#include "CMindMapNodeGroup.h"
#include "CMindMapNodeSprite.h"
#include "CMindMapIconProvider.h"
#include "CMindMapCommands.h"

extern bool levelGreaterThan(CMindMapNode *n1, CMindMapNode *n2);

void extractSubNodes(CMindMapNode *node, QSet<CMindMapNode*> &nodes)
{
	QList<CMindMapNode*>	subNodes;

	if (!node)
		return;

	subNodes = node->childs();
	foreach (CMindMapNode *t, subNodes)
	{
		nodes.insert(t);
		extractSubNodes(t, nodes);
	}
}

CMindMapDraggingCommand::CMindMapDraggingCommand(CDiagram *d, CDiagramItem *item, QPointF &p, QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_item(item),
	  m_newPosition(p)
{
	m_oldPosition = item->pos();
}

void CMindMapDraggingCommand::redo()
{
	m_item->setPos( m_newPosition );
	m_d->emitItemsGeometryChanged();
}

void CMindMapDraggingCommand::undo()
{
	m_item->setPos( m_oldPosition );
	m_d->emitItemsGeometryChanged();
}


CMindMapChangePropertyCommand::CMindMapChangePropertyCommand(CDiagram *d, 
                           const QList<CDiagramItem*> nodes,
						   const char *name, 
						   const QVariant &value, 
						   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_nodes(nodes),
	  m_name(name)
{
    foreach (CDiagramItem *n, m_nodes)
	{
		m_oldValues.push_back( n->property(m_name) );
		m_newValues.push_back( value );
	}
}

CMindMapChangePropertyCommand::CMindMapChangePropertyCommand(CDiagram *d, 
                           const QList<CDiagramItem*> nodes,
						   const char *name, 
						   const QList<QVariant> &values, 
						   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_nodes(nodes),
	  m_name(name),
	  m_newValues(values)
{
    foreach (CDiagramItem *n, m_nodes)
	{
		m_oldValues.push_back( n->property(m_name) );
	}
}

void CMindMapChangePropertyCommand::undo()
{
	int				i = 0;
    CDiagramItem	*n = NULL;
	
	if (m_oldValues.size() != m_nodes.size()) return;

	for (i = 0; i < m_nodes.size(); ++i)
	{
		n = m_nodes.at(i);
		n->setProperty(m_name, m_oldValues.at(i));
	}
	
	QUndoCommand::undo();
}

void CMindMapChangePropertyCommand::redo()
{
	if (m_newValues.size() == m_nodes.size())
	{
		for (int i = 0; i < m_nodes.size(); ++i)
		{
			m_nodes.at(i)->setProperty(m_name, m_newValues[i]);
		}
	}
	QUndoCommand::redo();
}

CMindMapNewNodeCommand::CMindMapNewNodeCommand(CDiagram *d,
											   CMindMapNode *parentNode, 
											   CMindMapNode *refNode,
											   bool beforeRef,
                                               bool duplicateStyle,
											   QUndoCommand *parent)
	: CDiagramCommand(d, parent),
      m_newNode(NULL),
	  m_parentNode(parentNode),
	  m_refNode(refNode),
      m_beforeRef(beforeRef),
      m_duplicateStyle(duplicateStyle)
{
}

void CMindMapNewNodeCommand::redo()
{
    int                         refIndex = -1;
    CMindMapNodeGroup           *group = NULL;
    QList<CMindMapNodeGroup*>   groups;

	if (!m_parentNode)
		return;

    m_newNode = m_parentNode->createChildNode();
    if (m_newNode)
    {
        // update groups
        if (m_refNode)
            refIndex = m_refNode->index();

        if (m_duplicateStyle)
        {
            if (m_refNode)
            {
                m_newNode->setBrush( m_refNode->brush() );
                m_newNode->setPen( m_refNode->pen() );
            }
            else
            {
                m_newNode->setBrush( m_parentNode->brush() );
                m_newNode->setPen( m_parentNode->pen() );
            }
        }

        m_d->insertItem(m_newNode, m_newNode->pos());
        m_parentNode->addChild(m_newNode, m_refNode, m_beforeRef);

        groups = m_parentNode->groups();
        foreach (group, groups)
        {
            group->onAddChild(m_newNode->index(), refIndex);
        }

		m_parentNode->onStructChanged();
        m_d->clearSelection();
        m_newNode->startEditingContent();
	}
}

void CMindMapNewNodeCommand::undo()
{
    int                         index = -1;
    CMindMapNodeGroup           *group = NULL;
    QList<CMindMapNodeGroup*>   groups;

	if (!m_newNode || !m_parentNode)
		return;

    index = m_newNode->index();
    m_parentNode->dropChild(m_newNode);
    groups = m_parentNode->groups();
    foreach (group, groups)
    {
        group->onDeleteChild(index);
    }

	m_d->deleteItem(m_newNode);
	m_parentNode->onStructChanged();
    SAFE_DELETE(m_newNode);
}

CMindMapDelNodeCommand::CMindMapDelNodeCommand(CDiagram *d,
											   QList<CMindMapNode*> &delNodes,
											   QUndoCommand *parent)
	: CDiagramCommand(d, parent)
{
	QSet<CMindMapNode*>		nodes;

	nodes = QSet<CMindMapNode*>::fromList(delNodes);
	foreach (CMindMapNode* node, delNodes)
	{
		extractSubNodes(node, nodes);
	}

	m_delNodes = nodes.toList();
	qStableSort(m_delNodes.begin(), m_delNodes.end(), levelGreaterThan);
}

void CMindMapDelNodeCommand::redo()
{
    CMindMapNodeGroup           *group = NULL;
    QList<CMindMapNodeGroup*>   groups;
	
	m_indexes.clear();
	m_parentNodes.clear();
	m_groups.clear();
	m_groupMarkers.clear();

	foreach (CMindMapNode *node, m_delNodes)
    {
        int                         index = -1, ref = -1;
		CMindMapNode                *parentNode = NULL;
        QList<int>					groupMarkerRecs;
        QList<CMindMapNodeGroup*>   groupRecs;

		parentNode = node->upLevelNode();
		if (parentNode)
		{
            index = node->index();
            groups = parentNode->groups();
            foreach (group, groups)
            {
                groupRecs.push_back(group);
                ref = group->onDeleteChild(index);
                if (group->isEmpty())
                {
                    parentNode->dropNodeGroup(group);
                    m_d->deleteItem(group);
                    groupMarkerRecs.push_back(-2);
                }
                else
                {
                    groupMarkerRecs.push_back(ref);
                }
            }
			
			parentNode->dropChild(node);
		}

		m_indexes.push_back( index );
		m_parentNodes.push_back( parentNode );
		m_groups.push_back( groupRecs );
		m_groupMarkers.push_back( groupMarkerRecs );
		m_d->deleteItem(node);
	}
}

void CMindMapDelNodeCommand::undo()
{
    int                             index = 0, ref = -1;
    CMindMapNode                    *node = NULL;
    CMindMapNode                    *parentNode = NULL;
	CMindMapNodeGroup				*group = NULL;
    QList<CMindMapNodeGroup*>		groups;
    QList<int>						groupMarkers;

	// make sure the length matches
	if (m_delNodes.length() != m_indexes.length())
		return;

	// re-add the nodes in the anti order.
	for (int i = m_delNodes.length() - 1; i >= 0; --i)
	{
		node = m_delNodes.at(i);
		index = m_indexes.at(i);
		parentNode = m_parentNodes.at(i);
		m_d->insertItem(node, node->pos(), false);
		if (parentNode)
			parentNode->addChild(node, index);

		// for all the recorded groups, we restore the status
		// by using the markers
        groups = m_groups.at(i);
		groupMarkers = m_groupMarkers.at(i);

		// loop from end to the start
		for (int j = groups.length() - 1; j >= 0; --j)
		{
			group = groups.at(j);
            ref = groupMarkers[j];
            if (ref == -2) // readded
			{
                group->setNode(node);
				parentNode->addNodeGroup(group);
				m_d->insertItem( group, group->pos(), false );
                group->setStartIndex(index);
                group->setEndIndex(index);
			}
			else
			{
                group->onAddChild(index, ref);
			}
        }
		parentNode->onStructChanged();
	}
}

CMindMapReparentCommand::CMindMapReparentCommand(CDiagram *d,
											     CMindMapNode *node,
											     CMindMapNode *newParent,
                                                 int           index,
											     QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_node(node),
	  m_oldParent(NULL),
	  m_newParent(newParent),
      m_newIndex(index),
	  m_oldIndex(-1)
{
	m_oldParent = m_node->upLevelNode();
}

void CMindMapReparentCommand::redo()
{
	CMindMapNodeGroup				*group = NULL;
    QList<CMindMapNodeGroup*>		groups;

	if (!m_node || !m_newParent)
		return;

	m_groups.clear();
	m_groupMarkers.clear();
	if (m_oldParent)
	{
		m_oldIndex = m_node->index();
		m_oldParent->dropChild(m_node);

        groups = m_oldParent->groups();
        foreach (group, groups)
        {
            int refIndex = -1;

            m_groups.push_back(group);
            refIndex = group->onDeleteChild(m_oldIndex);
            if (group->isEmpty())
            {
                m_oldParent->dropNodeGroup(group);
                m_d->deleteItem(group);
                m_groupMarkers.push_back(-2);
            }
            else
            {
                m_groupMarkers.push_back(refIndex);
            }
        }
		m_oldParent->onStructChanged();
	}

    m_newParent->addChild(m_node, m_newIndex);
    groups = m_newParent->groups();
    foreach (group, groups)
    {
        group->onAddChild(m_newIndex);
    }

	m_newParent->onStructChanged();
}

void CMindMapReparentCommand::undo()
{
    CMindMapNodeGroup				*group = NULL;
    QList<CMindMapNodeGroup*>		groups;

	if (!m_node || !m_newParent)
		return;

	m_newParent->dropChild(m_node);
    groups = m_newParent->groups();
    foreach (group, groups)
    {
        group->onDeleteChild(m_newIndex);
    }
	m_newParent->onStructChanged();

	if (m_oldParent)
	{
		m_oldParent->addChild(m_node, m_oldIndex);
        for (int i = m_groups.length() - 1; i >= 0; --i)
        {
            int ref = -1;

            group = m_groups.at(i);
            ref = m_groupMarkers.at(i);
            if (ref == -2)
            {
                group->setNode(m_oldParent);
                m_oldParent->addNodeGroup(group);
                m_d->insertItem(group, group->pos(), false);
                group->setStartIndex(m_oldIndex);
                group->setEndIndex(m_newIndex);
            }
            else
            {
                group->onAddChild(m_oldIndex, ref);
            }
        }

		m_oldParent->onStructChanged();
	}
}

CMindMapReorderCommand::CMindMapReorderCommand(CDiagram *d,
                                               CMindMapNode *node,
                                               int oldIndex,
                                               int newIndex,
                                               QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_node(node),
      m_oldIndex(oldIndex),
      m_newIndex(newIndex)
{

}

void CMindMapReorderCommand::redo()
{
    CMindMapNode					*parentNode = NULL;
	CMindMapNodeGroup				*group = NULL;
    QList<CMindMapNodeGroup*>		groups;

    if (!m_node)
        return;

	parentNode = m_node->upLevelNode();
	if (!parentNode)
		return;

	m_groups.clear();
	m_groupStartIndexes.clear();
	m_groupEndIndexes.clear();

	groups = parentNode->groups();
	foreach (group, groups)
	{
		int		refIndex = -1;
		m_groups.push_back(group);
		m_groupStartIndexes.push_back( group->startIndex() );
		m_groupEndIndexes.push_back( group->endIndex() );

		group->onReorderChild(m_oldIndex, m_newIndex);
	}
    
    parentNode->reorder(m_node, m_newIndex);
}

void CMindMapReorderCommand::undo()
{
    CMindMapNode				*p = NULL;
	CMindMapNodeGroup			*group = NULL;

    if (!m_node)
        return;

    p = m_node->upLevelNode();
	for (int i = 0; i < m_groups.length(); ++i)
	{
		group = m_groups.at(i);
        group->setStartIndex( m_groupStartIndexes.at(i) );
        group->setEndIndex( m_groupEndIndexes.at(i) );
	}

    p->reorder(m_node, m_oldIndex);
}

CMindMapInsertParentCommand::CMindMapInsertParentCommand(CDiagram *d,
											     CMindMapNode *childNode,
												 CMindMapNode *newParentNode,
											     QUndoCommand *parent)
	: CDiagramCommand(d, parent),
	  m_childNode(childNode),
	  m_oldParentNode(NULL),
	  m_newParentNode(newParentNode),
	  m_oldIndex(-1)
{
	m_oldParentNode = m_childNode->upLevelNode();
}

void CMindMapInsertParentCommand::redo()
{
	if (!m_childNode || !m_newParentNode || !m_oldParentNode)
		return;

	m_oldIndex = m_oldParentNode->indexOf(m_childNode);
	m_oldParentNode->dropChild(m_childNode);

	m_d->insertItem(m_newParentNode, m_newParentNode->pos());
	m_oldParentNode->addChild(m_newParentNode, m_oldIndex);
	m_newParentNode->addChild(m_childNode);
}

void CMindMapInsertParentCommand::undo()
{
	if (!m_childNode || !m_newParentNode || !m_oldParentNode)
		return;

	m_newParentNode->dropChild(m_childNode);
	m_oldParentNode->dropChild(m_newParentNode);
	m_oldParentNode->addChild(m_childNode, m_oldIndex);

	m_d->deleteItem(m_newParentNode);
}

CMindMapAddGroupCommand::CMindMapAddGroupCommand(CDiagram *d,
                                                 CMindMapNode *node,
                                                 int startIndex,
                                                 int endIndex,
                                                 int groupType,
                                                 int styleType,
                                                 QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_node(node),
      m_group(NULL),
      m_startIndex(startIndex),
      m_endIndex(endIndex),
      m_groupType(groupType),
      m_styleType(styleType)
{

}

void CMindMapAddGroupCommand::redo()
{
    if (!m_d)
        return;

    switch (m_groupType)
    {
    case CMindMapNode::Boundary:
        {
            CMindMapNodeBoundary    *group = NULL;
            group = new CMindMapNodeBoundary(m_d, m_node);
            group->setBoundaryType( m_styleType );
            m_group = group;
            break;
        }

    case CMindMapNode::Summary:
        m_group = new CMindMapNodeSummary(m_d, m_node);
        break;

    default:
        break;

    }; // End of switch

    m_group->setStartIndex(m_startIndex);
    m_group->setEndIndex(m_endIndex);
    m_group->setNode(m_node);
    m_d->insertItem(m_group, QPointF(0, 0));
    m_node->addNodeGroup( m_group );
    m_d->clearSelection();
    m_group->setSelected(true);
}

void CMindMapAddGroupCommand::undo()
{
    if (!m_group || !m_d)
        return;

    m_node->dropNodeGroup( m_group );
    m_d->deleteItem(m_group);
    SAFE_DELETE(m_group);
    m_d->clearSelection();
}

CMindMapDropGroupCommand::CMindMapDropGroupCommand(CDiagram *d,
                                                   CMindMapNodeGroup *group,
                                                   QUndoCommand *parent)
    : CDiagramCommand(d, parent)
{
    if (group)
    {
        m_groups.push_back( group );
        m_nodes.push_back( group->node() );
    }
}

CMindMapDropGroupCommand::CMindMapDropGroupCommand(CDiagram *d,
                                                   QList<CMindMapNodeGroup*> &groups,
                                                   QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_groups(groups)
{
    foreach (CMindMapNodeGroup *group, groups)
    {
        m_nodes.push_back( group->node() );
    }
}

void CMindMapDropGroupCommand::undo()
{
    int                 type;
    CMindMapNode        *node = NULL;
    CMindMapNodeGroup   *group = NULL;

    // first undo
    QUndoCommand::undo();

    if (m_groups.isEmpty() ||
        m_nodes.isEmpty() ||
        m_groups.length() != m_nodes.length() )
        return;

    for (int i = 0; i < m_groups.length(); ++i)
    {
        group = m_groups.at(i);
        node = m_nodes.at(i);
        type = group->groupType();
        switch(type)
        {
        case CMindMapNode::Boundary:
            {
                m_d->insertItem(group, group->pos(), false);
                node->addNodeGroup(group);
                break;
            }

        case CMindMapNode::Summary:
            {
                break;
            }
        default:
            break;
        }; // End of swith
    }
}

void CMindMapDropGroupCommand::redo()
{
    int                 type;
    CMindMapNode        *node = NULL;
    CMindMapNodeGroup   *group = NULL;

    if (m_groups.isEmpty() ||
        m_nodes.isEmpty() ||
        m_groups.length() != m_nodes.length() )
        return;

    for (int i = 0; i < m_groups.length(); ++i)
    {
        group = m_groups.at(i);
        node = m_nodes.at(i);
        type = group->groupType();
        switch(type)
        {
        case CMindMapNode::Boundary:
            {
                node->dropNodeGroup(group);
                group->setNode(NULL);
                m_d->deleteItem(group);
                break;
            }

        case CMindMapNode::Summary:
            {
                break;
            }

        default:
            break;
        }; // End of swith
    }

    // later redo
    QUndoCommand::redo();
}

CMindMapAddSpriteCommand::CMindMapAddSpriteCommand(CDiagram *d,
                                                   QList<CMindMapNode *> &nodes,
                                                   int spriteType,
                                                   QVariant val1,
                                                   QVariant val2,
                                                   QVariant val3,
                                                   QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_nodes(nodes),
      m_spriteType(spriteType),
      m_val1(val1),
      m_val2(val2),
      m_val3(val3)
{

}

void CMindMapAddSpriteCommand::redo()
{
    CMindMapNode                    *node = NULL;
    CMindMapNodeSprite              *sprite = NULL;
    CMindMapIconProvider            *iconProvider = NULL;
    QList<CMindMapNodeSprite*>      sprites;

	m_val1Records.clear();
	m_val2Records.clear();
	m_val3Records.clear();
	m_replaceMarks.clear();
	m_sprites.clear();

    for (int i = 0; i < m_nodes.length(); ++i)
    {
        node = m_nodes.at(i);
        switch (m_spriteType)
        {
        case CMindMapNode::Pixmap:
		{
			QString					filePath;
			bool					replaced = false;
			CMindMapPixmapSprite	*pixmapSprite = NULL;

			filePath = m_val1.value<QString>();
			sprites = node->sprites( CMindMapNode::Pixmap );
			if (sprites.length() > 0)
			{
				replaced = true;
				pixmapSprite = dynamic_cast<CMindMapPixmapSprite*>(sprites.first());
				if (pixmapSprite)
				{
					m_val1Records.push_back( QVariant(filePath) );
					m_sprites.push_back(pixmapSprite);
					pixmapSprite->setPixmapPath(filePath);
				}
			}

            if (!replaced)
            {
                pixmapSprite = new CMindMapPixmapSprite(node);
				pixmapSprite->setPixmapPath(filePath);
                m_sprites.push_back(pixmapSprite);
				node->addSprite(pixmapSprite);
            }
            m_replaceMarks.push_back(replaced);
            break;
		}

        case CMindMapNode::Icon:
        {
            QIcon       icon;
            int         index;
            bool        replaced = false;

            icon = m_val1.value<QIcon>();
            index = m_val2.value<int>();
            iconProvider = CMindMapIconProvider::instance();
            sprites = node->sprites( CMindMapNode::Icon );
            foreach (sprite, sprites)
            {
                CMindMapIconSprite *iconSprite = NULL;

                iconSprite = dynamic_cast<CMindMapIconSprite*>(sprite);
                if (iconSprite)
                {
                    if ( iconProvider->checkConflict( iconSprite->iconIndex(), index ) )
                    {
                        m_val1Records.push_back( QVariant(iconSprite->icon()) );
                        m_val2Records.push_back( QVariant(iconSprite->iconIndex()));
                        m_sprites.push_back(iconSprite);
                        iconSprite->setIcon(icon);
                        iconSprite->setIconIndex(index);
                        replaced = true;
                        break;
                    }
                }
            }

            if (!replaced)
            {
                sprite = new CMindMapIconSprite(node, icon, index);
                node->addSprite(sprite);
                m_sprites.push_back(sprite);
            }
            m_replaceMarks.push_back(replaced);
            break;
        }

        case CMindMapNode::File:
		{
			QString					filePath;
			bool					added = false;
			CMindMapFileSprite		*fileSprite = NULL;

			filePath = m_val1.value<QString>();
			sprites = node->sprites( CMindMapNode::File );
			if (sprites.length() > 0)
			{
				added = true;
				fileSprite = dynamic_cast<CMindMapFileSprite*>(sprites.first());
				if (fileSprite && fileSprite->addFile(filePath))
				{
					m_sprites.push_back( fileSprite );

					m_val1Records.push_back( QVariant(filePath) );
					m_replaceMarks.push_back(added);
				}
			}

            if (!added)
            {
                fileSprite = new CMindMapFileSprite(node);
				fileSprite->addFile(filePath);
				node->addSprite(fileSprite);

                m_sprites.push_back(fileSprite);
				m_replaceMarks.push_back(added);
            }
            break;
		}

        case CMindMapNode::Note:
        {
            QString					note;
            bool					replaced = false;
            CMindMapNoteSprite      *noteSprite = NULL;

            note = m_val1.value<QString>();
            sprites = node->sprites( CMindMapNode::Note );
            if (sprites.length() > 0)
            {
                replaced = true;
                noteSprite = dynamic_cast<CMindMapNoteSprite*>(sprites.first());
                if (noteSprite)
                {
                    m_val1Records.push_back( QVariant(noteSprite->note()) );
                    m_sprites.push_back(noteSprite);
                    noteSprite->setNote(note);
                }
            }

            if (!replaced)
            {
                noteSprite = new CMindMapNoteSprite(node);
                noteSprite->setNote(note);
                m_sprites.push_back(noteSprite);
                node->addSprite(noteSprite);
            }
            m_replaceMarks.push_back(replaced);
            break;
        }

        case CMindMapNode::Link:
        {
            int                     linkType;
            QUrl                    linkAddress;
            bool					replaced = false;
            CMindMapLinkSprite      *linkSprite = NULL;

            linkType = m_val1.value<int>();
            linkAddress = m_val2.value<QString>();
            sprites = node->sprites( CMindMapNode::Link );
            if (sprites.length() > 0)
            {
                replaced = true;
                linkSprite = dynamic_cast<CMindMapLinkSprite*>(sprites.first());
                m_val1Records.push_back( QVariant(linkSprite->linkType()) );
                m_val2Records.push_back( QVariant(linkSprite->link()));
                m_sprites.push_back(linkSprite);
                linkSprite->setLink( linkAddress );
                linkSprite->setLinkType( linkType );
            }

            if (!replaced)
            {
                linkSprite = new CMindMapLinkSprite(node);
                linkSprite->setLink( linkAddress );
                linkSprite->setLinkType( linkType );
                m_sprites.push_back(linkSprite);
                node->addSprite(linkSprite);
            }
            m_replaceMarks.push_back(replaced);
            break;
        }

        case CMindMapNode::Task:
        {
            int                     dataType = -1;
            int                     priority = 0;
            int                     progress = 0;
            QString                 assignedTo;
            QDateTime               dueDate;
            bool					replaced = false;
            CMindMapTaskSprite      *taskSprite = NULL;

            sprites = node->sprites( CMindMapNode::Task );
            if (sprites.length() > 0)
            {
                replaced = true;
                taskSprite = dynamic_cast<CMindMapTaskSprite*>(sprites.first());
            }
            else
            {
                taskSprite = new CMindMapTaskSprite(node);
                node->addSprite(taskSprite);
            }

            dataType = m_val1.value<int>();
            switch(dataType)
            {
            case CMindMapTaskSprite::Assignee:
                if (replaced)
                    m_val1Records.push_back( QVariant(taskSprite->assignedTo()) );
                assignedTo = m_val2.value<QString>();
                taskSprite->setAssignedTo(assignedTo);
                break;

            case CMindMapTaskSprite::DueDate:
                if (replaced)
                    m_val1Records.push_back( QVariant(taskSprite->dueDate()) );
                dueDate = m_val2.value<QDateTime>();
                taskSprite->setDueDate(dueDate);
                break;

            default:
                break;

            }; // End of switch

            // if task sprite is null now, then we should remove it
            if (taskSprite->isNull())
            {
                node->dropSprite( taskSprite );
                m_d->removeItem( taskSprite );
            }
            m_replaceMarks.push_back(replaced);
            m_sprites.push_back(taskSprite);

            break;
        }

		case CMindMapNode::TaskPriority:
		{
            int         priority;
            bool        replaced = false;

            priority = m_val1.value<int>();
            sprites = node->sprites( CMindMapNode::TaskPriority );
            foreach (sprite, sprites)
            {
                CMindMapTaskPrioritySprite *prioritySprite = NULL;

                prioritySprite = dynamic_cast<CMindMapTaskPrioritySprite*>(sprite);
                if (prioritySprite)
                {
                    m_val1Records.push_back( QVariant(prioritySprite->priority()) );
                    m_sprites.push_back(prioritySprite);
                    prioritySprite->setPriority(priority);
                    replaced = true;
                    break;
                }
            }

            if (!replaced)
            {
                sprite = new CMindMapTaskPrioritySprite(node, priority);
                node->addSprite(sprite);
                m_sprites.push_back(sprite);
            }
            m_replaceMarks.push_back(replaced);
			break;
		}

		case CMindMapNode::TaskProgress:
		{
			int         progress;
            bool        replaced = false;

            progress = m_val1.value<int>();
            sprites = node->sprites( CMindMapNode::TaskProgress );
            foreach (sprite, sprites)
            {
                CMindMapTaskProgressSprite *progressSprite = NULL;

                progressSprite = dynamic_cast<CMindMapTaskProgressSprite*>(sprite);
                if (progressSprite)
                {
                    m_val1Records.push_back( QVariant(progressSprite->progress()) );
                    m_sprites.push_back(progressSprite);
                    progressSprite->setProgress(progress);
                    replaced = true;
                    break;
                }
            }

            if (!replaced)
            {
                sprite = new CMindMapTaskProgressSprite(node, progress);
                node->addSprite(sprite);
                m_sprites.push_back(sprite);
            }
            m_replaceMarks.push_back(replaced);
			break;
			break;
		}

        default:
            break;
        }; // End of switch
    }
}

void CMindMapAddSpriteCommand::undo()
{
	int						index = 0;
    bool                    r = false;
    CMindMapNode            *node = NULL;
    CMindMapNodeSprite      *sprite = NULL;

    for (int i = 0; i < m_sprites.length(); ++i)
    {
        node = m_nodes.at(i);
        sprite = m_sprites.at(i);
        r = m_replaceMarks.at(i);
        if (r)
        {
            switch (m_spriteType)
            {
			case CMindMapNode::Pixmap:
			{
				CMindMapPixmapSprite *pixmapSprite = NULL;
				pixmapSprite = dynamic_cast<CMindMapPixmapSprite*>(sprite);
				if (pixmapSprite)
				{
					pixmapSprite->setPixmapPath( m_val1Records.at(index).value<QString>() );
					++index;
				}
				break;
			}

            case CMindMapNode::Icon:
            {
                CMindMapIconSprite  *iconSprite = NULL;
                iconSprite = dynamic_cast<CMindMapIconSprite*>(sprite);
                if (iconSprite)
                {
                    iconSprite->setIcon( m_val1Records.at(index).value<QIcon>() );
                    iconSprite->setIconIndex( m_val2Records.at(index).value<int>() );
					++index;
                }
                break;
            }

            case CMindMapNode::Link:
            {
                CMindMapLinkSprite *linkSprite = NULL;
                linkSprite = dynamic_cast<CMindMapLinkSprite*>(sprite);
                if (linkSprite)
                {
                    linkSprite->setLinkType( m_val1Records.at(index).value<int>() );
                    linkSprite->setLink( m_val2Records.at(index).value<QUrl>() );
                    ++index;
                }
                break;
            }

			case CMindMapNode::File:
			{
				CMindMapFileSprite	*fileSprite = NULL;

				fileSprite = dynamic_cast<CMindMapFileSprite*>(sprite);
                if (fileSprite)
                {
					fileSprite->deleteFile( m_val1Records.at(index).value<QString>() );
					++index;
                }
				break;
			}

            case CMindMapNode::Note:
            {
                CMindMapNoteSprite *noteSprite = NULL;
                noteSprite = dynamic_cast<CMindMapNoteSprite*>(sprite);
                if (noteSprite)
                {
                    noteSprite->setNote( m_val1Records.at(index).value<QString>() );
                    ++index;
                }
                break;
            }

            case CMindMapNode::Task:
            {
                int                 dataType = -1;
                CMindMapTaskSprite  *taskSprite = NULL;

                dataType = m_val1.value<int>();
                taskSprite = dynamic_cast<CMindMapTaskSprite*>(sprite);
                if (taskSprite)
                {
                    // re-added the sprite
                    if (taskSprite->isNull())
                    {
                        m_d->addItem( taskSprite );
                        node->addSprite(taskSprite);
                    }

                    // refill the data
                    switch(dataType)
                    {
                    case CMindMapTaskSprite::Assignee:
                        taskSprite->setAssignedTo( m_val1Records.at(index).value<QString>() );
                        break;

                    case CMindMapTaskSprite::DueDate:
                         taskSprite->setDueDate(m_val1Records.at(index).value<QDateTime>() );
                        break;

                    default:
                        break;
                    }; // End of switch

                    ++index;
                }
                break;
            }

			case CMindMapNode::TaskPriority:
			{
				CMindMapTaskPrioritySprite  *prioritySprite = NULL;
                prioritySprite = dynamic_cast<CMindMapTaskPrioritySprite*>(sprite);
                if (prioritySprite)
                {
                    prioritySprite->setPriority( m_val1Records.at(index).value<int>() );
					++index;
                }
				break;
			}

			case CMindMapNode::TaskProgress:
			{
				CMindMapTaskProgressSprite  *progressSprite = NULL;
                progressSprite = dynamic_cast<CMindMapTaskProgressSprite*>(sprite);
                if (progressSprite)
                {
                    progressSprite->setProgress( m_val1Records.at(index).value<int>() );
					++index;
                }
				break;
			}

            default:
                break;

            }; // End of switch
        }
        else
        {
            node->dropSprite(sprite);
            SAFE_DELETE(sprite);
        }
    }

    m_sprites.clear();
    m_val1Records.clear();
    m_val2Records.clear();
    m_val3Records.clear();
	m_replaceMarks.clear();
}

CMindMapRemoveSpriteCommand::CMindMapRemoveSpriteCommand(CDiagram *d,
                                                         QList<CMindMapNode *> &nodes,
                                                         int spriteType,
                                                         bool ctrl,
                                                         QVariant val,
                                                         QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_nodes(nodes),
      m_spriteType(spriteType),
      m_ctrl(ctrl),
      m_val(val)
{

}

void CMindMapRemoveSpriteCommand::redo()
{
    CMindMapNode                *node = NULL;
    QList<CMindMapNodeSprite*>  sprites;

    m_sprites.clear();
    m_valRecords.clear();
    switch(m_spriteType)
    {
    case CMindMapNode::Icon:
    {
        // drop all icon sprites
        if (m_ctrl)
        {
            foreach (node, m_nodes)
            {
                sprites = node->sprites(m_spriteType);
                node->dropSprites(m_spriteType);
                m_sprites.push_back(sprites);
                foreach (CMindMapNodeSprite *sprite, sprites)
                {
                    m_d->removeItem(sprite);
                }
            }
        }
        else // only the specified one
        {
            int                 iconIndex = -1;
            CMindMapIconSprite  *iconSprite = NULL;

            iconIndex = m_val.toInt();
            foreach (node, m_nodes)
            {
                QList<CMindMapNodeSprite*>  iconSprites;

                sprites = node->sprites(m_spriteType);
                foreach (CMindMapNodeSprite *s, sprites)
                {
                    iconSprite = dynamic_cast<CMindMapIconSprite*>(s);
                    if (iconSprite && iconSprite->iconIndex() == iconIndex)
                    {
                        node->dropSprite(iconSprite);
                        m_d->removeItem(iconSprite);
                        iconSprites.push_back(iconSprite);
                        break;
                    }
                }
                m_sprites.push_back(iconSprites);
            }
        }
        break;
    }

    case CMindMapNode::File:
    {
        // drop the file sprite
        if (m_ctrl)
        {
            foreach (node, m_nodes)
            {
                sprites = node->sprites(m_spriteType);
                if (sprites.length() > 0)
                {
                    m_d->removeItem(sprites.first());
                }
                node->dropSprites(m_spriteType);
                m_sprites.push_back(sprites);
            }
        }
        else // only drop files
        {
            QStringList             files;
            CMindMapFileSprite      *fileSprite = NULL;

            files = m_val.toStringList();
            foreach (node, m_nodes)
            {
                sprites = node->sprites(m_spriteType);
                if (sprites.length() > 0)
                {
                    fileSprite = dynamic_cast<CMindMapFileSprite*>(sprites.first());
                    foreach (QString file, files)
                    {
                        fileSprite->deleteFile(file);
                    }
                }
                m_valRecords.push_back(files);
                m_sprites.push_back(sprites);
            }
        }
    }
        break;

    case CMindMapNode::Note:
    case CMindMapNode::Link:
    case CMindMapNode::Pixmap:
    case CMindMapNode::Task:
	case CMindMapNode::TaskPriority:
	case CMindMapNode::TaskProgress:
    {
        foreach (node, m_nodes)
        {
            sprites = node->sprites(m_spriteType);
            if (sprites.length() > 0)
                m_d->removeItem(sprites.first());
            node->dropSprites(m_spriteType);
            m_sprites.push_back(sprites);
        }
        break;
    }

    default:
        break;
    }; // End of switch
}

void CMindMapRemoveSpriteCommand::undo()
{
    CMindMapNode                *node = NULL;
    CMindMapNodeSprite          *sprite = NULL;
    QList<CMindMapNodeSprite*>  sprites;

    switch(m_spriteType)
    {
    case CMindMapNode::File:
    {
        if (m_ctrl)
        {
            for (int i = 0; i < m_nodes.length(); ++i)
            {
                node = m_nodes.at(i);
                sprites = m_sprites.at(i);
                foreach (sprite, sprites)
                {
                    m_d->addItem(sprite);
                    node->addSprite(sprite);
                }
            }
        }
        else
        {
            QStringList         files;
            CMindMapFileSprite  *fileSprite = NULL;

            for (int i = 0; i < m_nodes.length(); ++i)
            {
                node = m_nodes.at(i);
                sprites = node->sprites(m_spriteType);
                if (sprites.length() > 0)
                    fileSprite = dynamic_cast<CMindMapFileSprite*>(sprites.first());

                if (fileSprite)
                {
                    files = m_valRecords.at(i).toStringList();
                    foreach (QString file, files)
                    {
                        fileSprite->addFile(file);
                    }
                }
            }
        }
        break;
    }

    default:
        for (int i = 0; i < m_nodes.length(); ++i)
        {
            node = m_nodes.at(i);
            sprites = m_sprites.at(i);
            foreach (sprite, sprites)
            {
                m_d->addItem(sprite);
                node->addSprite(sprite);
            }
        }

        break;
    }; // End of switch
}

CMindMapNodeSpriteResizeCommand::CMindMapNodeSpriteResizeCommand(CDiagram *d,
                                                                 CMindMapNodeSprite *sprite,
                                                                 QSizeF oldSize,
                                                                 QSizeF newSize,
                                                                 QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_sprite(sprite),
      m_oldSize(oldSize),
      m_newSize(newSize)
{

}

void CMindMapNodeSpriteResizeCommand::redo()
{
    if (m_sprite)
    {
        m_sprite->setSize( m_newSize );
    }
}

void CMindMapNodeSpriteResizeCommand::undo()
{
    if (m_sprite)
    {
        m_sprite->setSize(m_oldSize);
    }
}

CMindMapPasteCommand::CMindMapPasteCommand(CDiagram *d,
                                           QList<CMindMapNode *> &nodes,
                                           QUndoCommand *parent)
    : CDiagramCommand(d, parent),
      m_nodes(nodes)
{
    CDiagramClipboard	*clipboard = NULL;

    clipboard = CDiagramClipboard::clipboard();
    if (clipboard)
    {
        m_image = clipboard->image(CDiagramClipboard::Global);
        m_pixmap = clipboard->pixmap(CDiagramClipboard::Global);
        m_text = clipboard->text(CDiagramClipboard::Application);
        if (m_text.isNull() || m_text.isEmpty())
            m_text = clipboard->text(CDiagramClipboard::Global);
        clipboard->clear(CDiagramClipboard::All);
    }
}

void CMindMapPasteCommand::redo()
{
    QDomDocument                doc;
    CMindMapNode                *node = NULL;
    QList<CMindMapNode*>        topNewNodes;

    if (m_nodes.length() < 1)
    {
        return;
    }

    m_newNodes.clear();
    if (doc.setContent(m_text))
    {
        foreach (node, m_nodes)
        {
            int                         id;
            QDomNode                    xmlNode;
            QDomNodeList                xmlNodelist;
            QDomElement                 xmlElement;
            QString                     idString, category, name;
            CMindMapNode                *newNode = NULL;
            QMap<int, CMindMapNode*>    nodesMap;

            xmlNodelist = doc.elementsByTagName(QLatin1String("item"));
            for (int i = 0; i < xmlNodelist.count(); ++i)
            {
                xmlNode = xmlNodelist.at(i);
                if (!xmlNode.isNull() && xmlNode.isElement())
                {
                    xmlElement = xmlNode.toElement();
                    idString = xmlElement.attribute(QLatin1String("id"));
                    category = xmlElement.attribute(QLatin1String("category"));
                    name = xmlElement.attribute(QLatin1String("name"));

                    if (category == NODE_CATEGORY &&
                        name == NODE_NAME)
                    {
                        newNode = new CMindMapNode(m_d);
                        m_d->insertItem(newNode, newNode->pos());
                        id = newNode->id();
                        newNode->fromXml(xmlElement);
                        newNode->setId(id);

                        nodesMap.insert(idString.toInt(), newNode);
                        topNewNodes.push_back(newNode);
                        m_newNodes.push_back(newNode);
                    }
                }
            }

            // reconstruct the connection between nodes
            foreach (newNode, nodesMap.values())
            {
                int             origionChildsCount = 0;
                int             newChildsCount = 0;
                CMindMapNode    *tmpNode = NULL;
                QList<int>      childIds;

                childIds = newNode->childsIdArray();
                origionChildsCount = childIds.count();
                foreach (int i, childIds)
                {
                    if (nodesMap.contains(i))
                    {
                        tmpNode = nodesMap[i];
                        newNode->addChild(tmpNode);
                        ++ newChildsCount;
                        topNewNodes.removeAll(tmpNode);
                    }
                }

                // check whether we should drop the group
                if (origionChildsCount != newChildsCount)
                {
                    newNode->dropAllNodeGroups();
                }
            }

            foreach (newNode, topNewNodes)
            {
                node->addChild(newNode);
            }
        }
    }
    else
    {
        foreach (node, m_nodes)
        {
            if (!m_text.trimmed().isEmpty())
            {
                // TODO
                continue;
            }

            if (!m_image.isNull())
            {
                // TODO
                continue;
            }

            if (!m_pixmap.isNull())
            {
                CMindMapPixmapSprite    *sprite = NULL;

                node = new CMindMapNode(m_d);
                sprite = new CMindMapPixmapSprite(node);
                sprite->setPixmap(m_pixmap);
                node->addSprite(sprite);

                m_d->insertItem(node, QPointF(0, 0));
                m_newNodes.push_back(node);

                node->addChild(node);

                continue;
            }
        }
    }
}

void CMindMapPasteCommand::undo()
{
    CMindMapNode    *node = NULL;
    CMindMapNode    *upLevelNode = NULL;

    if (m_newNodes.length() > 1)
        qStableSort(m_newNodes.begin(), m_newNodes.end(), levelGreaterThan);

    foreach (node, m_newNodes)
    {
        upLevelNode = node->upLevelNode();
        if (upLevelNode)
            upLevelNode->dropChild(node);
        node->setUpLevelNode(NULL);
        SAFE_DELETE(node);
    }

    m_newNodes.clear();
}
