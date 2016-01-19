/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CGlobal>
#include <core/CDiagramPlugin>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramContext>

CDiagramPluginEngine* CDiagramPluginEngine::m_inst = NULL;

CDiagramPluginEngine::CDiagramPluginEngine()
{
	registerSymbol( Global::SYM_DB_ROUTER, CDiagramEdgeStyleDatabase::instance() );
	registerSymbol( Global::SYM_DB_ARROWSTYLE, CDiagramArrowDatabase::instance() );
	registerSymbol( Global::SYM_DB_ITEM, CDiagramItemTypeDatabase::instance() );
	registerSymbol( Global::SYM_DB_CONTEXT, CDiagramContext::env() );
}

CDiagramPluginEngine::~CDiagramPluginEngine()
{
}

void CDiagramPluginEngine::registerSymbol(const QString &symbol, QObject *obj)
{
	if (!m_symbols.contains(symbol))
	{
		m_symbols[symbol] = obj;
	}
}

QObject* CDiagramPluginEngine::getSymbol(const QString &symbol)
{
	QObject	*c = NULL;

	if (m_symbols.contains(symbol))
		c = m_symbols[symbol];
	return c;
}

CDiagramPlugin::CDiagramPlugin(const QString &name, const QString &desc)
	: m_name(name),
	  m_desc(desc)
{

}