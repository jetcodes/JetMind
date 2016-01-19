/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMSPRITE_H
#define PRODRAW_CDIAGRAMSPRITE_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramItem;

class CORE_EXPORT CDiagramSprite: public QGraphicsObject
{
public:
	CDiagramSprite(CDiagram *d = NULL);
	
	// Sprite type, intended to be used as the identification of
	// a sprite
	enum SpriteType { CustomType = 0x10000 };
	virtual int spriteType();

	void setPriority(int value);
	int priority();

	// used to check whether the sprite supports current selected items
	virtual bool support(const QList<CDiagramItem*> &items);

	// used to initialize/uninitialize the sprites
	virtual void init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect);
	virtual void uninit(CDiagram *d);

protected:
	CDiagram		*m_d;
	int				m_priority;

}; // End of class CDiagramSprite

#endif // PRODRAW_CDIAGRAMSPRITE_H