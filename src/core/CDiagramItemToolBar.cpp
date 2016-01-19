/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramItemToolBar>

CDiagramItemToolBar::CDiagramItemToolBar()
	: CDiagramToolBar(NULL)
{
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, false);

	setVisible(false);
}

CDiagramItemToolBar::~CDiagramItemToolBar()
{
	
}

void CDiagramItemToolBar::onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect)
{
    CDiagramHostedWidget        *widget = NULL;

    widget = getPreferredHostedWidget();
    setHostedWidget( widget );
    if (widget && items.length() > 0)
	{
		updatePosViaGeometryChange(rect);
		updateVisibility(true);
	}
	else
	{
		updateVisibility(false);
	}
}

void CDiagramItemToolBar::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	setOpacity(1.0);
}

void CDiagramItemToolBar::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	bool					fakeout = true;
	QGraphicsItem			*item = NULL;
	QGraphicsProxyWidget	*proxy = NULL;
	QList<QGraphicsItem*>	items;

	items = m_proxy->childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		item = items.at(i);
		if (item->isVisible() && item->type() == QGraphicsProxyWidget::Type)
		{
			if (item->isUnderMouse())
			{
				fakeout = false;
				break;
			}
		}
	}

    if (fakeout && anchorType() == CDiagramWidget::AnchorToSelectedItem )
		setOpacity(0.5);
}

void CDiagramItemToolBar::onAnchorTypeChanged(CDiagramWidget::AnchorType value)
{
    QRectF		r;

	if (m_d)
		r = m_d->getItemsBoundingRect(true);
	setAnchorType(value);
	onSelectedItemsGeometryChanged(r);

    if (anchorType() != CDiagramWidget::AnchorToSelectedItem)
		setOpacity(1.0);
}

void CDiagramItemToolBar::onAutoHide(bool value)
{
	setAutoHide(value);
}