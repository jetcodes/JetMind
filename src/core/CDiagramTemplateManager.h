/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMTEMPLATEMANAGER_H
#define PRODRAW_CDIAGRAMTEMPLATEMANAGER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramTemplate;
class CDiagramTemplateManager;

#define TEMPLATE_SUFFIX "tpl"

class CORE_EXPORT CDiagramTemplate
{
public:
	QIcon		icon;
	QString		title;
	QString		type;
	QString		filePath;

}; // End of class CDiagramTemplate

class CORE_EXPORT CDiagramTemplateManager
{
public:
	// Manipulate the template root
	void setTemplateRoot(const QString &root);
	QString templateRoot() const;

	// load all the templates from the disk
	// into the memory
	void load();

	// load templates from a package. 
	// the package must be a zip file, contains a list
	// of template files without any folder
	int loadPackage(const QString &zipFile);

	// save specified diagram as a template
	// using the type and title
	void saveAsTemplate(CDiagram *diagram, const QString &type, const QString &title);
	void loadAsTemplate(const QString &file);
    bool addTemplate(CDiagramTemplate *tpl);

	// create diagrams
	CDiagram* createDiagram(const QString &type, int index);
	CDiagram* createDiagram(const CDiagramTemplate *tpl);

	// fetch all the diagram types, it is
	// the official list of types.
	QStringList diagramTypes() const;
	void registerDiagramMeta(const QString &type, QMetaObject *obj);
	QMetaObject* getDiagramMeta(const QString &type);

	// fetch the templates specified to a diagram type
	QList<CDiagramTemplate*> getTemplates(const QString &type);

	static CDiagramTemplateManager *instance()
	{
		if (!m_instance)
		{
			m_instance = new CDiagramTemplateManager();
		}
		return m_instance;
	}
	
protected:
    bool saveTemplateData(const QByteArray &data, const QString &filePath);

	QIcon genIconFromDiagram(CDiagram *d);
	CDiagramTemplate* loadStringAsTemplate(const QString &content);

private:
	CDiagramTemplateManager();
    Q_DISABLE_COPY(CDiagramTemplateManager)
	
    QMap<QString, QList<CDiagramTemplate*>* >	m_templates;
	QMap<QString, QMetaObject*>					m_diagramMetas;
	QString										m_templatesRoot;
	static CDiagramTemplateManager				*m_instance;

}; // End of class CDiagramTemplateManager

typedef CDiagramTemplate* CDiagramTemplateP;
Q_DECLARE_METATYPE(CDiagramTemplateP)

#endif // PRODRAW_CDIAGRAMTEMPLATEMANAGER_H
