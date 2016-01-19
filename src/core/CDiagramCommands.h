/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMCOMMANDS_H
#define PRODRAW_CDIAGRAMCOMMANDS_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramCommand>

class CDiagram;
class CDiagramItem;
class CDiagramGroup;
class CDiagramPort;
class CDiagramEdge;
class CDiagramEdgeStyle;
class CDiagramEdgeDragger;
class CDiagramArrow;

class CORE_EXPORT CNewItemCommand: public CDiagramCommand
{
	Q_OBJECT
public:
	Q_INVOKABLE CNewItemCommand(CDiagram *d,
					CDiagramItem *item, 
					const QPointF &p = QPointF(), 
					QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QPointF			m_pos;
	CDiagramItem	*m_item;

}; // End of class CNewItemCommand

class CORE_EXPORT CDeleteItemCommand: public CDiagramCommand
{
	Q_OBJECT

public:
    Q_INVOKABLE CDeleteItemCommand(CDiagram *d, const QList<CDiagramItem*> &items, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	void deleteEdge(CDiagramEdge *edge);

	QList<CDiagramEdge*>	m_edges;
	QList<CDiagramItem*>	m_items;
	QMap<CDiagramItem*, CDiagramItem*>		   m_parents;
	QMap<CDiagramEdge*, QList<CDiagramPort*> > m_startMaps;
	QMap<CDiagramEdge*, QList<CDiagramPort*> > m_endMaps;

}; // End of CDeleteItemCommand

class CORE_EXPORT CGroupCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CGroupCommand(CDiagram *d, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	CDiagramGroup			*m_group;
	QList<CDiagramItem*>	m_items;

}; // End of class CGroupCommand

class CORE_EXPORT CUnGroupCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CUnGroupCommand(CDiagram *d, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	CDiagramGroup			*m_group;
	QList<CDiagramItem*>	m_items;

}; // End of CUnGroupCommand

class CORE_EXPORT CRotateCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CRotateCommand(CDiagram *d, const QPointF &origin, qreal delta, QUndoCommand *parent = 0);
	void undo();
	void redo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	QPointF					m_origin;
	qreal					m_delta;
	QList<CDiagramItem*>	m_items;
	QList<QPointF>			m_oldPos;

}; // End of class CRotateCommand

class CORE_EXPORT CResizeCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CResizeCommand(CDiagram *d, 
				   const QList<CDiagramItem*> &items,
				   const QList<QRectF> &oldGeometry,
				   const QList<QRectF> &newGeometry,
				   QUndoCommand *parent = 0);
	void undo();
	void redo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

protected:
	QList<QRectF>			m_oldGeometry;
	QList<QRectF>			m_newGeometry;
	QList<CDiagramItem*>	m_items;

}; // End of CResizeCommand

class CORE_EXPORT CDraggingItemsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CDraggingItemsCommand(CDiagram *d, 
									  QList<CDiagramItem*> &selectedItems, 
									  QList<QPointF> &newPositions, 
									  QUndoCommand *parent = 0);
	void redo();
	void undo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	QList<QPointF>				m_oldPositions;
	QList<QPointF>				m_newPositions;
	QList<CDiagramItem*>		m_items;

}; // End of class CDraggingItemsCommand

class CORE_EXPORT CMoveItemsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CMoveItemsCommand(CDiagram *d, 
                        const QList<CDiagramItem*> &selectedItems,
						qreal dx, qreal dy, 
						QUndoCommand *parent = 0);
	void redo();
	void undo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	qreal						m_dx;
	qreal						m_dy;
	QList<CDiagramItem*>		m_items;

}; // End of class CMoveItemsCommand

class CORE_EXPORT CAdjustRouteCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CAdjustRouteCommand(CDiagram *d, 
						CDiagramEdge *item, 
						CDiagramEdgeDragger *dragger, 
						QUndoCommand *parent = 0);
	void undo();
	void redo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	QPointF						m_newPos;
	QPointF						m_oldPos;
	QPointF						m_startPoint;
	QPointF						m_endPoint;
	bool						m_xAnchored[2];
	bool						m_yAnchored[2];
	CDiagramEdge		*m_edge;
	CDiagramEdgeDragger	*m_dragger;

}; // End of CAdjustRouteCommand

