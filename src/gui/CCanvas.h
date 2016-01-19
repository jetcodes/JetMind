/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CCANVAS_H
#define PRODRAW_CCANVAS_H

#include <QtGui>
#include <core/CDiagram>
#include "ui_CCanvas.h"

class CUiRuler;
class CCanvas;
class CCanvasBand;
class CDiagramItem;
class CDiagramNodeStyle;
class CDiagramEditor;
class CDiagramText;
class CWorkbench;

class CCanvas : public QWidget
{
	Q_OBJECT

public:
	CCanvas(QWidget *parent = 0);
	virtual ~CCanvas();

	void paintEvent(QPaintEvent *event);

	void viewDiagram(CDiagram *d, const QPointF &center = QPointF());
	void closeDiagram();

	CDiagramEditor* editor() { return m_editor; }
	CDiagram* diagram() { return m_d; }

	void setGridsVisible(bool value);
	bool gridsVisible() const;

	void setRulersVisible(bool value);
	bool rulersVisible() const;

	void setAutoAlignment(bool value);
	bool autoAlignment() const;

	void ensureDiagramVisible();

	void setFullScreenMode(bool value);

Q_SIGNALS:
	// Emitted when the user select to hide/show grids of the canvas
	// from the canvas, the status MUST be synchronized with the workbench.
	void gridsVisibleChanged(bool);

	// The similar logic with grids
	void rulersVisibleChanged(bool);

	// auto-alignment changed
	void autoAlignmentChanged(bool);

public Q_SLOTS:
	void onZoom(int);
	void onScaleChanged(qreal);

protected Q_SLOTS:
	void onGridsVisibleChanged(bool);
    void onRulersVisibleChanged(bool);
    void onAutoAlignmentChanged(bool);
	void onShowCanvasSetupPanel();
    void onShowCanvasQuickView();
	void onEnsureDiagramVisible();
	void onMoveViewCenter(const QPointF &);
    void onCanvasStyleSelected(QBrush);

protected:
	Ui::CCanvas				*m_ui;

	QWidget					*m_holder;
	CCanvasBand				*m_band;
	CUiRuler				*m_hRuler;
    CUiRuler				*m_vRuler;

	CDiagram				*m_d;
    CDiagramEditor			*m_editor;
    
	CWorkbench				*m_workbench;

	bool					m_showGrids;
	bool					m_showRulers;
	bool					m_autoExpand;
	bool					m_autoAlignment;

	friend class CWorkbench;
	friend class CCanvasBand;
};

#endif // PRODRAW_CCANVAS_H
