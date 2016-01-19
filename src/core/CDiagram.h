/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_DIAGRAM_H
#define PRODRAW_DIAGRAM_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramPrivateData;
class CDiagramContext;
class CDiagramCommand;
class CDiagramItem;
class CDiagramText;
class CDiagramLayer;
class CDiagramTagLayer;
class CDiagramItemLayer;
class CDiagramGadgetLayer;
class CDiagramBackgroundLayer;
class CDiagramCommandBuilder;
class CDiagramEventHandler;

/**
 * CDiagram is the base class providing the logic and storage
 * of a diagram's data.
 */
class CORE_EXPORT CDiagram: public QGraphicsScene
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagram();
	virtual ~CDiagram();

	// Different layers of diagram
	enum DiagramLayer {
		BackgroundLayer = 0x1,
		ItemLayer = 0x2,
		TagLayer = 0x3,
		HandlerLayer = 0x4,
		ControlLayer = 0x5,
		SpriteLayer = 0x6
	}; // End of enumeration of diagram layers

	// Different mode of a diagram
	enum DiagramMode {
		UserMode = 0x0,
		TagMode = 0x1,
		CursorMode = 0x2,
		EditTextMode = 0x3,
		AddSingleItemMode = 0x4,
		AddMultipleItemMode = 0x5
	}; // End of mode enumeration

	QGraphicsView* getMainView();

	void setMode(CDiagram::DiagramMode mode);
	CDiagram::DiagramMode mode() const;

	void setContext(CDiagramContext *cxt);
	CDiagramContext* context();

	void setContextMenu(int mode, QMenu *menu);
	QMenu* contextMenu(int mode);

	// Item manipulation
	void insertItem(CDiagramItem *item, const QPointF &pos, bool needNewId = true, bool useOldPos = false);
	void deleteItem(CDiagramItem *item);

	// add/remove gadgets
	// Gadgets can only be added to the layers above the item layer
	void addGadget(QGraphicsItem *item, const DiagramLayer layer);
	void removeGadget(QGraphicsItem *item, const DiagramLayer layer);

	CDiagramItem* itemUnderPressed();
	CDiagramText* textUnderEditing();

	CDiagramItem* selectedTopLevelItem();
	CDiagramItem* getItemById(int id);
	CDiagramItem* getItemAt(const QPointF& pos);

	QList<CDiagramItem*> topLevelItems(Qt::SortOrder order = Qt::DescendingOrder);
	QList<CDiagramItem*> selectedTopLevelItems();
	QList<CDiagramItem*> getItemsHasFocus();

	QList<CDiagramItem*> getItemsIsObscuredBy(CDiagramItem *item);
	QList<CDiagramItem*> getItemsIsObscuringTo(CDiagramItem *item);

	bool isTopLevelItem(CDiagramItem *item);

	// Item meta data is the item to be inserted when
	// mouse clicked.
	void setNewItemMeta(const QMetaObject *meta);
	QMetaObject* getNewItemMeta();

	void setNewItem(const CDiagramItem *item);
	CDiagramItem* getNewItem();

	// used by the items to add custom undo/redo command to
	// the diagram's undostack
	void addUndoCommand(CDiagramCommand *cmd);
	QUndoStack* undoStack();

	// used to render items to outside resources
	QRectF getItemsBoundingRect(bool filterSelected = false);

	void selectAll();
	void copy();
	void copyItems(const QList<CDiagramItem*> &items, QDomDocument &doc);

	void drawAllItems(QPainter *painter, const QRectF &target = QRectF());
	void drawSelectedItems(QPainter *painter, const QRectF &target  = QRectF());
	void drawArea(QPainter *painter, const QRectF &source, const QRectF &target =  QRectF());

	// scene rect related
	void setAutoExpand(bool value);
	bool isAutoExpand() const;

	void setAutoAlignment(bool value);
	bool isAutoAlignment() const;

	// Used to scale the items by factor,
	// it only affects the items layer.
	void setScale(qreal factor);
	qreal getScale();

	// page size setup routing 
	void setPageSize(qreal w, qreal h);
	void setPageSize(const QSizeF &size);
	const QSizeF getPageSize();

	QRectF printArea(bool onlySelectedItems = false);
	
    // background related
	bool isGridVisible() const;
	void setGridVisible(bool value);

	int gridSize() const;
	void setGridSize(int r);

	bool snapToGrid() const;
	void setSnapToGrid(bool snap);

  void setBackground(const QBrush &brush);
  QBrush background() const;

	int rotateStep() const;
	void setRotateStep(int r);

	// layer manipulation
	void addLayer(CDiagramLayer *layer);
	void insertLayer(int index, CDiagramLayer *layer);
	CDiagramLayer* getDefaultLayer(CDiagram::DiagramLayer layer);
	void removeLayer(CDiagramLayer *layer);
	void setHelperLayersVisible(bool value);

	void refreshStackOrder();

	virtual void toXml(QDomElement &e);
	virtual void fromXml(QDomElement &e);

