/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMPLUGIN_H
#define PRODRAW_CDIAGRAMPLUGIN_H

#include <QtGui>
#include <core/CGlobal>

// CDiagramPluginEngine provides the resource pool
// for all plugins. It is used to pass/retrieve data
// from the additional plugins
class CORE_EXPORT CDiagramPluginEngine: public QObject
{
	Q_OBJECT

public:
	CDiagramPluginEngine();
	virtual ~CDiagramPluginEngine();
	
	void registerSymbol(const QString &symbol, QObject *obj);
	QObject* getSymbol(const QString &symbol);

	static CDiagramPluginEngine* instance()
	{
		if (m_inst == NULL)
			m_inst = new CDiagramPluginEngine();
		return m_inst;
	}

protected:
	static CDiagramPluginEngine	*m_inst;

	QMap<QString, QObject*>		m_symbols;

}; // End of class CDiagramPluginEngine

// CDiagramPlugin is the abstract base class for all
// plugins intended for used in diagrams.
class CORE_EXPORT CDiagramPlugin : public QObject
{
	Q_OBJECT

public:
	CDiagramPlugin(const QString &name = QString::fromUtf8(""), const QString &desc = QString::fromUtf8("") );
	virtual void load(CDiagramPluginEngine *engine) {}

	const QString name() { return m_name; }
	const QString desc() { return m_desc; }
	
protected:
	QString					m_name;
	QString					m_desc;

}; // End of class CDiagramPlugin

// Make it an interface for plugins
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CDiagramPlugin,"com.prodraw.CDiagramPlugin/1.0")
QT_END_NAMESPACE

#endif // PRODRAW_CDIAGRAMPLUGIN_H
