/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPFILE_H
#define PRODRAW_CMINDMAPFILE_H

#include <QtGui>
#include <QtXml>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramFile>

class CMindMapNode;
class CMindMapFile;
class CMindMapGraph;

class CMindMapGraph: public CDiagram
{
	Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(int level READ level WRITE setLevel)
    Q_PROPERTY(QPointF viewCenter READ viewCenter WRITE setViewCenter)
    Q_PROPERTY(QSizeF size READ size WRITE setSize)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(int parentGraphId READ parentGraphId WRITE setParentGraphId)
    Q_PROPERTY(QStringList subGraphIds READ subGraphIds WRITE setSubGraphIds)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QImage backgroundTexture READ backgroundTexture WRITE setBackgroundTexture)

public:
    Q_INVOKABLE CMindMapGraph();
	CMindMapGraph(CMindMapFile *file, CMindMapGraph *parentGraph = NULL);
	virtual ~CMindMapGraph();

    int id();
    void setId(int id);

	int level();
	void setLevel(int value);

	void setViewCenter(const QPointF &value);
	QPointF viewCenter();

	void setSize(qreal w, qreal h);
	void setSize(QSizeF size);
	QSizeF size() const;

    void setTitle(const QString &value);
    QString title() const;

    void setHidden(bool value);
    bool hidden();

	void setFile(CMindMapFile *file);
	CMindMapFile* file();

    void setParentGraphId(int id);
	void setParentGraph(CMindMapGraph *graph);
    int parentGraphId() const;

    void setSubGraphIds(QStringList ids);
    QStringList subGraphIds() const;

    CMindMapGraph* parentGraph();
    CMindMapGraph* createSubGraph();
    void removeSubGraph(CMindMapGraph *graph);
    QList<CMindMapGraph*> subGraphes();

    QList<CMindMapNode*> getAllSaveNodes();

    QAbstractItemModel* dataModel(bool tree = false);
    void filterNodes(const QString &filter);
	void locateNode(int id);

    void setBackgroundTexture(const QImage &value);
    QImage backgroundTexture();

    void initDefault();
	
	void toXml(QDomElement &n);
    void fromXml(QDomElement &n);

public Q_SLOTS:
    void onRebuildGraph();
    void onItemAdded(CDiagramItem *);
    void onContentRectChanged(QRectF);

protected:
    Q_DISABLE_COPY(CMindMapGraph)

    int                             m_id;
    int								m_level;
	QPointF							m_viewCenter;
	QSizeF							m_size;
	QString							m_title;
    bool                            m_hidden;

    int                             m_parentGraphId;
    CMindMapGraph                   *m_parentGraph;

    QList<int>                      m_subGraphIds;
    QList<CMindMapGraph*>			m_subGraphes;

    QStandardItemModel              *m_itemModel;

    CMindMapFile                    *m_file;

    friend class CMindMapFile;

}; // End of CMindMapGraph

class CMindMapFile : public CDiagramFile
{
	Q_OBJECT

public:
    Q_INVOKABLE CMindMapFile(const QString &fileName = QString("Untitle Map"), const QString &filePath = QString(""));
	virtual ~CMindMapFile();

    int graphCount() const;
	void addGraph(CMindMapGraph *graph);
    void removeGraph(CMindMapGraph *graph);
    void moveGraph(int fromIndex, int toIndex);

	CMindMapGraph* getGraphById(int id);
    CMindMapGraph* getGraphByIndex(int index);
    CMindMapGraph* getGraphByTitle(const QString &value);
    QList<CMindMapGraph*> graphes() const;
    QStringList graphTitles() const;

    QList<QUndoStack*> undoStacks();
    void initDefault();

	// actions
	void save(const QString &fullPath);
	void save(QByteArray &data);

	void load(const QString &fullPath);
	void load(QByteArray &data);

	// get the current diagram
	CDiagram* getEditDiagram();
	void setEditDiagram(CDiagram *d);

	CMindMapGraph* createGraph();

	void toXml(QDomDocument &doc);
    void fromXml(const QDomDocument &doc);

	QAbstractItemModel* dataModel();

Q_SIGNALS:
    void loadStarted();
    void loadProgress(int);
    void loadFinished();
    void saveStarted();
    void saveProgress(int);
    void saveFinished();

public Q_SLOTS:
    void onGraphChanged();

protected:
    Q_DISABLE_COPY(CMindMapFile)
    
	CMindMapGraph* createSubGraph(CMindMapGraph *parentGraph);

    void addSubGraph(CMindMapGraph *graph);
    void removeSubGraph(CMindMapGraph *graph);
    CMindMapGraph* getSubGraphById(int value);

	void setNextSubGraphId(int value);
	int nextSubGraphId();

    void setNextGraphId(int value);
    int nextGraphId();

	CMindMapGraph					*m_currentGraph;

    QList<CMindMapGraph*>			m_graphes;
	int								m_nextGraphId;

	int                             m_nextSubGraphId;
	QMap<int, CMindMapGraph*>		m_subGraphes;

    friend class CMindMapGraph;

}; // End of class CMindMapFile

#endif // PRODRAW_CMINDMAPFILE_H
