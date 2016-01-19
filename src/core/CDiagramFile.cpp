/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramPlugin>

CDiagramFile::CDiagramFile(const QString &type, const QString &fileName, const QString &path)
    : m_id(0),
      m_type(type),
	  m_fileName(fileName),
	  m_path(path),
	  m_version( DIAGRAM_FILE_VERSION ),
	  m_revision( 0 ),
	  m_hasPhysicalFile(false),
	  m_unsaved(false),
      m_modifiedOutside(false),
	  m_useCompress(true)
{
	m_createdDate = QDateTime::currentDateTimeUtc();
    m_lastUpdatedDate = QDateTime::currentDateTimeUtc();
}

CDiagramFile::~CDiagramFile()
{
}

void CDiagramFile::setId(int value)
{
    if (value < 0)
        return;

    m_id = value;
}

int CDiagramFile::id()
{
    return m_id;
}

QList<QUndoStack*> CDiagramFile::undoStacks()
{
    QList<QUndoStack*>  stacks;

    return stacks;
}

void CDiagramFile::initDefault()
{
    // TO BE IMPLEMENTED IN SUBCLASSES
}

void CDiagramFile::setPath(const QString &value)
{
    QFileInfo       info(value);

    if (info.exists() && info.isDir())
        m_path = value;
}

QString CDiagramFile::path()
{
    return m_path;
}

void CDiagramFile::setFileName(const QString &fileName)
{
    if (fileName.trimmed().isEmpty())
        return;

    m_fileName = fileName;
}

QString CDiagramFile::fileName()
{
    return m_fileName;
}

QString CDiagramFile::version()
{
    return m_version;
}

void CDiagramFile::setVersion(const QString &value)
{
    m_version = value;
}

QString CDiagramFile::type()
{
    return m_type;
}

void CDiagramFile::setType(const QString &value)
{
    m_type = value;
}

QString CDiagramFile::author()
{
    return m_author;
}

void CDiagramFile::setAuthor(const QString &value)
{
    m_author = value;
}

QDateTime CDiagramFile::createdDate()
{
    return m_createdDate;
}

void CDiagramFile::setCreatedDate(const QDateTime& value)
{
    m_createdDate = value;
}

QDateTime CDiagramFile::lastUpdatedDate()
{
    return m_lastUpdatedDate;
}

void CDiagramFile::setLastUpdatedDate(const QDateTime& value)
{
    m_lastUpdatedDate = value;
}

int CDiagramFile::revision()
{
    return m_revision;
}

void CDiagramFile::setRevision(int value)
{
    if (value >= 0)
        m_revision = value;
}

bool CDiagramFile::isUnsaved()
{
    return m_unsaved;
}

void CDiagramFile::setUnsaved(bool value)
{
    m_unsaved = value;
    emit saveStatusChanged(!value);
}

bool CDiagramFile::hasPhysicalFile()
{
    return m_hasPhysicalFile;
}

bool CDiagramFile::isModifiedOutside()
{
    return m_modifiedOutside;
}

void CDiagramFile::save()
{
    QDir        dir(m_path);

    save(dir.absoluteFilePath(m_fileName));
}

void CDiagramFile::save(const QString &fullPath)
{
    // TO BE CUSTOMIZED IN SUBCLASSES
}

void CDiagramFile::save(QByteArray &data)
{
	// TO BE CUSTOMIZED IN SUBCLASSES
}

void CDiagramFile::load()
{
    QDir        dir(m_path);

    return load( dir.absoluteFilePath(m_fileName) );
}

void CDiagramFile::load(const QString &fullPath)
{
	// TO BE IMPLEMENTED IN SUBCLASSES
}

void CDiagramFile::load(QByteArray &data)
{
    // TO BE IMPLEMENTED IN SUBCLASSES
}

CDiagram* CDiagramFile::getEditDiagram()
{
    return NULL;
}

void CDiagramFile::setEditDiagram(CDiagram *d)
{
	// TO BE IMPLEMENTED IN SUBCLASSES
}

void CDiagramFile::onContentChanged()
{
    m_unsaved = true;
    emit saveStatusChanged(false);
}

void CDiagramFile::toXml(QDomDocument &doc)
{
    Q_UNUSED(doc);
    // TO BE IMPLEMENTED IN SUBCLASSES
}

void CDiagramFile::fromXml(const QDomDocument &doc)
{
    Q_UNUSED(doc);
    // TO BE IMPLEMENTED IN SUBCLASSES
}

QAbstractItemModel* CDiagramFile::dataModel()
{
	return NULL;
}

bool CDiagramFile::serialize(QByteArray &data, const QString &fullPath)
{
	int						len = 0;
	char					*chs = NULL;
	QByteArray				bytes;
    QString                 fileType;
    QFile					outFile(fullPath);
    QDataStream				outStream(&outFile);

	if (!outFile.open( QIODevice::WriteOnly ))
        return false;

	// compress the string to save space
	if (m_useCompress)
		bytes = qCompress(data, 7);
	else
		bytes = data;

	// attach the id of the file type
    fileType = type();
    CDiagramFileManager::instance()->patch(fileType, bytes);

    chs = const_cast<char*>( bytes.constData() );
    len = 0;
    while (len < bytes.length())
    {
        len += outStream.writeRawData(chs + len, bytes.length() - len);
    }

	outFile.close();

	return true;
}

