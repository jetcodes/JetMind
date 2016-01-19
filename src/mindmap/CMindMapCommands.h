/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef	PRODRAW_CMINDMAPCOMMANDS_H
#define PRODRAW_CMINDMAPCOMMANDS_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramCommand>

class CMindMapNode;
class CMindMapNodeGroup;
class CMindMapNodeSprite;

class CMindMapDraggingCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapDraggingCommand(CDiagram *d,
					CDiagramItem *item, 
					QPointF &p,
					QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	CDiagramItem		*m_item;
	QPointF				m_newPosition;
	QPointF				m_oldPosition;

}; // End of class CMindMapDraggingCommand


class CMindMapChangePropertyCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapChangePropertyCommand(CDiagram *d,
                           const QList<CDiagramItem*> nodes,
						   const char *name, 
						   const QVariant &value,
						   QUndoCommand *parent = 0);
	Q_INVOKABLE CMindMapChangePropertyCommand(CDiagram *d,
                           const QList<CDiagramItem*> nodes,
						   const char *name, 
						   const QList<QVariant> &values,
						   QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
    QList<CDiagramItem*>	m_nodes;
	QList<QVariant>			m_oldValues;
	QList<QVariant>			m_newValues;
	const char				*m_name;

}; // End of class CChangePropertyCommand

class CMindMapNewNodeCommand: public CDiagramCommand
{
	Q_OBJECT

public:
    Q_INVOKABLE CMindMapNewNodeCommand(CDiagram *d,
                           CMindMapNode *parentNode,
                           CMindMapNode *refNode = NULL,
                           bool beforeRef = false,
                           bool duplicateStyle = false,
                           QUndoCommand *parent = 0);

	void undo();
	void redo();

private:
	CMindMapNode		*m_newNode;
	CMindMapNode		*m_parentNode;
	CMindMapNode		*m_refNode;
	bool				m_beforeRef;
    bool                m_duplicateStyle;

}; // End of class CMindMapNewNodeCommand

class CMindMapDelNodeCommand:  public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapDelNodeCommand(CDiagram *d,
						   QList<CMindMapNode*>	&delNodes,
						   QUndoCommand *parent = 0);

	void undo();
	void redo();

private:
    QList<CMindMapNode*>                        m_parentNodes;
    QList<QList<CMindMapNodeGroup*> >			m_groups;
    QList<QList<int> >						    m_groupMarkers;
    QList<CMindMapNode*>                        m_delNodes;
    QList<int>                                  m_indexes;

}; // End of class CMindMapDelNodeCommand

class CMindMapReparentCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapReparentCommand(CDiagram *d,
						   CMindMapNode *node,
						   CMindMapNode *newParent,
                           int          index = -1,
						   QUndoCommand *parent = 0);

	void undo();
	void redo();

private:
    int                                 m_oldIndex;
    int                                 m_newIndex;
    CMindMapNode                        *m_node;
    CMindMapNode                        *m_oldParent;
    CMindMapNode                        *m_newParent;
    QList<CMindMapNodeGroup*>			m_groups;
    QList<int>                          m_groupMarkers;

}; // End of class CMindMapReparentCommand

class CMindMapReorderCommand: public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapReorderCommand(CDiagram *d,
                                      CMindMapNode *node,
                                      int oldIndex,
                                      int newIndex,
                                      QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    int							m_oldIndex;
    int							m_newIndex;
    CMindMapNode				*m_node;
	QList<CMindMapNodeGroup*>	m_groups;
	QList<int>					m_groupStartIndexes;
	QList<int>					m_groupEndIndexes;
}; // End of class CMindMapReorderCommand

class CMindMapInsertParentCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMindMapInsertParentCommand(CDiagram *d,
						   CMindMapNode *childNode,
						   CMindMapNode *newParentNode,
						   QUndoCommand *parent = 0);

	void undo();
	void redo();

