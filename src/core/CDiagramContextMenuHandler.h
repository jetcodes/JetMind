/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMCONTEXTMENUHANDLER_H
#define PRODRAW_CDIAGRAMCONTEXTMENUHANDLER_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramContext>
#include <core/CDiagramEventHandler>

class CDiagramPrivateData;
class CDiagramContextMenuHandler;

class CORE_EXPORT CDiagramContextMenu : public QObject
{
    Q_OBJECT

public:
    CDiagramContextMenu();
    virtual ~CDiagramContextMenu();

    virtual bool support(CDiagram *d);
    virtual void reset(CDiagram *d);
    virtual void apply(QMenu *menu);

}; // End of class CDiagramContextMenu

class CORE_EXPORT CDiagramContextMenuHandler : public CDiagramEventHandler
{
public:
	CDiagramContextMenuHandler();
	virtual ~CDiagramContextMenuHandler();

	bool onEvent(CDiagramPrivateData *p, QEvent *event);
	
    void addContextMenu(CDiagram::DiagramMode mode, CDiagramContextMenu *menu);
    QList<CDiagramContextMenu*> getContextMenus(CDiagram::DiagramMode mode);

private:
    Q_DISABLE_COPY(CDiagramContextMenuHandler)
    bool triggerMenu(CDiagram *d, const QPoint &p);

    QMap<CDiagram::DiagramMode, QList<CDiagramContextMenu*> >   m_menus;

}; // End of class CDiagramContextMenuHandler

#define REGISTER_CONTEXT_MENU(mode, menu) \
    {\
        CDiagramContext             *cxt;\
        CDiagramContextMenuHandler  *handler;\
        cxt = CDiagramContext::env();\
        if (cxt)\
        {\
            handler = dynamic_cast<CDiagramContextMenuHandler*>( cxt->getEventHandlerByName("DiagramContextMenuHandler") );\
            if (handler)\
                handler->addContextMenu((mode), (menu));\
        }\
    };

#endif // PRODRAW_CDIAGRAMCONTEXTMENUHANDLER_H
