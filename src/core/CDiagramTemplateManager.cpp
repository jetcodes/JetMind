/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <QCryptographicHash>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramFile>
#include <core/CDiagramContext>
#include <core/CDiagramTemplateManager>
#include <quazip/quazip>
#include <quazip/quazipfile>

// persistent
// save: text -> compressed
// load: uncompressed -> text;

CDiagramTemplateManager* CDiagramTemplateManager::m_instance = NULL;

CDiagramTemplateManager::CDiagramTemplateManager()
{
}

void CDiagramTemplateManager::setTemplateRoot(const QString &root)
{
	m_templatesRoot = root;
}

QString CDiagramTemplateManager::templateRoot() const
{
	return m_templatesRoot;
}

QStringList CDiagramTemplateManager::diagramTypes() const
{
	QStringList			types;
	CDiagramContext		*cxt = NULL;

	cxt = CDiagramContext::env();
	if (cxt)
	{
		types = cxt->diagramTypes();
	}

	return types;
}

void CDiagramTemplateManager::registerDiagramMeta(const QString &type, QMetaObject *obj)
{
	if (!m_diagramMetas.contains(type) && obj)
	{
		m_diagramMetas[type] = obj;
	}
}

QMetaObject* CDiagramTemplateManager::getDiagramMeta(const QString &type)
{
	QMetaObject		*obj = NULL;

	if (m_diagramMetas.contains(type))
		obj = m_diagramMetas[type];
	return obj;
}

QList<CDiagramTemplate*> CDiagramTemplateManager::getTemplates(const QString &type)
{
	QList<CDiagramTemplate*> tpls;

	if (m_templates.contains(type))
	{
		tpls = *m_templates[type];
	}

	return tpls;
}

void CDiagramTemplateManager::load()
{
	QDir				dir(m_templatesRoot);
	QFileInfo			dirInfo(m_templatesRoot);
	QFileInfo			fileInfo;
	QStringList			nameFilters;
    QFileInfoList		ps;

	nameFilters << QString("*.%1").arg(TEMPLATE_SUFFIX);
	if (dirInfo.exists() && dirInfo.isDir())
	{
		ps = dir.entryInfoList(nameFilters, QDir::Files);
		for (int i = 0; i < ps.length(); ++i)
		{
			fileInfo = ps.at(i);
			if (fileInfo.exists() && fileInfo.isFile())
			{
				loadAsTemplate(fileInfo.absoluteFilePath());
			}
		}
	}
}

