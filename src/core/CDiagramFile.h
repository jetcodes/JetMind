/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMFILE_H
#define PRODRAW_CDIAGRAMFILE_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>

class CDiagram;

class CORE_EXPORT CDiagramFile : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString version READ version WRITE setVersion)
	Q_PROPERTY(QString type READ type WRITE setType)
	Q_PROPERTY(QString author READ author WRITE setAuthor)
	Q_PROPERTY(QDateTime createdDate READ createdDate WRITE setCreatedDate)
  Q_PROPERTY(QDateTime lastUpdatedDate READ lastUpdatedDate WRITE setLastUpdatedDate)
	Q_PROPERTY(int revision READ revision WRITE setRevision)

public:
	CDiagramFile(const QString &type = "Unknown", 
				 const QString &fileName = "Untitle", 
				 const QString &path = "");
	virtual ~CDiagramFile();

	int id();
	void setId(int value);

  virtual QList<QUndoStack*> undoStacks();
  virtual void initDefault();

  void save();
  virtual void save(const QString &fullPath);
	virtual void save(QByteArray &data);

  void load();
  virtual void load(const QString &fullPath);
  virtual void load(QByteArray &data);

  // return the diagram to edit
  virtual CDiagram* getEditDiagram();
	virtual void setEditDiagram(CDiagram *d);

	// Properties
    void setPath(const QString &path);
    QString path();

    void setFileName(const QString &fileName);
    QString fileName();

    QString version();
    void setVersion(const QString &value);

    QString type();
    void setType(const QString &value);

    QString author();
    void setAuthor(const QString &value);

    QDateTime createdDate();
    void setCreatedDate(const QDateTime& value);

    QDateTime lastUpdatedDate();
    void setLastUpdatedDate(const QDateTime& value);

    int revision();
    void setRevision(int value);

    bool isUnsaved();
    void setUnsaved(bool value);

    bool hasPhysicalFile();
    bool isModifiedOutside();

    virtual void toXml(QDomDocument &doc);
    virtual void fromXml(const QDomDocument &doc);

	virtual QAbstractItemModel* dataModel();

Q_SIGNALS:
	void saveStatusChanged(bool);

public Q_SLOTS:
    void onContentChanged();

protected:
	bool serialize(QByteArray &data, const QString &fullPath);
	bool unserialize(const QString &fullPath, QByteArray &data);

    int                 m_id;

	QString				m_fileName;
    QString				m_path;
	bool				m_unsaved;
	bool				m_modifiedOutside;
	bool				m_hasPhysicalFile;
	bool				m_useCompress;

	QString				m_version;
	QString				m_type;
	QString				m_author;
	QDateTime			m_createdDate;
  QDateTime			m_lastUpdatedDate;
	int					m_revision;

private:
    Q_DISABLE_COPY(CDiagramFile)

}; // End of CDiagramFile

class CORE_EXPORT CDiagramBasicFile: public CDiagramFile
{
    Q_OBJECT
    Q_PROPERTY(QPointF viewCenter READ viewCenter WRITE setViewCenter)

public:
    Q_INVOKABLE CDiagramBasicFile();
    virtual ~CDiagramBasicFile();

    virtual void save(const QString &fullPath);
	virtual void save(QByteArray &data);

	virtual void load(const QString &fullPath);
    virtual void load(QByteArray &data);

    CDiagram* getEditDiagram();
	void setEditDiagram(CDiagram *d);

    void setViewCenter(const QPointF &value);
    QPointF viewCenter();

protected:
    CDiagram            *m_d;
	QPointF				m_viewCenter;

}; // End of class CDiagramBasicFile

#endif // PRODRAW_CDIAGRAMFILE_H
