/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPNODESPRITE_H
#define PRODRAW_CMINDMAPNODESPRITE_H

#include <QtGui>
#include <QtXml>

class CMindMapNode;
class CMindMapNodeSprite;
class CMindMapNodeSpriteSizer;
class CMindMapPixmapSprite;
class CMindMapIconSprite;
class CMindMapFileSprite;
class CMindMapLinkSprite;
class CMindMapAudioSprite;
class CMindMapTaskSprite;

class CMindMapNodeSpriteSizer: public QGraphicsItem
{
public:
	CMindMapNodeSpriteSizer(CMindMapNodeSprite *sprite);
	virtual ~CMindMapNodeSpriteSizer();

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	CMindMapNodeSprite			*m_sprite;
	qreal						m_size;
	bool						m_pressed;

}; // End of class CMindMapNodeSpriteSizer

class CMindMapNodeSprite : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(int order READ order WRITE setOrder)
    Q_PROPERTY(int spriteType READ spriteType)
    Q_PROPERTY(QSizeF size READ size WRITE setSize)
    Q_PROPERTY(int spriteId READ spriteId WRITE setSpriteId)

public:
    CMindMapNodeSprite(CMindMapNode *node,
                       int order = 0);
    virtual ~CMindMapNodeSprite();

    QRectF contentRect() const;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setOrder(int value);
    int order() const;

    void setSize(const QSizeF &value);
    QSizeF size() const;

    void setSpriteId(int value);
    int spriteId();

	virtual void startDragging(CMindMapNodeSpriteSizer *sizer);
	virtual void doDragging(CMindMapNodeSpriteSizer *sizer);
	virtual void finishDragging(CMindMapNodeSpriteSizer *sizer);

    virtual void fromXml(const QDomElement &n);
    virtual void toXml(QDomElement &n);

    // sprite type
    virtual int spriteType() const;
    CMindMapNode* node();

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    virtual QMenu* contextMenu();
    virtual void triggered();

	virtual void layoutContent();

    virtual QStringList filterProperties();

    CMindMapNode        *m_node;
    int                 m_order;
    int                 m_spriteId;
    qreal               m_width;
    qreal               m_height;
    bool                m_hovered;

}; // End of class CMindMapNodeSprite

class CMindMapPixmapSprite : public CMindMapNodeSprite
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
    Q_PROPERTY(bool embeded READ embeded WRITE setEmbeded)

public:
    CMindMapPixmapSprite(CMindMapNode *node);
    virtual ~CMindMapPixmapSprite();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setPixmapPath(const QString &filePath);
    void setPixmap(const QPixmap &value);

	// pixmap must be const reference for it to be correctly
	// stored using datastream and << operator
    const QPixmap& pixmap();

    void setEmbeded(bool value);
    bool embeded() const;

    void restore();

    int spriteType() const;

	void startDragging(CMindMapNodeSpriteSizer *sizer);
	void doDragging(CMindMapNodeSpriteSizer *sizer);
	void finishDragging(CMindMapNodeSpriteSizer *sizer);

protected:
    virtual QMenu* contextMenu();
    virtual void triggered();
	virtual void layoutContent();
	QVariant itemChange(GraphicsItemChange change, const QVariant &value); 
	void updateInfo();
    QStringList filterProperties();

	qreal						m_defaultSize;
	qreal						m_defaultRatio;
	qreal						m_minSize;
	QSizeF						m_oldSize;
    QPixmap						m_pixmap;
    bool						m_embeded;
	CMindMapNodeSpriteSizer		*m_sizer;

}; // End of class CMindMapPixmapSprite

class CMindMapIconSprite : public CMindMapNodeSprite
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(int iconIndex READ iconIndex WRITE setIconIndex)

public:
    CMindMapIconSprite(CMindMapNode *node, QIcon icon = QIcon(), int index = 0);
    virtual ~CMindMapIconSprite();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setIcon(const QIcon &value);
    QIcon icon() const;

    void setIconIndex(int value);
    int iconIndex() const;

    int spriteType() const;

protected:
    virtual QMenu* contextMenu();
    virtual void triggered();
    QStringList filterProperties();

    QIcon         m_icon;
    int           m_iconIndex;

}; // End of class CMindMapIconSprite

class CMindMapLinkSprite : public CMindMapIconSprite
{
    Q_OBJECT
    Q_PROPERTY(QUrl link READ link WRITE setLink)
    Q_PROPERTY(int linkType READ linkType WRITE setLinkType)

public:
    enum LinkTypes {
        Web,
        Email,
        Topic,
        SubGraph
    }; // End of enumeration

