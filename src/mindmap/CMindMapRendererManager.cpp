/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>

#include "CMindMapNodeRenderer.h"
#include "CMindMapPathRenderer.h"
#include "CMindMapRendererManager.h"

CMindMapRendererManager* CMindMapRendererManager::m_inst = NULL;

CMindMapRendererManager::CMindMapRendererManager()
{

}

void CMindMapRendererManager::addNodeRenderer(int type, CMindMapNodeRenderer *renderer)
{
	if (renderer && !m_nodeRenderers.contains(type))
		m_nodeRenderers.insert(type, renderer);
}

CMindMapNodeRenderer* CMindMapRendererManager::getNodeRenderer(int type)
{
	CMindMapNodeRenderer	*r = NULL;

	if (m_nodeRenderers.contains(type))
		r = m_nodeRenderers[type];
	return r;
}

void CMindMapRendererManager::addPathRenderer(int type, CMindMapPathRenderer *renderer)
{
	if (renderer && !m_pathRenderers.contains(type) )
		m_pathRenderers.insert(type, renderer);
}

CMindMapPathRenderer* CMindMapRendererManager::getPathRenderer(int type)
{
	CMindMapPathRenderer	*r = NULL;

	if (m_pathRenderers.contains(type))
		r = m_pathRenderers[type];
	return r;
}
