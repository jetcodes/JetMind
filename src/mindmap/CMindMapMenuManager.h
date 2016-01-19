/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPMENUMANAGER_H
#define PRODRAW_CMINDMAPMENUMANAGER_H

#include <QtGui>

class CMindMapNodeSprite;
class CMindMapNodeSpriteMenu;
class CMindMapNodeGroup;
class CMindMapNodeGroupMenu;
class CMindMapMenuManager;

class CMindMapNodeSpriteMenu
{
public:
    virtual void reset(CMindMapNodeSprite* sprite);
}; // End of class CMindMapNodeSpriteMenu

class CMindMapNodeGroupMenu
{
public:
    virtual void reset(CMindMapNodeGroup *group);
}; // End of class CMindMapNodeGroupMenu

class CMindMapMenuManager : public QObject
{
	Q_OBJECT

public:
	void setNodeSpriteMenu(int type, CMindMapNodeSpriteMenu *menu);
	CMindMapNodeSpriteMenu* nodeSpriteMenu(int type);

    void setNodeGroupMenu(int type, CMindMapNodeGroupMenu *menu);
    CMindMapNodeGroupMenu* nodeGroupMenu(int type);

	static CMindMapMenuManager* instance()
	{
		if (!m_inst)
			m_inst = new CMindMapMenuManager();
		return m_inst;
	}

private:
    Q_DISABLE_COPY(CMindMapMenuManager)
	CMindMapMenuManager() {}
	
	QMap<int, CMindMapNodeSpriteMenu*>	m_nodeSpriteMenus;
    QMap<int, CMindMapNodeGroupMenu*>   m_nodeGroupMenus;
	static CMindMapMenuManager			*m_inst;

}; // End of class CMindMapMenuManager

#endif // PRODRAW_CMINDMAPMENUMANAGER_H