    CMindMapLinkSprite(CMindMapNode *node,
                       QUrl link = QUrl(),
                       int linkType = CMindMapLinkSprite::Web);
    virtual ~CMindMapLinkSprite();

    void setLink(QUrl value);
    QUrl link() const;

    void setLinkType(int value);
    int linkType() const;

    int spriteType() const;

protected:
    virtual QMenu* contextMenu();
    virtual void triggered();
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QStringList filterProperties();

    QUrl            m_link;
    int             m_linkType;

}; // End of class CMindMapLinkSprite

class CMindMapFileSprite : public CMindMapIconSprite
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ files WRITE setFiles)
    Q_PROPERTY(QStringList fileTypes READ fileTypes WRITE setFileTypes)
    Q_PROPERTY(QStringList fileEmbedMarkers READ fileEmbedMarkers WRITE setFileEmbedMarkers)

public:
    CMindMapFileSprite(CMindMapNode *node);
    virtual ~CMindMapFileSprite();

    enum FileTypes {
        LocalFile,
        RemoteFile
    }; // End of enumeration FileTypes

    bool addFile(const QString &value, bool embeded = false);
	bool deleteFile(const QString &value);
	bool deleteFile(int index);
	
	void setFiles(const QStringList &value);
	QStringList files() const;
	
    void setFileTypes(const QStringList &value);
    QStringList fileTypes() const;

    void setFileEmbedMarkers(const QStringList &value);
    QStringList fileEmbedMarkers() const;

    int spriteType() const;

protected:
    virtual QMenu* contextMenu();
    virtual void triggered();
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QStringList filterProperties();

    QStringList     m_files;
	QList<int>		m_fileTypes;
	QList<bool>		m_embedMarkers;

}; // End of class CMindMapFileSprite

class CMindMapNoteSprite : public CMindMapIconSprite
{
    Q_OBJECT
    Q_PROPERTY(QString note READ note WRITE setNote)

public:
    CMindMapNoteSprite(CMindMapNode *node);
    virtual ~CMindMapNoteSprite();

    void setNote(const QString &value);
    QString note() const;

    int spriteType() const;

protected:
    virtual QMenu* contextMenu();
    virtual void triggered();
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QStringList filterProperties();

    QString         m_note;

}; // End of class CMindMapNoteSprite

class CMindMapTaskSprite : public CMindMapNodeSprite
{
    Q_OBJECT
    Q_PROPERTY(QString assignedTo READ assignedTo WRITE setAssignedTo)
    Q_PROPERTY(QDateTime dueDate READ dueDate WRITE setDueDate)

public:
    CMindMapTaskSprite(CMindMapNode *node);
    virtual ~CMindMapTaskSprite();

    enum DataTypes {
        DueDate,
        Assignee
    }; // End of enumeration

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setAssignedTo(const QString &value);
    QString assignedTo();

    void setDueDate(const QDateTime &date);
    QDateTime dueDate() const;
    void setUseTime(bool value);
    bool useTime();
    QString dueDateString();

    bool isNull() const;

    int spriteType() const;

protected:
    QSizeF preferredSize();
    QStringList filterProperties();
	void triggered();

    QString             m_assignedTo;
    QDateTime           m_dueDate;
    QFont               m_font;
    bool                m_useTime;

}; // End of class CMindMapTaskSprite

class CMindMapTaskPrioritySprite : public CMindMapIconSprite
{
	Q_OBJECT
	Q_PROPERTY(int priority READ priority WRITE setPriority)

public:
	CMindMapTaskPrioritySprite(CMindMapNode *node, int priority = 0);
	virtual ~CMindMapTaskPrioritySprite();

	void setPriority(int value);
	int priority();

	int spriteType() const;

protected:
	QStringList filterProperties();
	void triggered();
	QMenu* contextMenu();

	int					m_priority;
	int					m_maxPriority;

}; // End of class CMindMapTaskPrioritySprite

class CMindMapTaskProgressSprite : public CMindMapIconSprite
{
	Q_OBJECT
	Q_PROPERTY(int progress READ progress WRITE setProgress)

public:
	CMindMapTaskProgressSprite(CMindMapNode *node, int progress  = 0);
	virtual ~CMindMapTaskProgressSprite();

	void setProgress(int value);
	int progress();

	int spriteType() const;

protected:
	QStringList filterProperties();
	void triggered();
	QMenu* contextMenu();

	int				m_progress;
	int				m_maxProgressNo;

}; // End of class CMindMapTaskProgressSprite

#endif // PRODRAW_CMINDMAPNODESPRITE_H
