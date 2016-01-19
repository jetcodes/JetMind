/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMITEMGROUPHANDLER_H
#define PRODRAW_CDIAGRAMITEMGROUPHANDLER_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramNodeHandler>

class CDiagram;
class CDiagramItem;
class CDiagramGroupHandler;

/**
 * CDiagramGroupHandler is the box surrounding the nodes, providing the resize, rotation
 * ability for the embeded nodes via serveral ctrl buttons.
 */
class CORE_EXPORT CDiagramGroupHandler : public CDiagramNodeHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramGroupHandler();
	virtual ~CDiagramGroupHandler() {}

	enum { SpriteType = CustomType + 4 };
	virtual int spriteType() { return SpriteType; }

	bool support(const QList<CDiagramItem*> &items);
	void finishDragging(CDiagramItemHandlerDragger *g);

protected:
	virtual void updateGeometry(const QRectF &rect);
	virtual void updateItemsGeometry(qreal xratio, qreal yratio, QPointF &op, QList<QRectF> &oldGeometry, QList<QRectF> &newGeometry);

	friend class CDiagram;

private:
	Q_DISABLE_COPY(CDiagramGroupHandler);

}; // end of class CDiagramGroupHandler.

#endif // PRODRAW_CDIAGRAMITEMGROUPHANDLER_H
