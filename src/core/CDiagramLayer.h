/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMLAYER_H
#define PRODRAW_CDIAGRAMLAYER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramItem;
class CDiagramItemLayer;
class CDiagramBackgroundLayer;

// CDiagramLayer is the base class of all kinds of layers in
// a diagram, inherited by CDiagramBackgroundLayer, CDiagramForegroundLayer
// CDiagramItemLayer
class CORE_EXPORT CDiagramLayer : public QGraphicsObject, public QGraphicsLayoutItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsLayoutItem)

public:
	CDiagramLayer(CDiagram *parent);
	virtual ~CDiagramLayer();

	enum { Type = UserType + Global::DiagramLayer };
	virtual int type() const { return Type; }

	// Overrided member functions inherited from the parent classes.
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void setGeometry( const QRectF &rect);
	void setSize(qreal w, qreal h);
	QSizeF size();

    void setBrush(const QBrush &value);
    QBrush brush() const;

    void setPen(const QPen &value);
    QPen pen() const;

protected:
	virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

	CDiagram			*m_p;

	QBrush				m_brush;
	QPen				m_pen;

	qreal				m_width;
	qreal				m_height;
};

 // CDiagramItemLayer it used to hold the items in the scene
 // It is different with the background/foreground layers
class CORE_EXPORT CDiagramItemLayer: public CDiagramLayer
{
public:
	CDiagramItemLayer(CDiagram *parent);
	virtual ~CDiagramItemLayer();

	void insertItem(CDiagramItem *item, const QPointF &pos);
	CDiagramItem* deleteItem(CDiagramItem *item);
	CDiagramItem* getItemById(int value);
	void changeStackOrder(CDiagramItem *item, int step, Qt::SortOrder order);

protected:
	void updateStackOrder();

	QMap<int, CDiagramItem*>		m_items;

}; // End of class CDiagramItemLayer

// CDiagramBackgroundLayer is used to provide the diagram with a
// background image, it can be white paper or grids.
class CORE_EXPORT CDiagramBackgroundLayer : public CDiagramLayer
{
public:
	CDiagramBackgroundLayer(CDiagram *parent, int tick);
	virtual ~CDiagramBackgroundLayer();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);
	
	void setGridColor(const QColor &color);
	QColor gridColor();
	
	void setTick(int tick);
	int tick();

	void setGridVisible(bool value);
	bool gridVisible();

protected:
	int			m_tick;
	qreal		m_padding;
	bool		m_gridVisible;
	QColor		m_gridColor;

}; // End of class CDiagramBackgroundLayer

class CORE_EXPORT CDiagramTag : public QGraphicsTextItem
{
	Q_OBJECT

public:
	CDiagramTag(QGraphicsItem *parent = 0);
	virtual ~CDiagramTag();

	enum { Type = UserType + 1000 };
	int type() { return Type; }

Q_SIGNALS:
	void lostFocus(CDiagramTag*);

 protected:
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
     void focusOutEvent(QFocusEvent *event);
     void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

}; // End of CDiagramTag

class CORE_EXPORT CDiagramTagLayer : public CDiagramLayer
{
	Q_OBJECT

public:
	CDiagramTagLayer(CDiagram *parent);
	virtual ~CDiagramTagLayer();

	enum EditMode {
		Scribble,
		Tagging,
		Cursor
	}; // End of EditMode

	QRectF boundingRect() const;
	QPainterPath shape() const;
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void setEditMode(CDiagramTagLayer::EditMode mode);
	CDiagramTagLayer::EditMode editMode();
	
	void clear();

public Q_SLOTS:
	void editorLostFocus(CDiagramTag *tag);

protected:
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent *event );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent *event );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent *event );
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );

	CDiagramTagLayer::EditMode	m_mode;

	QList<QGraphicsPathItem*>	m_lines;
	QList<CDiagramTag*>			m_tags;

	bool						m_scribbling;
	QPainterPath				m_currPath;
	QGraphicsPathItem			*m_currPathItem;

	QCursor						m_cursor;

}; // End of class CDiagramTagLayer

class CORE_EXPORT CDiagramGadgetLayer: public CDiagramLayer
{
public:
	CDiagramGadgetLayer(CDiagram *parent);
	virtual ~CDiagramGadgetLayer();

	void addGadget(QGraphicsItem *gadget);
	void removeGadget(QGraphicsItem *gadget);
	bool containsGadget(QGraphicsItem *gadget);

	void enterTemporaryHide();
	void exitTemporaryHide();

	void setGadgetsVisible(bool value);

protected:
	QList<QGraphicsItem*>		m_gadgets;
	QList<QGraphicsItem*>		m_visibleGadgets;

}; // End of class CDiagramGadgetLayer

#endif // PRODRAW_CDIAGRAMLAYER_H
