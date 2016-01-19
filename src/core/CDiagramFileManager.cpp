/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>

CDiagramFileManager* CDiagramFileManager::m_fileManager = NULL;

CDiagramFileManager::CDiagramFileManager()
    : m_infoModel(NULL),
      m_currentIndex(-1),
      m_nextFileId(0)
{
	m_infoModel = new QStandardItemModel(this);
}

CDiagramFile* CDiagramFileManager::create(const QString type)
{
	CDiagramFile	*file = NULL;
	QMetaObject		*meta = NULL;

	if (m_metaObjects.contains(type))
	{
		meta = m_metaObjects[type];
		file = dynamic_cast<CDiagramFile*>( meta->newInstance() );
		if (file)
		{
			file->setId( nextFileId() );
			addFile(file);
		}
	}

	return file;
}

CDiagramFile* CDiagramFileManager::open(const QString &fullPath)
{
	QString			type;
	CDiagramFile	*file = NULL;

	type = getFileType(fullPath);
	file = create(type);
	if (file)
		file->load(fullPath);
	return file;
}

bool CDiagramFileManager::isOpened(const QString &fullPath)
{
	bool			r = false;
	QString			path;

	foreach (CDiagramFile *file, m_files)
	{
		path = QDir(file->path()).absoluteFilePath(file->fileName());
		if ( path == fullPath)
		{
			r = true;
			break;
		}
	}

	return r;
}

void CDiagramFileManager::addFile(CDiagramFile *file)
{
	QStandardItem		*item = NULL;

	if (!file || m_files.contains(file))
		return;

	m_files.push_back(file);

	item = new QStandardItem();
    item->setData( file->path(), Qt::UserRole);
	item->setData( file->id(), Qt::UserRole + 1);
    item->setData( file->lastUpdatedDate(), Qt::UserRole + 2);
    item->setData( file->author(), Qt::UserRole + 3);
    item->setData( file->isUnsaved(), Qt::UserRole + 4);
    item->setData( file->revision(), Qt::UserRole + 5);
	item->setData( file->fileName(), Qt::DisplayRole );
	item->setData( QDir(file->path()).absoluteFilePath(file->fileName()), Qt::ToolTipRole );
	m_infoModel->appendRow(item);
}

void CDiagramFileManager::closeFile(CDiagramFile *file)
{
    int				id;
	QStandardItem	*item = NULL;

	if (!file || !m_files.contains(file))
		return;

	for (int i = 0; i < m_infoModel->rowCount(); ++i)
	{
		item = m_infoModel->item(i, 0);
		id = item->data( Qt::UserRole + 1 ).toInt();
		if (id == file->id())
		{
			m_infoModel->removeRow(i);
			break;
		}
	}
	m_files.removeAll(file);
	SAFE_DELETE(file);
}

CDiagramFile* CDiagramFileManager::getCurrentFile()
{
	CDiagramFile	*file = NULL;

	if (m_currentIndex >= 0 && m_currentIndex < m_files.length())
		file = m_files[m_currentIndex];
	return file;
}

CDiagramFile* CDiagramFileManager::getFileById(int id)
{
    CDiagramFile        *file = NULL;

    foreach (CDiagramFile *t, m_files)
    {
        if (t->id() == id)
        {
            file = t;
            break;
        }
    }

    return file;
}

void CDiagramFileManager::setCurrentFile(CDiagramFile *file)
{
	int			index = -1;

	index = m_files.indexOf(file);
    if (index >= 0)
		m_currentIndex = index;
}

CDiagramFile* CDiagramFileManager::getNextFile(int step)
{
	int		index = -1;

	index = qMin(qMax(m_currentIndex + step, 0), m_files.length() - 1);
	return m_files[index];
}

QAbstractItemModel* CDiagramFileManager::infoModel()
{
	return dynamic_cast<QAbstractItemModel*>( m_infoModel );
}

void CDiagramFileManager::updateFileInfo(CDiagramFile *file)
{
    int                 id = -1;
    QStandardItem       *item = NULL;

    if (!file)
        return;

    for (int i = 0; i < m_files.length(); ++i)
    {
        item = m_infoModel->item(i, 0);
        id = item->data( Qt::UserRole + 1 ).toInt();
        if (id == file->id())
        {
            item->setData( file->path(), Qt::UserRole);
            item->setData( file->lastUpdatedDate(), Qt::UserRole + 2);
            item->setData( file->author(), Qt::UserRole + 3);
            item->setData( file->isUnsaved(), Qt::UserRole + 4);
            item->setData( file->revision(), Qt::UserRole + 5);
            item->setData( file->fileName(), Qt::DisplayRole );
            item->setData( QDir(file->path()).absoluteFilePath(file->fileName()), Qt::ToolTipRole );
            break;
        }
    }
}

bool CDiagramFileManager::hasUnsavedFile()
{
    bool       unsaved = false;

    foreach (CDiagramFile *file, m_files)
    {
        if (file->isUnsaved())
        {
            unsaved = true;
            break;
        }
    }

    return unsaved;
}

bool CDiagramFileManager::closeAllFiles()
{
    if (hasUnsavedFile())
        return false;

    foreach (CDiagramFile *file, m_files)
    {
        SAFE_DELETE(file);
    }
    m_files.clear();
    m_infoModel->clear();
    m_currentIndex = -1;

    // the file count has been changed to zero
    emit fileCountChanged(0);

    return true;
}

void CDiagramFileManager::saveAllFiles()
{
    foreach (CDiagramFile *file, m_files)
    {
        if (file->hasPhysicalFile())
            file->save();
    }
}

void CDiagramFileManager::restoreAllFiles()
{
    // TO BE IMPLEMENTED
}

QList<CDiagramFile*> CDiagramFileManager::files()
{
    return m_files;
}

int CDiagramFileManager::fileCount()
{
    return m_files.count();
}

int CDiagramFileManager::nextFileId()
{
    return m_nextFileId++;
}

QString CDiagramFileManager::getFileType(const QString &fullPath)
{
    int				len;
    char			chs[20];
    QFile			file(fullPath);
    QString         type;
    QByteArray		data;
    QDataStream		stream(&file);

    if (!file.open(QIODevice::ReadOnly))
        return type;

    while ( (len = stream.readRawData(chs, 18)) >= 16)
    {
        data.append(chs, len);
        break;
    }

    // left justfied
    if (data.length() > 16)
        data = data.left(16);

    foreach (QString key, m_metaObjects.keys())
    {
        if (QCryptographicHash::hash(key.toAscii(), QCryptographicHash::Md5) == data)
        {
            type = key;
            break;
        }
    }

    return type;
}

void CDiagramFileManager::registerMeta(const QString &type, QMetaObject *obj)
{
    if (!m_metaObjects.contains(type))
    {
        m_metaObjects[type] = obj;
    }
}

void CDiagramFileManager::patch(QString &id, QByteArray &data)
{
    QByteArray      md5;

    md5 = QCryptographicHash::hash(id.toAscii(), QCryptographicHash::Md5);
    data.prepend(md5);
}

void CDiagramFileManager::depatch(QByteArray &id, QByteArray &data)
{
    int     len = 16;

    // left 16 bytes is the id
    id = data.left(len);
    data.remove(0, len);
}
