/**
* Copyright (c) 2011 JetMind. All rights reserved.
*/
#include <core/CGlobal>
#include <core/CDiagramContext>
#include <core/CDiagramSpriteManager>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramWidget>
#include <core/CDiagramWidgetManager>
#include <core/CDiagramItemToolBar>
#include <core/CDiagramFileManager>
#include <core/CDiagramTemplateManager>

#include "CMindMapLoader.h"
#include "CMindMapNode.h"
#include "CMindMapNodeHandler.h"
#include "CMindMapCategory.h"
#include "CMindMapIconProvider.h"
#include "CMindMapEventHandler.h"
#include "CMindMapNodeRenderer.h"
#include "CMindMapPathRenderer.h"
#include "CMindMapRendererManager.h"
#include "CMindMapFile.h"
#include "./tools/CMindMapToolPanel.h"

void CMindMapLoader::load(CDiagramPluginEngine *engine)
{
	CDiagramContext					*cxt = NULL;
	CDiagramSpriteManager			*spriteManager = NULL;
	CDiagramItemTypeDatabase		*db = NULL;
	CDiagramItemTypeCategory		*ctg = NULL;
	CDiagramWidgetManager			*widgetManager = NULL;
    CDiagramWidget					*widget = NULL;
	CMindMapRendererManager			*rendererManager = NULL;
    CMindMapIconProvider            *iconProvider = NULL;
	
    // load icon library
    iconProvider = CMindMapIconProvider::instance();
    if (iconProvider)
        iconProvider->load();

	db = dynamic_cast<CDiagramItemTypeDatabase*>( engine->getSymbol( Global::SYM_DB_ITEM ) );
	if (db)
	{
		ctg = new CMindMapCategory();
		ctg->load();
		db->registerCategory(ctg->name(), ctg);
	}

	cxt = dynamic_cast<CDiagramContext*>(engine->getSymbol( Global::SYM_DB_CONTEXT ) );
	if (cxt)
	{
		spriteManager = cxt->spriteManager();
		if (spriteManager)
		{
			spriteManager->addSprite(new CMindMapNodeHandler() );
			spriteManager->addSprite(new CMindMapGroupHandler() );
		}

		widgetManager = cxt->widgetManager();
		if (widgetManager)
		{
			widget = widgetManager->getWidget( CDiagramItemToolBar::WidgetType );
			if (widget)
            {
                widget->addHostedWidget( new CMindMapToolPanel(NULL) );
			}
		}

        cxt->addEventHandler( new CMindMapEventHandler() );
	}

	rendererManager = CMindMapRendererManager::instance();
	if (rendererManager)
	{
		// node renderers
		rendererManager->addNodeRenderer(CMindMapNode::MindMap, new CMindMapRenderer(CMindMapRenderer::DoubleFlow) );
		rendererManager->addNodeRenderer(CMindMapNode::ClockwiseMindMap, new CMindMapRenderer(CMindMapRenderer::Clockwise) );
		rendererManager->addNodeRenderer(CMindMapNode::AntiClockwiseMindMap, new CMindMapRenderer(CMindMapRenderer::AntiClockwise) );
		rendererManager->addNodeRenderer(CMindMapNode::DownsideOrganization, new COrganizationMapRenderer() );
		rendererManager->addNodeRenderer(CMindMapNode::UpsideOrganization, new COrganizationMapRenderer(COrganizationMapRenderer::Upwards) );
		rendererManager->addNodeRenderer(CMindMapNode::LeftTree, new CTreeMapRenderer(CTreeMapRenderer::Leftwards));
		rendererManager->addNodeRenderer(CMindMapNode::RightTree, new CTreeMapRenderer(CTreeMapRenderer::Rightwards));
		rendererManager->addNodeRenderer(CMindMapNode::LeftLogic, new CLogicMapRenderer(CLogicMapRenderer::Leftwards));
		rendererManager->addNodeRenderer(CMindMapNode::RightLogic, new CLogicMapRenderer(CLogicMapRenderer::Rightwards));
		rendererManager->addNodeRenderer(CMindMapNode::LeftFishBone, new CFishMapRenderer(CFishMapRenderer::Leftwards));
		rendererManager->addNodeRenderer(CMindMapNode::RightFishBone, new CFishMapRenderer(CFishMapRenderer::Rightwards));

		// path renderers
		rendererManager->addPathRenderer(CMindMapNode::StraightLine, new CStraightLineRenderer());
		rendererManager->addPathRenderer(CMindMapNode::CurveLine, new CCurveLineRenderer());
		rendererManager->addPathRenderer(CMindMapNode::ArrowLine, new CAngledLineRenderer());
		rendererManager->addPathRenderer(CMindMapNode::PolygonLine, new CPolygonLineRenderer());
		rendererManager->addPathRenderer(CMindMapNode::RoundedLine, new CRoundedLineRenderer());
	}

	// register file type
	CDiagramFileManager::instance()->registerMeta( QString( CMindMapFile::staticMetaObject.className() ), const_cast<QMetaObject*>( &CMindMapFile::staticMetaObject ) );
	
	// register diagram type
	if (cxt)
		cxt->registerDiagramType(NODE_CATEGORY);
	CDiagramTemplateManager::instance()->registerDiagramMeta(NODE_CATEGORY, const_cast<QMetaObject*>(&CMindMapGraph::staticMetaObject));
}
