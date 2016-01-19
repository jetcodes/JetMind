/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_DIAGRAMEDITOR_H
#define PRODRAW_DIAGRAMEDITOR_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramView;
class CDiagramEditor;
class CDiagramContext;
class CDiagramObserver;

class CORE_EXPORT CDiagramView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit CDiagramView(QWidget *parent = 0);
	virtual ~CDiagramView();

	void setDiagram(CDiagram *d);
	CDiagram* diagram();

	void setViewRect(const QRectF &value);
	QRectF viewRect();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void drawForeground(QPainter *painter, const QRectF &rect);
	void resizeEvent(QResizeEvent *event);
	void updateScale();

	CDiagram			*m_d;
	QRectF				m_sc;
	bool				m_pressed;
	qreal				m_scale;
	QPointF				m_viewCenterPos;
	QPointF				m_buttonDownPos;

    friend class CDiagramEditor;

}; // End of class CDiagramView

/**
 * CDiagramEditor is actually a widget supports the rendering
 * of a diagram on its surface. It inherits QGraphicsView.
 */
class CORE_EXPORT CDiagramEditor : public QGraphicsView
{
	Q_OBJECT

public:
	explicit CDiagramEditor(QWidget* parent = 0);
	virtual ~CDiagramEditor();

	// Overrided member functions from the super class
	void setDiagram(CDiagram *d, const QPointF &center = QPointF());
	CDiagram* diagram();

	void setDiagramContext(CDiagramContext *cxt);
	CDiagramContext* diagramContext();

	// diagram view
	CDiagramView* diagramView();

	// scale the diagram
	void scaleView(qreal scaleFactor);

	// get the perfect position for rect
	QPointF getAnchorPoint(QRectF rect);
	QPoint getAnchorPoint(QRect rect);
	
Q_SIGNALS:
	void diagramChanged(CDiagram *prev, CDiagram *curr);

public Q_SLOTS:
	void unitedSceneRect(const QRectF& rect);
	void onScrollBarChanged(int value);

protected:
	bool event(QEvent *evt);
	void mousePressEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void mouseReleaseEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
	void wheelEvent( QWheelEvent * event );
	void resizeEvent( QResizeEvent * event );
	void scrollContentsBy(int dx, int dy);

private:
	CDiagram							*m_d;
	CDiagramContext						*m_context;
	CDiagramView						*m_miniView;
	bool								m_rubberBandSelection;
	bool								m_rubberBandSelectionStarted;
	QPointF								m_rubberBandStartPosition;

}; // end of class CDiagramEditor

#endif // PRODRAW_DIAGRAMWIDGET_H
