/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPNODEHANDLER_H
#define PRODRAW_CMINDMAPNODEHANDLER_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CDiagramItem>
#include <core/CDiagramSprite>
#include <core/CDiagramItemHandler>

class CDiagram;
class CDiagramNode;
class CMindMapNodeHandler;

class CMindMapNodeHandler: public CDiagramItemHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapNodeHandler();
	virtual ~CMindMapNodeHandler();

	virtual int spriteType();
	virtual bool support(const QList<CDiagramItem*> &items);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	
protected:
	void initDraggers();

}; // End of class CMindMapNodeHandler

class CMindMapGroupHandler: public CMindMapNodeHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapGroupHandler();
	virtual ~CMindMapGroupHandler();

	virtual int spriteType();

}; // End of class CMindMapGroupHandler
#endif // PRODRAW_CMINDMAPNODEHANDLER_H
