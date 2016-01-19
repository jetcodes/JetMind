/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMFILEMANAGER_H
#define PRODRAW_CDIAGRAMFILEMANAGER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramFile;
class CDiagramFileManager;

class CORE_EXPORT CDiagramFileManager: public QObject
{
    Q_OBJECT

public:
    CDiagramFile* create(const QString type="Core");
    CDiagramFile* open(const QString &fullPath);
	bool isOpened(const QString &fullPath);

    void addFile(CDiagramFile *file);
    void closeFile(CDiagramFile *file);

    CDiagramFile* getCurrentFile();
    CDiagramFile* getFileById(int id);
	void setCurrentFile(CDiagramFile *file);
    CDiagramFile* getNextFile(int step = 1);

	QAbstractItemModel* infoModel();

	void updateFileInfo(CDiagramFile *file);

    bool hasUnsavedFile();
    bool closeAllFiles();
	void saveAllFiles();
	void restoreAllFiles();
    QList<CDiagramFile*> files();
    int fileCount();

	void registerMeta(const QString &type, QMetaObject *obj);

	void patch(QString &id, QByteArray &data);
    void depatch(QByteArray &id, QByteArray &data);

    static CDiagramFileManager* instance()
    {
        if (!m_fileManager)
            m_fileManager = new CDiagramFileManager();
        return m_fileManager;
    }

Q_SIGNALS:
	void fileCountChanged(int);

private:
    Q_DISABLE_COPY(CDiagramFileManager)
    CDiagramFileManager();
	int	nextFileId();
    QString getFileType(const QString &fullPath);

    int                             m_nextFileId;
	int								m_currentIndex;
    QStandardItemModel              *m_infoModel;
    QList<CDiagramFile*>            m_files;
	QMap<QString, QMetaObject*>		m_metaObjects;

    static CDiagramFileManager      *m_fileManager;

}; // End of class CDiagramFileManager

#endif // PRODRAW_CDIAGRAMFILEMANAGER_H
