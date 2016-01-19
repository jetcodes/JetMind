/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPRENDERERMANAGER_H
#define PRODRAW_CMINDMAPRENDERERMANAGER_H

#include <QtGui>

#include "CMindMapNode.h"

class CMindMapNodeRenderer;
class CMindMapPathRenderer;

class CMindMapRendererManager
{
public:
	void addNodeRenderer(int layout, CMindMapNodeRenderer *renderer);
	CMindMapNodeRenderer* getNodeRenderer(int layout);

	void addPathRenderer(int type, CMindMapPathRenderer *renderer);
	CMindMapPathRenderer* getPathRenderer(int type);

	static CMindMapRendererManager* instance()
	{
		if (!m_inst)
			m_inst = new CMindMapRendererManager();
		return m_inst;
	}

private:
	CMindMapRendererManager();
	Q_DISABLE_COPY(CMindMapRendererManager);

	QMap<int, CMindMapNodeRenderer*>		m_nodeRenderers;
	QMap<int, CMindMapPathRenderer*>		m_pathRenderers;

	static CMindMapRendererManager* m_inst;

}; // End of class CMindMapRendererManager

#endif // PRODRAW_CMINDMAPRENDERERMANAGER_H
