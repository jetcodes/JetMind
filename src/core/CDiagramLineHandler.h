/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMLINEHANDLER_H
#define PRODRAW_CDIAGRAMLINEHANDLER_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramItemHandler>

class CDiagram;
class CDiagramItem;

/**
 * CDiagramLineHandler is the box surrounding the nodes, providing the resize, rotation
 * ability for the embeded nodes via serveral ctrl buttons.
 */
class CORE_EXPORT CDiagramLineHandler : public CDiagramItemHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramLineHandler();
	virtual ~CDiagramLineHandler() {}

	enum { SpriteType = CustomType + 3 };
	virtual int spriteType() { return SpriteType; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	bool support(QList<CDiagramItem*> &items);

protected:
	void initDraggers();
	void layoutDraggers();
	void startDragging(CDiagramItemHandlerDragger *g);
	void doDragging(CDiagramItemHandlerDragger *g);
	void finishDragging(CDiagramItemHandlerDragger *g);

private:
	Q_DISABLE_COPY(CDiagramLineHandler)

	friend class CDiagram;

}; // end of class CDiagramLineHandler.

#endif // PRODRAW_CDIAGRAMLINEHANDLER_H
