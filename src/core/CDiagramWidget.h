/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMWIDGET_H
#define PRODRAW_CDIAGRAMWIDGET_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>

class CDiagramItem;
class CDiagramTool;
class CDiagramWidget;
class CDiagramHostedWidget;

class CORE_EXPORT CDiagramTool : public QWidget
{
public:
	CDiagramTool(QWidget *parent = NULL);

	// check whether the items is supported by the
	// tool.
	virtual bool support(const QList<CDiagramItem*> &items);

	// init the tool according to the specified diagram status
	virtual void init(CDiagram *d);

	// clear things up
	virtual void uninit(CDiagram *d);

	// reset the tool
	virtual void reset();

	// manipulate the hosted widget
	void setHostWidget(CDiagramHostedWidget *w);
	CDiagramHostedWidget* hostWidget();

protected:
	CDiagram				*m_d;
	CDiagramHostedWidget	*m_p;
	int						m_priority;

}; // End of class CDiagramTool

class CORE_EXPORT CDiagramHostedWidget : public QWidget
{
	Q_OBJECT

public:
	CDiagramHostedWidget(QWidget *parent = NULL);
	virtual ~CDiagramHostedWidget();
	
	// check whether the hosted widget support the current
	// status of the diagram, if it does support, then it may be
	// show up.
	virtual bool support(CDiagram *d);

	// use to initialize the hosted widget using signal/slot
	// initialization. Each hosted widget will focus on different
	// signals sent by diagram.
	virtual void init(CDiagramWidget *parent, CDiagram *d);
	virtual void uninit(CDiagramWidget *parent, CDiagram *d);

	// used to reset the diagram tools to the preferred status
	// tools suitable for current editing action should be picked
	// out and show, while others are hidden
	virtual void reset();

	// used to manage the tools for the hosted widget.
	void addTool(CDiagramTool *tool);
	QList<CDiagramTool*> tools();

	// hosted widgets have priority for shown or not.
	void setPriority(int value);
	int priority();

	// the preferred position
	virtual bool customLocate();
	virtual QPointF preferredPos();

    CDiagramWidget* diagramWidget() const;

protected:
	// initialization of diagram tools, to be inherited by
	// subclasses.
	virtual void initTools();

	int						m_priority;
	CDiagram				*m_d;
	CDiagramWidget			*m_parent;
	QList<CDiagramTool*>	m_tools;

}; // End of class CDiagramHostedWidget

class CORE_EXPORT CDiagramWidget : public QGraphicsObject 
{
public:
	CDiagramWidget(CDiagram *d = NULL);
	virtual ~CDiagramWidget();

	// Enumeration of anchor types
	enum AnchorType {
		AnchorToSelectedItem = 0x01,
		AnchorToSceneLeft,
        AnchorToSceneRight,
		FreeInScene
	}; // End of AnchorType

	enum WidgetType {
		CustomType = 0x10000
	}; // End of enumeration
	
	// Used to identify the widget type
	virtual int widgetType();

	// initialization/uninitialization
	virtual void init(CDiagram *d);
	virtual void uninit(CDiagram *d);
	
	// anchor type setting
	void setAnchorType(CDiagramWidget::AnchorType type);
	CDiagramWidget::AnchorType anchorType();

	//
	void addHostedWidget(CDiagramHostedWidget *widget);
	CDiagramHostedWidget* activeHostedWidget();

	void updateVisibility(bool value);
	void setAutoHide(bool value);
	bool autoHide();

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0); 

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );

	// custom methods
	virtual void updatePosViaGeometryChange(const QRectF &rect); 
	virtual void setHostedWidget(CDiagramHostedWidget *widget);
	virtual CDiagramHostedWidget* getPreferredHostedWidget();

	CDiagram						*m_d;
	QGraphicsProxyWidget			*m_proxy;
	CDiagramHostedWidget			*m_activeHostedWidget;
	QList<CDiagramHostedWidget*>	m_hostedWidgets;
	QPointF							m_prev;
	QPointF							m_viewPos;
	bool							m_moving;
	bool							m_autoHide;
	bool							m_hovered;
	qreal							m_width;
	qreal							m_height;
	QSizeF							m_boxSize;
	CDiagramWidget::AnchorType		m_anchorType;

}; // End of class CDiagramWidget


class CORE_EXPORT CDiagramToolBar : public CDiagramWidget
{
	Q_OBJECT

public:
	CDiagramToolBar(CDiagram *d = NULL);
	virtual ~CDiagramToolBar();

	enum { Type = UserType + Global::DiagramToolBar };
	virtual int type() const { return Type; }

	virtual void init(CDiagram *d);
	virtual void uninit(CDiagram *d); 

public Q_SLOTS:
	virtual void onSelectionChanged(QList<CDiagramItem*>&, const QRectF &);
	virtual void onSelectedItemsMoving(qreal, qreal);
	virtual void onSelectedItemsDragging();
	virtual void onSelectedItemsDraggingStop(const QRectF &rect);
	virtual void onSelectedItemsGeometryChanged(const QRectF &rect);
	virtual void onDiagramModeChanged(CDiagram::DiagramMode);
	virtual void onDiagramVisibleRectChanged(const QRectF &);
}; // End of class CDiagramToolBar

#endif // PRODRAW_CDIAGRAMWIDGET_H
