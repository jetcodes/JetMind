/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPNODE_H
#define PRODRAW_CMINDMAPNODE_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CDiagramNode>
#include <core/CdiagramText>
#include <core/CDiagramTextNode>

#define NODE_NAME       "Start Topic"
#define NODE_CATEGORY   "Mind Map"

class CDiagram;
class CDiagramNode;
class CDiagramText;
class CDiagramTextNode;
class CMindMapNode;
class CMindMapNodeSprite;
class CMindMapNodeGroup;
class CMindMapNodeRenderer;
class CMindMapPathRenderer;
class CMindMapNodeCollapser;
class CMindMapNodeChildPanel;

class CMindMapText : public CDiagramTextNode
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapText(CDiagram *parent);

public Q_SLOTS:
	void onTextSizeAutoChanged();

}; // End of CMindMapText

class CMindMapNodeCollapser: public QGraphicsItem
{
public:
    CMindMapNodeCollapser(CMindMapNode *parent = NULL);
    virtual ~CMindMapNodeCollapser();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void setEdgePen(const QPen &value);
    QPen edgePen();

protected:
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    bool			m_opened;
    bool			m_hovered;
    qreal			m_size;
    CMindMapNode	*m_node;
    QPainterPath	m_edgePath;
    QPen			m_edgePen;

}; // End of class CMindMapNodeCollapser

class CMindMapNodeChildPanel : public QGraphicsItem
{
public:
    CMindMapNodeChildPanel(CMindMapNode *node);
    virtual ~CMindMapNodeChildPanel();
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updateSize();

    CMindMapNode* node() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    CMindMapNode        *m_node;
    qreal               m_width;
    qreal               m_height;
    QPainterPath        m_path;

}; // End of class CMindMapNodeChildPanel

class CMindMapNode: public CDiagramNode
{
	Q_OBJECT
	Q_PROPERTY(bool customPen READ customPen WRITE setCustomPen)
	Q_PROPERTY(QPen edgePen READ edgePen WRITE setEdgePen)
	Q_PROPERTY(bool customFontSize READ customFontSize WRITE setCustomFontSize)
    Q_PROPERTY(int level READ level WRITE setLevel)
	Q_PROPERTY(int nodeType READ nodeType WRITE setNodeType)
	Q_PROPERTY(int layoutType READ layoutType WRITE setLayoutType)
	Q_PROPERTY(int pathType READ pathType WRITE setPathType)
	Q_PROPERTY(QStringList childsId READ childsId WRITE setChildsId)
	Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily)
	Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)
	Q_PROPERTY(bool fontBold READ fontBold WRITE setFontBold)
	Q_PROPERTY(bool fontItalic READ fontItalic WRITE setFontItalic)
	Q_PROPERTY(bool fontUnderline READ fontUnderline WRITE setFontUnderline)
	Q_PROPERTY(QPointF collapserPos READ collapserPos WRITE setCollapserPos)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(int index READ index WRITE setIndex)
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)
    Q_PROPERTY(int nextSpriteId READ nextSpriteId WRITE setNextSpriteId)

