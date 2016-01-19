/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramCommands>
#include <core/CDiagramContextMenuHandler>

CDiagramContextMenu::CDiagramContextMenu()
    : QObject(NULL)
{

}

CDiagramContextMenu::~CDiagramContextMenu()
{
}

bool CDiagramContextMenu::support(CDiagram *d)
{
    return true;
}

void CDiagramContextMenu::reset(CDiagram *d)
{
    // TODO
}

void CDiagramContextMenu::apply(QMenu *menu)
{
    // TODO
}

CDiagramContextMenuHandler::CDiagramContextMenuHandler()
    : CDiagramEventHandler()
{
    m_name = "DiagramContextMenuHandler";
}

CDiagramContextMenuHandler::~CDiagramContextMenuHandler()
{

}

bool CDiagramContextMenuHandler::onEvent(CDiagramPrivateData *p, QEvent *evt)
{
    QMenu                           *menu = NULL;
    bool							r = false;
    QGraphicsItem                   *item = NULL;
    QList<QGraphicsItem*>           items;
    CDiagram						*d = NULL;
	QGraphicsSceneContextMenuEvent	*event = NULL;

	if (!p)
		return r;

	d = p->m_d;
	switch(evt->type())
	{
	case QEvent::GraphicsSceneContextMenu:
		event = static_cast<QGraphicsSceneContextMenuEvent*>(evt);
    items = d->items( event->scenePos());
    foreach (item, items)
    {
        if (item)
        {
            d->sendEvent(item, event);
            break;
        }
    }

    if (!event->isAccepted())
		{
        menu = d->contextMenu(d->mode());
        if (!menu)
        {
            r = triggerMenu(d, event->screenPos());
        }
        else
        {
            menu->exec(event->screenPos());
            r = true;
        }
    }
    else
        {
            item->setSelected(true);
        }
		break;

	default:
		break;
	}; // End of switch

	return r;
}

void CDiagramContextMenuHandler::addContextMenu(CDiagram::DiagramMode mode, CDiagramContextMenu *menu)
{
    if (!menu)
        return;

    if (!m_menus.contains(mode))
        m_menus[mode] = QList<CDiagramContextMenu*>();

    if (!m_menus[mode].contains(menu))
        m_menus[mode].append( menu );
}

QList<CDiagramContextMenu*> CDiagramContextMenuHandler::getContextMenus(CDiagram::DiagramMode mode)
{
    QList<CDiagramContextMenu*>     r;

    if (m_menus.contains(mode))
    {
        r = m_menus[mode];
    }

    return r;
}

bool CDiagramContextMenuHandler::triggerMenu(CDiagram *d, const QPoint &p)
{
    bool                        r = false;
    QMenu                       hostMenu;
    CDiagram::DiagramMode       mode;
    CDiagramContextMenu         *hostedMenu = NULL;
    QList<CDiagramContextMenu*> menus;

    mode = d->mode();
    if (m_menus.contains(mode))
        menus = m_menus[mode];

    if (menus.length() > 0)
    {
        for (int i = 0; i < menus.length(); ++i)
        {
            hostedMenu = menus.at(i);
            if (hostedMenu->support(d))
            {
                hostedMenu->reset(d);
                hostedMenu->apply(&hostMenu);
            }
        }

        if (!hostMenu.isEmpty())
        {
            hostMenu.exec(p);
            r = true;
        }
    }

    return r;
}