private:
	int					m_oldIndex;
	CMindMapNode		*m_newParentNode;
	CMindMapNode		*m_oldParentNode;
	CMindMapNode		*m_childNode;

}; // End of class CMindMapInsertParentCommand

class CMindMapAddGroupCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapAddGroupCommand(CDiagram *d,
                                        CMindMapNode *node,
                                        int startIndex,
                                        int endIndex,
                                        int groupType,
                                        int styleType,
                                        QUndoCommand *parent = 0);
    void undo();
    void redo();

private:
    int                 m_startIndex;
    int                 m_endIndex;
    int                 m_groupType;
    int                 m_styleType;
    CMindMapNode        *m_node;
    CMindMapNodeGroup   *m_group;

}; // End of class CMindMapAddGroupCommand

class CMindMapDropGroupCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapDropGroupCommand(CDiagram *d,
                                         CMindMapNodeGroup *group,
                                         QUndoCommand *parent = NULL);
    Q_INVOKABLE CMindMapDropGroupCommand(CDiagram *d,
                                         QList<CMindMapNodeGroup*> &groups,
                                         QUndoCommand *parent = NULL);

    void undo();
    void redo();

private:
    QList<CMindMapNodeGroup*>       m_groups;
    QList<CMindMapNode*>            m_nodes;

}; // End of class CMindMapDropGroupCommand

class CMindMapAddSpriteCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapAddSpriteCommand(CDiagram *d,
                                         QList<CMindMapNode*> &nodes,
                                         int spriteType,
                                         QVariant   val1 = QVariant(),
                                         QVariant   val2 = QVariant(),
                                         QVariant   val3 = QVariant(),
                                         QUndoCommand *parent = NULL);

    void undo();
    void redo();

private:
    int                          m_spriteType;
    QList<bool>                  m_replaceMarks;
    QList<CMindMapNode*>         m_nodes;
    QList<CMindMapNodeSprite*>   m_sprites;
    QList<QVariant>              m_val1Records;
    QList<QVariant>              m_val2Records;
    QList<QVariant>              m_val3Records;
    QVariant                     m_val1;
    QVariant                     m_val2;
    QVariant                     m_val3;
}; // End of class CMindMapAddSpriteCommand

class CMindMapRemoveSpriteCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapRemoveSpriteCommand(CDiagram *d,
                                            QList<CMindMapNode*> &nodes,
                                            int spriteType,
                                            bool ctrl = true,
                                            QVariant val = QVariant(),
                                            QUndoCommand *cmd = NULL);

    void undo();
    void redo();

private:
    int                                     m_spriteType;
    bool                                    m_ctrl;
    QList<CMindMapNode*>                    m_nodes;
    QList<QList<CMindMapNodeSprite*> >      m_sprites;

    QVariant                                m_val;
    QList<QVariant>                         m_valRecords;

}; // End of class CMindMapRemoveSpriteCommand

class CMindMapNodeSpriteResizeCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapNodeSpriteResizeCommand(CDiagram *d,
                                                CMindMapNodeSprite *sprite,
                                                QSizeF oldSize,
                                                QSizeF newSize,
                                                QUndoCommand *parent = NULL);
    void undo();
    void redo();

private:
    int                     m_spriteType;
    QSizeF                  m_newSize;
    QSizeF                  m_oldSize;
    CMindMapNodeSprite      *m_sprite;

}; // End of class CMindMapNodeSpriteResizeCommand

class CMindMapPasteCommand : public CDiagramCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE CMindMapPasteCommand(CDiagram *d,
                                     QList<CMindMapNode*> &nodes,
                                     QUndoCommand *parent = NULL);

    void undo();
    void redo();

private:
    QList<CMindMapNode*>        m_nodes;
    QList<CMindMapNode*>        m_newNodes;
    QString                     m_text;
    QImage                      m_image;
    QPixmap                     m_pixmap;

}; // End of class CMindMapPasteCommand

#endif // PRODRAW_CMINDMAPCOMMANDS_H
