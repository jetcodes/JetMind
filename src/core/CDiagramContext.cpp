/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramEditor>
#include <core/CDiagramContext>
#include <core/CDiagramCore>
#include <core/CDiagramCommands>
#include <core/CDiagramCommandBuilder>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramNodeHandler>
#include <core/CDiagramEdgeHandler>
#include <core/CDiagramGroupHandler>
#include <core/CDiagramSpriteManager>
#include <core/CDiagramWidgetManager>
#include <core/CDiagramEventHandler>
#include <core/CDiagramDragDropHandler>
#include <core/CDiagramContextMenuHandler>
#include <core/CDiagramItemToolBar>
#include <core/CDiagramTextToolBar>
#include <core/CDiagramDemoToolBar>
#include <core/CDiagramItemGeometryNotifier>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>

CDiagramContext* CDiagramContext::m_inst = NULL;

bool priorityLessThan(const CDiagramEventHandler *e1, const CDiagramEventHandler *e2)
{
	bool	r= false;

	if (e1 && e2)
		r = e1->priority() >= e2->priority();
	
	return r;
}

CDiagramContext::CDiagramContext()
	: m_mainUI(NULL),
	  m_mainEditor(NULL),
	  m_spriteManager(NULL),
	  m_widgetManager(NULL)
{
	
}

void CDiagramContext::initCoreEnvironment()
{
	CDiagramCommandBuilder		*cmdBuilder = NULL;
	CDiagramArrowCategory		*arrowCategory = NULL;
	CDiagramItemTypeCategory	*itemCategory = NULL;
	CDiagramEdgeStyleCategory	*edgeStyleCategory = NULL;
	CDiagramEditor				*editor = NULL;
	CDiagramSpriteManager		*spriteManager = NULL;
	CDiagramWidgetManager		*widgetManager = NULL;
	CDiagramEventHandler		*eventHandler = NULL;
	CDiagramFileManager			*fileManager = NULL;

	// Command builder
	cmdBuilder = CDiagramCommandBuilder::instance();
	if (cmdBuilder)
	{
		cmdBuilder->registerMetaObject( Global::CMD_PASTE, &CPasteCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_NEWITEM, &CNewItemCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_DELETEITEMS, &CDeleteItemCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_MOVEITEMS, &CMoveItemsCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_DRAGGINGITEMS, &CDraggingItemsCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_RESIZE, &CResizeCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_ADJUSTROUTE, &CAdjustRouteCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_RECONNECT, &CReconnectCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_ROTATE, &CRotateCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_CHANGEPROPERTY, &CChangePropertyCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_EDITTEXT, &CEditTextCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_DRAGCOPYITEMS, &CDragCopyItemsCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_DRAGADDITEM, &CDragNewItemCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_GROUP, &CGroupCommand::staticMetaObject );
		cmdBuilder->registerMetaObject( Global::CMD_UNGROUP, &CUnGroupCommand::staticMetaObject );
	}
	
	// Diagram Core Elements initialization
	itemCategory = new CCoreNodes();
	CDiagramItemTypeDatabase::instance()->registerCategory( itemCategory->name(), itemCategory );

	arrowCategory = new CCoreArrows();
	CDiagramArrowDatabase::instance()->registerCategory( arrowCategory->name(), arrowCategory );

	edgeStyleCategory = new CCoreEdges();
	CDiagramEdgeStyleDatabase::instance()->registerCategory( edgeStyleCategory->name(), edgeStyleCategory );
	
	// Diagram item handler manager's initialization
	spriteManager = new CDiagramSpriteManager();
	spriteManager->addSprite( new CDiagramNodeHandler() );
	spriteManager->addSprite( new CDiagramEdgeHandler() );
	spriteManager->addSprite( new CDiagramGroupHandler() );
	spriteManager->addSprite( new CDiagramItemGeometryNotifier() );
	setSpriteManager( spriteManager );

	widgetManager = new CDiagramWidgetManager();
	widgetManager->addWidget( new CDiagramItemToolBar() );
	widgetManager->addWidget( new CDiagramTextToolBar() );
	widgetManager->addWidget( new CDiagramDemoToolBar() );
	//widgetManager->addWidget( new CDiagramItemGeometryToolBar() );
	setWidgetManager( widgetManager );

	// Set main editor
	editor = new CDiagramEditor();
	editor->setDiagramContext(this);
	setMainEditor(editor);

	// Create diagram event handler
	eventHandler = new CDiagramDragDropHandler();
	addEventHandler(eventHandler);

	eventHandler = new CDiagramContextMenuHandler();
	addEventHandler(eventHandler);

	// Register basic file types
	fileManager = CDiagramFileManager::instance();
	fileManager->registerMeta( QString(CDiagramBasicFile::staticMetaObject.className()), const_cast<QMetaObject*>( &CDiagramBasicFile::staticMetaObject) );

	// Load fonts
	QFontDatabase::addApplicationFont(":/res/fonts/fontawesome-webfont.ttf");
}

