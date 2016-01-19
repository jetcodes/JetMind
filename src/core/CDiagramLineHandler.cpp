/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <limits>
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramLine>
#include <core/CDiagramItemHandler>
#include <core/CDiagramLineHandler>

CDiagramLineHandler::CDiagramLineHandler()
	: CDiagramItemHandler()
{
	setVisible(false);
}

void CDiagramLineHandler::initDraggers()
{
	int						i = 0;
	CDiagramItemHandlerDragger	*dragger;

	for (i=0; i<2; ++i)
	{
		dragger = new CDiagramItemHandlerDragger(this, i, m_draggerSize);
		dragger->setVisible(false);
		dragger->setParentItem(this);
		m_draggers.append(dragger);
	}
}

void CDiagramLineHandler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget /* = 0 */)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	Q_UNUSED(painter);
}

void CDiagramLineHandler::layoutDraggers()
{
	update();
}

void CDiagramLineHandler::startDragging(CDiagramItemHandlerDragger *dragger)
{
	if ( !scene() || !m_d)
		return;
	m_dragger = dragger;
}

void CDiagramLineHandler::doDragging(CDiagramItemHandlerDragger *dragger)
{
	if (!scene() || !m_d)
		return;
}

void CDiagramLineHandler::finishDragging(CDiagramItemHandlerDragger *dragger)
{
	int		index = 0;
	
	if (!scene() || !m_d)
		return;

	m_dragger = NULL;
}

bool CDiagramLineHandler::support(QList<CDiagramItem *> &items)
{
	bool		r = false;

	if (items.length() == 1)
		r = (NULL != dynamic_cast<CDiagramLine*>(items.first()));
	return r;
}