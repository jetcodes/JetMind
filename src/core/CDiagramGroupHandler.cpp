/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <limits>
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramGroup>
#include <core/CDiagramGroupHandler>

CDiagramGroupHandler::CDiagramGroupHandler()
: CDiagramNodeHandler()
{
}

bool CDiagramGroupHandler::support(const QList<CDiagramItem*> &items)
{
	bool	r = false;

	if (items.length() > 1 || (items.length() == 1 && dynamic_cast<CDiagramGroup*>(items.first()) ) )
	{
		r = true;
	}
	return r;
}

void CDiagramGroupHandler::finishDragging(CDiagramItemHandlerDragger *g)
{
	/*
	QRectF			r, rect;
	CDiagramItem	*item = NULL;

	foreach (item, m_d->selectedTopLevelItems())
	{
		rect = QRectF();
		rect = item->mapRectToScene( item->boundingRect() );
		r |= rect;
	}
	updateGeometry(r);
	*/
	m_dragger = NULL;
}

void CDiagramGroupHandler::updateGeometry(const QRectF &rect)
{
	if (!isChanging())
	{
		setRotation( 0 );
		setGeometry( QRectF( rect.center(), rect.size() ) );
	}
	else
	{
		setGeometry( m_tmpGeometry );
	}
}

void CDiagramGroupHandler::updateItemsGeometry(qreal xratio, qreal yratio, QPointF &op, QList<QRectF> &oldGeometry, QList<QRectF> &newGeometry)
{
	int				i = 0;
	QPointF			t;
	CDiagramItem	*item = NULL;

	foreach (item, m_items)
	{
		if (item->interType() == Global::Node || item->interType() == Global::Group)
		{
			t = item->mapToItem(this, QPointF(0,0) );
			t = QPointF(op.x() + (t.x() - op.x()) * xratio, op.y() + (t.y() - op.y() ) * yratio) ;
			newGeometry.push_back( QRectF(mapToItem(item->parentItem(), t), QSizeF(item->width() * xratio, item->height() * yratio) ) );
			oldGeometry.push_back( item->geometry() );
		}
	}
}