QWidget* CDiagramContext::getMainUI()
{
	return m_mainUI;
}

void CDiagramContext::setMainUI(QWidget *ui)
{
	if (!ui) return;

	m_mainUI = ui;
}

CDiagramEditor* CDiagramContext::getMainEditor()
{
	return m_mainEditor;
}

void CDiagramContext::setSpriteManager(CDiagramSpriteManager *manager)
{
	m_spriteManager = manager;
}

CDiagramSpriteManager* CDiagramContext::spriteManager()
{
	return m_spriteManager;
}

void CDiagramContext::setWidgetManager(CDiagramWidgetManager *manager)
{
	m_widgetManager = manager;
}

CDiagramWidgetManager* CDiagramContext::widgetManager()
{
	return m_widgetManager;
}

void CDiagramContext::registerDiagramType(const QString &type)
{
	if (!m_diagramTypes.contains(type))
	{
		m_diagramTypes << type;
	}
}

void CDiagramContext::setDiagramTypes(const QStringList &types)
{
	m_diagramTypes = types;
}

QStringList CDiagramContext::diagramTypes()
{
	return m_diagramTypes;
}

void CDiagramContext::addEventHandler(CDiagramEventHandler *handler)
{
    if (handler)
    {
        foreach (CDiagramEventHandler *h, m_eventHandlers)
        {
            if (h->name() == handler->name())
                return;
        }
		m_eventHandlers.push_back( handler );
        qStableSort(m_eventHandlers.begin(), m_eventHandlers.end(), priorityLessThan);
    }
}

QList<CDiagramEventHandler*> CDiagramContext::eventHandlers()
{
	return m_eventHandlers;
}

CDiagramEventHandler* CDiagramContext::getEventHandlerByName(const QString &name)
{
    CDiagramEventHandler        *handler = NULL;

    foreach (CDiagramEventHandler *h, m_eventHandlers)
    {
        if (h->name() == name)
        {
            handler = h;
            break;
        }
    }

    return handler;
}

void CDiagramContext::setMainEditor(CDiagramEditor *editor)
{
	if (!editor)
		return;

	m_mainEditor = editor;
}

void CDiagramContext::addObject(const QString &key, QObject *obj)
{
	if (!obj || m_objects.contains(key))
		return;
	m_objects.insert(key, obj);
}

QObject* CDiagramContext::getObject(const QString &key)
{
	QObject	*r = NULL;

	if (m_objects.contains(key))
		r = m_objects[key];
	return r;
}

void CDiagramContext::addWidget(const QString &key, QWidget *widget)
{
	if (!widget || m_widgets.contains(key))
		return;
	m_widgets.insert(key, widget);
}

QWidget* CDiagramContext::getWidget(const QString &key)
{
	QWidget	*w = NULL;

	if (m_widgets.contains(key))
		w = m_widgets[key];
	return w;
}

void CDiagramContext::addVariant(const QString &key, const QVariant &variant)
{
	if (!variant.isValid() || m_variants.contains(key))
		return;
	m_variants.insert(key, variant);
}

QVariant CDiagramContext::getVariant(const QString &key)
{
	QVariant	v;

	if (m_variants.contains(key))
		return m_variants[key];
	return v;
}

void CDiagramContext::addVariant(CDiagram *d, const QString &key, const QVariant &value)
{
	if (!d)
		return;

	if (!m_diagramVariants.contains(d))
		m_diagramVariants.insert(d, QMap<QString, QVariant>());
	m_diagramVariants[d].insert(key, value);
	emit diagramContextChanged(d, key);
}

QVariant CDiagramContext::getVariant(CDiagram *d, const QString &key)
{
	QVariant	r;

	if (!d || !m_diagramVariants.contains(d))
		return r;

	if (m_diagramVariants[d].contains(key))
		r = m_diagramVariants[d].value(key);
	return r;
}

void CDiagramContext::addObject(CDiagram *d, const QString &key, void *object)
{
	if (!d)
		return;

	if (!m_diagramObjects.contains(d))
		m_diagramObjects.insert(d, QMap<QString, void*>());
	m_diagramObjects[d].insert(key, object);
	emit diagramContextChanged(d, key);
}

void* CDiagramContext::getObject(CDiagram *d, const QString &key)
{
	void	*r = NULL;

	if (!d || !m_diagramObjects.contains(d))
		return r;

	if (m_diagramObjects[d].contains(key))
		r = m_diagramObjects[d].value(key);
	return r;
}

void CDiagramContext::initContext(CDiagram *d)
{
	if (!d)
		return;

	m_spriteManager->init(d);
	m_widgetManager->init(d);
	d->setContext(this);
}

void CDiagramContext::uninitContext(CDiagram *d)
{
	if (!d)
		return;

	m_spriteManager->uninit(d);
	m_widgetManager->uninit(d);

	d->setContext(NULL);
}
