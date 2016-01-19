/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMENVIRONMENT_H
#define PRODRAW_CDIAGRAMENVIRONMENT_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramEditor;
class CDiagramSpriteManager;
class CDiagramWidgetManager;
class CDiagramEventHandler;

class CORE_EXPORT CDiagramContext: public QObject
{
	Q_OBJECT

public:
	void initCoreEnvironment();

	QWidget* getMainUI();
	void setMainUI(QWidget *ui);

	CDiagramEditor* getMainEditor();
	void setMainEditor(CDiagramEditor *editor);

	CDiagramWidgetManager* widgetManager();
	void setWidgetManager(CDiagramWidgetManager *manager);

	CDiagramSpriteManager* spriteManager();
	void setSpriteManager(CDiagramSpriteManager *manager);
	
	QStringList diagramTypes();
	void registerDiagramType(const QString &type);
	void setDiagramTypes(const QStringList &types);

	void addEventHandler(CDiagramEventHandler *handler);
	QList<CDiagramEventHandler*> eventHandlers();
  CDiagramEventHandler* getEventHandlerByName(const QString &name);

	void addObject(const QString &key, QObject *obj);
	QObject* getObject(const QString &key);

	void addWidget(const QString &key, QWidget *widget);
	QWidget* getWidget(const QString &key);

	void addVariant(const QString &key, const QVariant &value);
	QVariant getVariant(const QString &key);

	void addVariant(CDiagram *d, const QString &key, const QVariant &value);
	QVariant getVariant(CDiagram *d, const QString &key);

	void addObject(CDiagram *d, const QString &key, void *obj);
	void* getObject(CDiagram *d, const QString &key);

	void initContext(CDiagram *d);
	void uninitContext(CDiagram *d);

	static CDiagramContext *env()
	{
		if (!m_inst)
			m_inst = new CDiagramContext();
		return m_inst;
	}
	
Q_SIGNALS:
	void environmentChanged(const QString&);
	void diagramContextChanged(CDiagram*, const QString &);

private:
	CDiagramContext();
  Q_DISABLE_COPY(CDiagramContext)
	
	QWidget							*m_mainUI;
	CDiagramEditor					*m_mainEditor;
	CDiagramSpriteManager			*m_spriteManager;
	CDiagramWidgetManager			*m_widgetManager;
	QList<CDiagramEventHandler*>	m_eventHandlers;
	
	QStringList						m_diagramTypes;

	QMap<QString, QWidget*>			m_widgets;
	QMap<QString, QObject*>			m_objects;
	QMap<QString, QVariant>			m_variants;

	QMap<CDiagram*, QMap<QString, QVariant> > m_diagramVariants;
	QMap<CDiagram*, QMap<QString, void*> >	  m_diagramObjects;

	static CDiagramContext *m_inst;

}; // End of class CDiagramContext

#endif // PRODRAW_CDIAGRAMENVIRONMENT_H