bool CDiagramFile::unserialize(const QString &fullPath, QByteArray &data)
{
	int				len;
    char			chs[1026];
    QFile			inFile(fullPath);
    QByteArray		id;
	QDataStream		stream(&inFile);
	
	if (!inFile.open(QIODevice::ReadOnly))
        return false;

	while ( (len = stream.readRawData(chs, 1024)) > 0)
	{
		data.append(chs, len);
	}

    // must be called to drop the prefix 16 bytes,
    // which identify the diagram file types
	CDiagramFileManager::instance()->depatch(id, data);

	if (m_useCompress)
		data = qUncompress(data);

	inFile.close();

	return true;
}

CDiagramBasicFile::CDiagramBasicFile()
    : CDiagramFile(),
      m_d(NULL)
{
	int			count = 0;
	QString		tpl("Untitled Document %1");

	count = CDiagramFileManager::instance()->fileCount();

	setType( QString( CDiagramBasicFile::staticMetaObject.className() ) );
	setFileName( tpl.arg( count + 1) );

    m_d = new CDiagram();
}

CDiagramBasicFile::~CDiagramBasicFile()
{
    SAFE_DELETE(m_d);
}

void CDiagramBasicFile::save(const QString &fullPath)
{
    QByteArray		data;
	QFileInfo		fileInfo(fullPath);
    
	// get the data to saved
	save(data);
	
	// serialize it
	if (!serialize( data, fullPath))
		return;
	
    if (!m_hasPhysicalFile)
	{
		m_createdDate = QDateTime::currentDateTimeUtc();
		m_hasPhysicalFile = true;
	}

    m_fileName = fileInfo.fileName();
    m_path = fileInfo.path();
    m_lastUpdatedDate = QDateTime::currentDateTimeUtc();
    m_revision += 1;
    m_unsaved = false;
}

void CDiagramBasicFile::save(QByteArray &data)
{
    QDomDocument			doc;
    QDomElement				root, tmp;
    QRectF					rect;
    QGraphicsView			*view = NULL;
    QMetaObject				*meta = NULL;
    QMetaProperty			prop;

    if (!m_d)
        return;

    view = m_d->getMainView();
    if (view)
    {
        rect = view->mapToScene(view->geometry()).boundingRect();
        setViewCenter(rect.center());
    }

    // create xml from the diagram instance
	root = doc.createElement("diagram");
    Utils::getXmlFromDiagram(m_d, root);

    meta = const_cast<QMetaObject*>(metaObject());
    for (int i = 0; i < meta->propertyCount(); ++i)
    {
        QByteArray	b;
        QDataStream s(&b, QIODevice::WriteOnly);

        prop = meta->property(i);
        if (prop.isValid() && prop.isReadable())
        {
            s <<  prop.read(this);
            tmp = doc.createElement(QString("property"));
            tmp.setAttribute( QString("name"), QString(prop.name()) );
            tmp.setAttribute( QString("type"), QString(prop.typeName()) );
            tmp.appendChild( doc.createTextNode( QString(b.toBase64() ) ) );
            root.insertBefore(tmp, QDomNode());
        }
    }

	doc.appendChild(root);

	// use UTF8 format
	data = doc.toString().toUtf8();
}

void CDiagramBasicFile::load(const QString &fullPath)
{
	QFileInfo		fileInfo(fullPath);
	QByteArray		data;

	if (!unserialize(fullPath, data))
		return;

    // then we can get the pure data
	load( data );
	
    m_path = fileInfo.path();
    m_fileName = fileInfo.fileName();
	m_hasPhysicalFile = true;
    m_unsaved = false;
}

void CDiagramBasicFile::load(QByteArray &data)
{
    QObject			*obj = NULL;
    QDomDocument	doc;
    QDomElement		element, root;

    if (!m_d)
        m_d = new CDiagram();

	if (doc.setContent( QString::fromUtf8(data) ))
    {
        obj = dynamic_cast<QObject*>(this);
        element = doc.documentElement();
        element = element.firstChildElement( QString("property") );
        while (!element.isNull())
        {
            QString			name, type;
            QByteArray		b;

            name = element.attribute( QString("name") );
            type = element.attribute( QString("type") );

            b = QByteArray::fromBase64( element.text().toAscii() );
            QDataStream s(&b, QIODevice::ReadOnly);
            QVariant	value(s);
            if (value.convert( QVariant::nameToType( qPrintable(type) ) ) )
            {
                obj->setProperty(qPrintable(name), value);
            }
            element = element.nextSiblingElement( QString("property") );
        }

        // load the items
		root = doc.documentElement();
        Utils::getDiagramFromXml(root, m_d);
    }
}

CDiagram* CDiagramBasicFile::getEditDiagram()
{
    return m_d;
}

void CDiagramBasicFile::setEditDiagram(CDiagram *d)
{
	if (!d)
		m_d = d;
}

void CDiagramBasicFile::setViewCenter(const QPointF &value)
{
	m_viewCenter = value;
}

QPointF CDiagramBasicFile::viewCenter()
{
	return m_viewCenter;
}
