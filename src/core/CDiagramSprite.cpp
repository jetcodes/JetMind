/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramSprite>

CDiagramSprite::CDiagramSprite(CDiagram *d)
	: m_d(d),
	  m_priority(0)
{
	
}

int CDiagramSprite::spriteType()
{
	return -1;
}

void CDiagramSprite::setPriority(int value)
{
	m_priority = value;
}

int CDiagramSprite::priority()
{
	return m_priority;
}

bool CDiagramSprite::support(const QList<CDiagramItem*> &items)
{
	return true;
}

void CDiagramSprite::init(CDiagram *d, QList<CDiagramItem*> &items, const QRectF &rect)
{
}

void CDiagramSprite::uninit(CDiagram *d)
{
}