public:
	Q_INVOKABLE CMindMapNode(CDiagram *d = NULL);
	virtual ~CMindMapNode();

	enum NodeTypes {
		Rect = 0x0,
		RoundedRect,
		Ellipse,
		Diamond,
		Underline,
		FishHeadLeft,
		FishHeadRight,
		Borderless,
		NoneType,
		DefaultType
	}; // End of NodeType

	enum LayoutTypes {
		MindMap = 0x0,
		ClockwiseMindMap,
		AntiClockwiseMindMap,
		DownsideOrganization,
		UpsideOrganization,
		LeftTree,
		RightTree,
		LeftLogic,
		RightLogic,
		LeftFishBone,
		RightFishBone,
		NoneLayout,
		DefaultLayout
	}; // End of LayoutType

	enum PathTypes {
		StraightLine = 0x0,
		CurveLine,
		ArrowLine,
		PolygonLine,
		RoundedLine,
		NoneLine,
		DefaultLine
	}; // End of enumeration PathType

    enum RelativePositions {
		North = 0x0,
		East,
		South,
		West,
		DefaultPosition
	}; // End of enumeration RelativePosition

    enum InsertPositions {
		InsertSiblingAfter = 0x0,
		InsertSiblingBefore,
		InsertParent,
		InsertChild,
		NoneInsertPosition
	}; // End of enumeration InsertPosition

    enum SpriteTypes {
        Pixmap = 0x0,
        Icon,
        File,
        Note,
        Link,
        Task,
        Tag,
		TaskProgress,
		TaskPriority,
        Undefined
    }; // End of enumeration spriteTypes

    enum GroupTypes {
        Boundary = 0x0,
        Summary
    }; // End of group types

	QRectF outlineRect() const;
	QRectF contentRect() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    void setAreaRect(const QRectF &value);
    QRectF areaRect();

	void setAreaSize(const QSizeF &value);
    QSizeF areaSize();

    QSizeF contentSize() const;

    QRectF printArea();

    QPainterPath connectArea();
    int connectIndex( const QPointF & );

	CMindMapNode* createChildNode();

    // child related methods
	void addChild(CMindMapNode *node, CMindMapNode *ref = NULL, bool beforeRef = false);
	void addChild(CMindMapNode *node, int index);
	void dropChild(CMindMapNode *node);
	QList<CMindMapNode*> childs();
    QList<CMindMapNode*> childs(int startIndex, int endIndex);
	CMindMapNode* childAt(int index);
	int childCount();
	int indexOf(CMindMapNode *node);
    void reorder(CMindMapNode *node, int index);

    // handle children's id generation
    QStringList childsId();
	void setChildsId(const QStringList &value);
    QList<int> childsIdArray();

    // sprites related methods
    void addSprite(CMindMapNodeSprite *sprite);
    void dropSprite(CMindMapNodeSprite *sprite);
    void dropSprites(int type);
    QList<CMindMapNodeSprite*> sprites(int type) const;
    CMindMapNodeSprite* getSpriteById(int value);
    int nextSpriteId();
    void setNextSpriteId(int value);

    // child node group related methods
    bool checkExistedGroup(int startIndex, int endIndex, int groupType);
    bool addNodeGroup(CMindMapNodeGroup *group);
    void dropNodeGroup(CMindMapNodeGroup *group);
    void dropAllNodeGroups();
    QList<CMindMapNodeGroup*> groups() const;
    QList<QList<int> > nodeRegions() const;

    // control whether the pen of the node
    // is customized
    void setCustomPen(bool value);
	bool customPen();
	QPen activePen() const;

    // edge pen
	void setEdgePen(const QPen &pen);
	QPen edgePen() const;

    // control whether the font size is customized
	void setCustomFontSize(bool value);
	bool customFontSize();

	void setCollapsed(bool value);
	bool isCollapsed() const;
	bool isCollapsible() const;

	void setUpLevelNode(CMindMapNode *node);
	CMindMapNode* upLevelNode();
	bool isDescendantOf(CMindMapNode *node);

	CMindMapNodeRenderer* nodeRenderer() const;
	CMindMapPathRenderer* pathRenderer() const;

	QPainterPath nodeShape() const;

	void setNodeType(int value);
	int nodeType() const;

	void setLayoutType(int layout);
	int layoutType() const;

	void setPathType(int path);
	int pathType() const;

	void setRelativePosition(int pos);
	int relativePosition() const;

	QGraphicsItem* collapser();
	void toggleVisible(bool value);

	// rendering related
	QSizeF recalc();
	void layout();
	
	void setLevel(int value);
	int level() const;

    void setIndex(int index);
    int index() const;

	bool isRoot() const;

	void setFontFamily(const QString &family);
	QString fontFamily() const;

	void setFontSize(int value);
	int fontSize() const;

	void setFontItalic(bool value);
	bool fontItalic() const;

	void setFontBold(bool value);
	bool fontBold() const;

	void setFontUnderline(bool value);
	bool fontUnderline() const;

	void setText(const QString &text);
	QString text();

	void startEditingContent();
	void finishEditingContent();

	bool isAnchorable() const;
	bool isAlignable() const;

    void setMovable(bool value);
    bool isMovable() const;

	void setCollapserPos(const QPointF &value);
	QPointF collapserPos() const;

Q_SIGNALS:
    void areaChanged(QRectF);

public Q_SLOTS:
	void onRebuildChilds();
	void onStructChanged();
	void onContentChanged(CDiagramText *text);
	void onPropertyChanged(const QString &name, const QVariant &value);
    void layoutContent();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event );
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event );
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	
    virtual QSizeF preferredSize() const;
	qreal xpadding() const;
	qreal ypadding() const;
    qreal hSpacing() const;
    qreal vSpacing() const;
	int nextNodeId();

    void extFromXml(const QDomElement &n);
    void extToXml(QDomElement &n);

private:
    // node customization related members
    QSizeF                          m_contentSize;

	int								m_shapeType;
	int								m_layoutType;
    int								m_pathType;

	CMindMapNode					*m_upLevelNode;
	QList<CMindMapNode*>			m_childs;
	QList<int>						m_childsId;
    int								m_maxNodeId;
    CMindMapNodeChildPanel          *m_childPanel;

    bool							m_collapsed;
    CMindMapNodeCollapser           *m_collapser;

    CDiagramTextNode				*m_editor;
	QString							m_text;
    qreal                           m_textWidth;
    QRectF                          m_textRect;
    bool							m_customFontSize;

    qreal							m_outlineStroke;
    QPainterPath					m_shape;
    bool							m_customPen;

    int                             m_index;
	int								m_level;
    QRectF                          m_areaRect;
    QSizeF							m_areaSize;
    int								m_relativePos;

    bool							m_hovered;

    int                             m_nextSpriteId;

    QMap<int, QList<CMindMapNodeSprite*> >  m_sprites;
    QList<CMindMapNodeGroup*>               m_groups;

}; // End of class CMindMapNode

#endif // PRODRAW_CMINDMAPNODE_H