int CDiagramTemplateManager::loadPackage(const QString &zipFileName)
{
	int					count = 0;
	QDir				dir(m_templatesRoot);
	QString				fileName, filePath;
	QuaZip				zip(zipFileName);
	QuaZipFile			zipFile(&zip);
	CDiagramTemplate	*tpl = NULL;
	
	if (zip.open(QuaZip::mdUnzip))
	{
		for(bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
		{
			fileName = zip.getCurrentFileName();
			if (fileName.toLower().endsWith(TEMPLATE_SUFFIX) && zipFile.open(QIODevice::ReadOnly))
			{
				int			len = 0;
				char		chs[1026];
				QByteArray	zipData, unzipData;
				QDataStream	stream(&zipFile);

				while ( (len = stream.readRawData(chs, 1024)) > 0)
				{
					zipData.append(chs, len);
				}

				unzipData = qUncompress( zipData );
				tpl = loadStringAsTemplate( unzipData );
				if (tpl)
				{
					fileName = QString( QCryptographicHash::hash(unzipData, QCryptographicHash::Md5).toBase64() );
					filePath = dir.absoluteFilePath( QString("%1.%2").arg(fileName).arg(TEMPLATE_SUFFIX) );
					
					saveTemplateData(unzipData, filePath);
					tpl->filePath = filePath;

					addTemplate(tpl);
					++count;
				}
				zipFile.close();
			}
		}
		zip.close();
	}

	return count;
}

void CDiagramTemplateManager::saveAsTemplate(CDiagram *d, const QString &type, const QString &title)
{
	QDir					dir(m_templatesRoot);
	QString					fileName, filePath;
	QDomDocument			templateDoc;
	QDomElement				root, element;
	QIcon					icon;
	QByteArray				iconData;
	QByteArray				unzipData;
	QDataStream				iconStream(&iconData, QIODevice::WriteOnly);
	CDiagramTemplate		*tpl = NULL;

	if (!d || title.isEmpty())
		return;
	
	icon = genIconFromDiagram(d);
	iconStream << icon;

	root = templateDoc.createElement("template");

	element = templateDoc.createElement("type");
	element.appendChild( templateDoc.createTextNode(type) );
	root.appendChild(element);

	element = templateDoc.createElement("icon");
	element.appendChild(templateDoc.createTextNode( QString(iconData.toBase64()) ));
	root.appendChild(element);

	element = templateDoc.createElement("title");
	element.appendChild(templateDoc.createTextNode(title));
	root.appendChild(element);

	// transform the content of the diagram to xml
	element = templateDoc.createElement("content");
	d->toXml(element);

	// append it to the end of the whole template document
	root.appendChild(element);
	
	templateDoc.appendChild(root);
	unzipData = templateDoc.toString().toAscii();

	fileName = QString( QCryptographicHash::hash(unzipData, QCryptographicHash::Md5).toBase64() );
	filePath = dir.absoluteFilePath( QString("%1.%2").arg( fileName ).arg( TEMPLATE_SUFFIX ) );

	tpl = new CDiagramTemplate();
	tpl->title = title;
	tpl->icon = icon;
	tpl->type = type;
	tpl->filePath = filePath;

	saveTemplateData(unzipData, filePath);
	addTemplate(tpl);
}

void CDiagramTemplateManager::loadAsTemplate(const QString &file)
{
	int					len;
	char				chs[1026];
	QFile				inFile(file);
	QByteArray			zipData;
	QDataStream			stream(&inFile);
	CDiagramTemplate	*tpl = NULL;

	if (!inFile.open(QIODevice::ReadOnly))
		return;

	while ( (len = stream.readRawData(chs, 1024)) > 0)
	{
		zipData.append(chs, len);
	}

	tpl = loadStringAsTemplate( qUncompress(zipData) );
	if (tpl)
	{
		tpl->filePath = file;
	}

	inFile.close();
}

CDiagramTemplate* CDiagramTemplateManager::loadStringAsTemplate(const QString &value)
{
	QDomDocument		doc;
	QDomElement			root;
	CDiagramTemplate	*tpl = NULL;
	QString				title, type;
	QIcon				icon;
	QByteArray			iconData;
	QDataStream			iconStream(&iconData, QIODevice::ReadOnly);

	if (doc.setContent(value))
	{
		tpl = new CDiagramTemplate();
		root = doc.documentElement();
		tpl->type = root.firstChildElement("type").text();
		tpl->title = root.firstChildElement("title").text();

		// retrieve icon
		iconData = QByteArray::fromBase64( root.firstChildElement("icon").text().toAscii() );
		iconStream >> icon;
		tpl->icon = icon;
		
		addTemplate(tpl);
	}

	return tpl;
}

CDiagram* CDiagramTemplateManager::createDiagram(const QString &type, int index)
{
	CDiagram					*d = NULL;
	CDiagramTemplate			*tpl = NULL;
	QList<CDiagramTemplate*>	*templates;
	
	if (m_templates.contains(type))
	{
		templates = m_templates[type];
		if (index >= 0 && templates->length() > index)
		{
			tpl = templates->at(index);
			d = createDiagram(tpl);
		}
	}

	return d;
}

CDiagram* CDiagramTemplateManager::createDiagram(const CDiagramTemplate *tpl)
{
	int				len;
	char			chs[1026];
	QFile			inFile;
	CDiagram		*d = NULL;
	QByteArray		zipData;
	QDataStream		stream(&inFile);
	QDomDocument	templateDoc;
    QDomElement		root, element;

	if (!tpl || !m_diagramMetas.contains(tpl->type))
		return d;

    inFile.setFileName( tpl->filePath );
	if (!inFile.open(QIODevice::ReadOnly))
		return d;

	while ( (len = stream.readRawData(chs, 1024)) > 0)
	{
		zipData.append(chs, len);
	}

	if (templateDoc.setContent( qUncompress(zipData) ) )
	{
		root = templateDoc.documentElement();
		element = root.firstChildElement("content");
		d = dynamic_cast<CDiagram*>( m_diagramMetas[tpl->type]->newInstance() );
		if (d)
		{
			d->fromXml(element);
		}
	}

	return d;
}

QIcon CDiagramTemplateManager::genIconFromDiagram(CDiagram *d)
{
	QPixmap		pixmap(d->sceneRect().size().toSize());
	QPainter	painter;

	pixmap.fill(Qt::transparent);
	painter.begin(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	d->drawAllItems(&painter);
	painter.end();

	return QIcon(pixmap);
}

bool CDiagramTemplateManager::saveTemplateData(const QByteArray &unzipData, const QString &filePath)
{
	bool	r = false;

    if (!QFile::exists(filePath))
	{
		int					len = 0;
		char				*chs = NULL;
		QByteArray			zipData;
		QFile				outFile(filePath);
		QDataStream			outStream(&outFile);

		if (!outFile.open(QIODevice::WriteOnly))
			return r;

		// compress the string to save space
		zipData = unzipData;
		
		zipData = qCompress(unzipData, 7);
		chs = const_cast<char*>( zipData.constData() );
		len = 0;
		while (len < zipData.length())
		{
			len += outStream.writeRawData(chs + len, zipData.length() - len);
		}

		outFile.close();
		r = true;
	}

	return r;
}

bool CDiagramTemplateManager::addTemplate(CDiagramTemplate *tpl)
{
	if (!tpl)
		return false;

	if (!m_templates.contains(tpl->type))
		m_templates[tpl->type] = new QList<CDiagramTemplate*>();
	m_templates[tpl->type]->push_back(tpl);

	return true;
}