class CORE_EXPORT CReconnectCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CReconnectCommand(CDiagram *d, 
					  CDiagramEdge *item, 
					  CDiagramEdgeDragger *dragger, 
					  CDiagramPort *n, CDiagramPort *o, 
					  QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	CDiagramEdge		*m_edge;
	CDiagramEdgeDragger	*m_dragger;
	CDiagramPort		*m_oldPort;
	CDiagramPort		*m_newPort;

}; // End of class CReconnectCommand

class CORE_EXPORT CChangePropertyCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangePropertyCommand(CDiagram *d,
						   const QList<CDiagramItem*> items,
						   const char *name, 
						   const QVariant &value, 
						   QUndoCommand *parent = 0);

	Q_INVOKABLE CChangePropertyCommand(CDiagram *d,
						   const QList<CDiagramItem*> items,
						   const QList<QVariant> &values,
						   const char *name,
						   QUndoCommand *parent = 0);

	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;
	QList<QVariant>			m_oldValues;
	QList<QVariant>			m_newValues;
	const char				*m_name;
	const QVariant			m_value;

}; // End of class CChangePropertyCommand

class CORE_EXPORT CChangeRouterCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeRouterCommand(CDiagram *d, 
						 const QList<CDiagramEdge*> &edges, 
						 const CDiagramEdgeStyle *style, 
						 QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramEdge*>		m_edges;
	QList<CDiagramEdgeStyle*>	m_oldStyles;
	const CDiagramEdgeStyle		*m_newStyle;

}; // End of class CDiagramConnectionItemRouterCommand

class CORE_EXPORT CChangeStartArrowCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeStartArrowCommand(CDiagram *d, 
							 const QList<CDiagramEdge*> &conns, 
							 const CDiagramArrow *style, 
							 QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramEdge*>	m_edges;
	QList<CDiagramArrow*>	m_oldStyles;
	const CDiagramArrow		*m_newStyle;

}; // End of class CChangeStartArrowCommand

class CORE_EXPORT CChangeEndArrowCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeEndArrowCommand(CDiagram *d, 
						   const QList<CDiagramEdge*> &conns, 
						   const CDiagramArrow *style, 
						   QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramEdge*>			m_edges;
	QList<CDiagramArrow*>	m_oldStyles;
	const CDiagramArrow		*m_newStyle;

}; // End of class CChangeEndArrowCommand

class CORE_EXPORT CPasteCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CPasteCommand(CDiagram *d, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QString						m_text;
	QImage						m_image;
	QPixmap						m_pixmap;
	QMap<int, CDiagramItem*>	m_items;
	QList<CDiagramEdge*>		m_edges;

}; // End of class CPasteCommand

class CORE_EXPORT CDragCopyItemsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CDragCopyItemsCommand(CDiagram *d, QList<CDiagramItem*> &draggedItems, QList<QPointF> &newPositions, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>		m_draggedItems;
	QMap<int, CDiagramItem*>    m_items;
	QList<CDiagramEdge*>		m_edges;
	QList<QPointF>				m_newPositions;

}; // End of class CDragCopyItemsCommand

class CORE_EXPORT CDragNewItemCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CDragNewItemCommand(CDiagram *d, CDiagramItem* item, const QPointF &newPosition, QUndoCommand *parent = 0);
	Q_INVOKABLE CDragNewItemCommand(CDiagram *d, CDiagramItem* i1, CDiagramItem *i2, QUndoCommand *parent = 0);
	void undo();
	void redo();

protected:
	void addNodeRedo();
	void addNodeUndo();
	void addEdgeRedo();
	void addEdgeUndo();

