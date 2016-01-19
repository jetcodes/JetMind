/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPNODEGROUP_H
#define PRODRAW_CMINDMAPNODEGROUP_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramNode>

class CMindMapNode;
class CMindMapNodeGroup;
class CMindMapNodeBoundary;
class CMindMapNodeSummary;

class CMindMapNodeGroup : public CDiagramNode
{
    Q_OBJECT
    Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex)
    Q_PROPERTY(int endIndex READ endIndex WRITE setEndIndex)
    Q_PROPERTY(int groupType READ groupType)

public:
    CMindMapNodeGroup(CDiagram *d = 0, CMindMapNode *node = NULL);
    virtual ~CMindMapNodeGroup();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void relocate();

    void setNode(CMindMapNode *node);
    CMindMapNode* node();

    void setStartIndex(int index);
    int startIndex();

    void setEndIndex(int index);
    int endIndex();

	void setRelativePos(int value);
	int relativePos();

    int groupType() const;
    bool isEmpty() const;

public Q_SLOTS:
    void onAddChild(int index, int refIndex = -1);
    void onReorderChild(int from, int to);
    void onPropertyChanged(const QString&name, const QVariant&value);
	int onDeleteChild(int index);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual QMenu* contextMenu();

    int					m_groupType;
    int					m_startIndex;
    int					m_endIndex;
    qreal               m_stroke;
    QRectF				m_areaRect;
    CMindMapNode		*m_node;
	int					m_relativePos;
	QString				m_text;
    
}; // End of class CMindMapNodeGroup

class CMindMapNodeBoundary : public CMindMapNodeGroup
{
    Q_OBJECT
    Q_PROPERTY(int boundaryType READ boundaryType WRITE setBoundaryType)

public:
    CMindMapNodeBoundary(CDiagram *d, CMindMapNode *node = NULL);
    virtual ~CMindMapNodeBoundary();

    enum BoundaryTypes {
        Rect,
        RounededRect,
        Cloud,
        Note,
        Highlight
    }; // End of enumeration boundary types

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void relocate();

    void setBoundaryType(int value);
    int boundaryType() const;

public Q_SLOTS:
	void layoutContent();

protected:
    virtual QMenu* contextMenu();
    QStringList filterProperties();

    int             m_boundaryType;

}; // End of class CMindMapNodeBoundary

class CMindMapNodeSummary : public CMindMapNodeGroup
{
    Q_OBJECT
    Q_PROPERTY(qreal braceWidth READ braceWidth WRITE setBraceWidth)
    Q_PROPERTY(qreal size READ size WRITE setSize)
    Q_PROPERTY(QPainterPath path READ path WRITE setPath)

public:
    CMindMapNodeSummary(CDiagram *d, CMindMapNode *node = NULL);
    virtual ~CMindMapNodeSummary();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void relocate();

    void setBraceWidth(qreal value);
    qreal braceWidth() const;

    void setSize(qreal value);
    qreal size() const;

    void setPath(QPainterPath &value);
    QPainterPath path() const;

public Q_SLOTS:
	void layoutContent();

protected:
	qreal					m_braceWidth;
	qreal					m_size;
	QPainterPath			m_path;
	
}; // End of class CMindMapNodeSummary

#endif // PRODRAW_CMINDMAPNODEGROUP_H