Q_SIGNALS:
	void closing();
	void changed();
	void visibleRectChanged(const QRectF&);
	void scaleChanged(qreal);
	void modeChanged(CDiagram::DiagramMode);
	void editingText(CDiagramText*);
	void textChanged(CDiagramText*);
	void selectionAreaChanged(const QPainterPath &);

	void itemsMoving(qreal, qreal);
	void itemsDragging();
	void itemsDraggingStop(const QRectF &rect);
	void itemsResizing();
	void itemsResizingStop(const QRectF &rect);
	void itemsRotating();
	void itemsRotatingStop(qreal value);
	void itemsGeometryChanged(const QRectF &rect);
	void itemsRotationChanged(const qreal rotation);
	void itemsSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect);
	void itemsAdded(CDiagramItem *item);
	void itemsFailedAdding(CDiagramItem *item);
	void itemsRemoved(CDiagramItem *item);
	void itemsPropertyChanged(QList<CDiagramItem*> &items);
	void itemsLoadFinished();

	void userModeEvent(CDiagram *d, QEvent *event);

public Q_SLOTS:
	void emitClosing();
	void emitVisibleRectChanged();
	void emitItemsRotationChanged(qreal);
	void emitItemsGeometryChanged();
	void emitItemsDragging();
	void emitItemsDraggingStop();
	void emitItemsResizing();
	void emitItemsResizingStop();
	void emitItemsRotating();
	void emitItemsRotatingStop();
	void emitItemsMoving(qreal, qreal);
	void emitItemsAdded(CDiagramItem *item);
	void emitItemsLoadFinished();
	void emitSelectionChanged();

protected Q_SLOTS:
	void onSelectionChanged();
	void onSceneRectChanged(const QRectF &);
	void onCheckMouseActionTimeout();
	
protected:
	bool event(QEvent *event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
	
	bool requestEditingText(CDiagramText *text);
	void finishEditingText(CDiagramText *text, const QString &oldText, const QRectF &oldRect);

private:
	void expandSceneRect(qreal step = 100);
	void shrinkSceneRect(qreal step = 100);

	void initDefaultLayers();

	int acquireItemId();
	void releaseItemId(int id);
	void updateIds();

	QRectF selectedItemsBoundingRect();
	QPainterPath selectedItemsShape();

	bool isMouseClickingItem(const QPointF &p) const;
	bool isMouseClickingGadget(const QPointF &p) const;

	CDiagramItem* getTopLevelParentItem(QGraphicsItem *item);

	void startRubberBandSelection(const QPointF &p);
	void updateRubberBandSelection(const QPointF &p);
	void finishRubberBandSelection(const QPointF &p);

private:
	DiagramMode					 m_mode;
	CDiagramContext				 *m_context;

	CDiagramPrivateData			 *m_pdata;
	QUndoStack					 *m_undoStack;

	CDiagramBackgroundLayer		 *m_background; // The background layer
	CDiagramGadgetLayer			 *m_handlerLayer;	// The handle layer
	CDiagramGadgetLayer			 *m_controlLayer;	// The control layer
	CDiagramItemLayer			 *m_layer;		// The current layer under editing
	CDiagramTagLayer			 *m_tagLayer;   // The tag layer, for presentation
	QList<CDiagramLayer*>		 m_layers;		// all the layers, for future usage
	
	QList<int>					 m_ids;			// Ids used to assigned to the items
	QList<int>					 m_allocatedIds;
	int							 m_idStep;

	int							 m_padding;

	bool						 m_autoExpand;
	bool						 m_autoAlignment;
	QSizeF						 m_pageSize;		
	qreal						 m_scale;
	QRectF						 m_printArea;

	bool						 m_snapToGrid;
	int							 m_gridSize;
	int							 m_rotateStep;

	CDiagramItem				 *m_newItem;
	QMetaObject					 *m_newItemMeta;
	CDiagramText				 *m_textUnderEditing;
	qreal						 m_textRotation;

	bool						 m_isMouseAction;

	QGraphicsRectItem			 *m_rubberBand;
	QPointF						 m_rubberBandStartPosition;
	int							 m_selectedItemsCount;

	QMap<int, QMenu*>			 m_contextMenus;

	friend class CDiagramText;
	friend class CDiagramItem;
	friend class CDiagramEditor;

}; // end of class CDiagram

class CDiagramPrivateData
{
public:
	CDiagramPrivateData(CDiagram *d)
		:m_d(d),
	     m_pressedItem(NULL),
		 m_scrollThreshold(48),
		 m_scrollStep(8),
		 m_snapThreshold(3),
		 m_customPressedItem(NULL)
	{
	}

	qreal						 m_scrollThreshold;
	qreal						 m_scrollStep;
	qreal						 m_snapThreshold;

	CDiagram					 *m_d;
	CDiagramItem				 *m_pressedItem;
	QList<CDiagramItem*>		 m_draggedItems;
	QGraphicsItem				 *m_customPressedItem;
	QList<QGraphicsItem*>		 m_customDraggedItems;

}; // End of struct CDiagramPrivateData

#endif // PRODRAW_DIAGRAM_H