private:
	CDiagramItem*			m_item;
	CDiagramItem*			m_item2;
	CDiagramEdge*			m_newEdge;
	CDiagramItem*			m_newItem;
	QPointF					m_newPosition;

}; // End of class CDragNewItemCommand

class CORE_EXPORT CAlignItemsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CAlignItemsCommand(CDiagram *d, Qt::Alignment align, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*> m_items;
	QList<QPointF>		 m_oldPos;
	QList<QPointF>		 m_newPos;
	Qt::Alignment		 m_align;

}; // End of class CAlignItemsCommand

class CORE_EXPORT CEqualItemsSizeCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CEqualItemsSizeCommand(CDiagram *d, Global::MatchSizeFlag flag, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;
	QList<QSizeF>			m_oldSize;
	Global::MatchSizeFlag   m_matchSizeflag;
	qreal					m_maxW;
	qreal					m_maxH;
	qreal					m_minW;
	qreal					m_minH;

}; // End of class CEqualItemsSizeCommand

class CORE_EXPORT CLayoutItemsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CLayoutItemsCommand(CDiagram *d, QList<CDiagramItem*> items, int mode);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>		m_items;
	QList<QPointF>				m_pos;
	int							m_mode;

}; // End of class CLayoutItemsCommand

class CORE_EXPORT CEditTextCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CEditTextCommand(CDiagram *d, CDiagramItem *item, CDiagramText *text, const QString &str, const QRectF &rect, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QString						m_newText;
	QString						m_oldText;
	QRectF						m_newRect;
	QRectF						m_oldRect;
	CDiagramItem				*m_item;
	CDiagramText				*m_text;
	bool						m_first;

}; // End of class CEditTextCommand

class CORE_EXPORT CSendToFrontCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CSendToFrontCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;

}; // End of class CSendToFrontCommand

class CORE_EXPORT CSendToBackCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CSendToBackCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;
	CDiagramItem*			m_sibling;

}; // End of class CSendToBackCommand

class CORE_EXPORT CBringForwardsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CBringForwardsCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;

}; // End of class CBringForwardsCommand

class CORE_EXPORT CBringBackwardsCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CBringBackwardsCommand(CDiagram *d, QList<CDiagramItem*> &items, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;

}; // End of class CBringBackwardsCommand

class CORE_EXPORT CChangeLabelPositionCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeLabelPositionCommand(CDiagram *d, CDiagramEdge *edge, int label, qreal pos, QUndoCommand *parent = 0);
	void undo();
	void redo();
	
	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	CDiagramEdge	*m_edge;
	int				m_label;
	qreal			m_newPos;
	qreal			m_oldPos;
}; // End of class CChangeLabelPositionCommand

class CORE_EXPORT CChangeLabelAnchorCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeLabelAnchorCommand(CDiagram *d, QList<CDiagramItem*> items, int anchor, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	QList<CDiagramItem*>	m_items;
	int						m_newAnchor;
	QList<int>				m_oldAnchors;

}; // End of class CChangeLabelAnchorCommand

class CORE_EXPORT CChangeConnectPointCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CChangeConnectPointCommand(CDiagram *d, CDiagramPort *port, CDiagramEdge *edge, qreal pos, QUndoCommand *parent = 0);
	void undo();
	void redo();

	bool mergeWith(const QUndoCommand *cmd);
	int id() const;

private:
	CDiagramPort		*m_port;
	CDiagramEdge		*m_edge;
	qreal				m_newPos;
	qreal				m_oldPos;
}; // End of class CChangeConnectPointCommand

class CORE_EXPORT CReplaceSymbolCommand: public CDiagramCommand
{
	Q_OBJECT

public:
	Q_INVOKABLE CReplaceSymbolCommand(CDiagram *d, CDiagramItem *oldItem, CDiagramItem *newItem, QUndoCommand *parent = 0);
	void undo();
	void redo();

private:
	CDiagramItem		*m_oldItem;
	CDiagramItem		*m_newItem;

}; // End of class CChangeLabelAnchorCommand

#endif // PRODRAW_CDIAGRAMCOMMANDS_H
