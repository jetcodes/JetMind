/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CCANVASBAND_H
#define PRODRAW_CCANVASBAND_H

#include <QtGui>
#include "ui_CCanvasBand.h"

class CCanvas;
class CDiagram;
class CDiagramFile;
class CUiPercentPicker;
class CDiagramItem;
class CMindMapGraph;
class CMindMapFile;
class CCanvasSettingPanel;
class CCanvasQuickView;

class CCanvasBand : public QWidget
{
	Q_OBJECT

public:
	CCanvasBand(QWidget *parent = 0);
	virtual ~CCanvasBand();

	void paintEvent(QPaintEvent *event);
	void setZoomValue(int value);
	void update(CDiagram *d);

    bool eventFilter(QObject *obj, QEvent *evt);

Q_SIGNALS:
    void zoom(int value);
    void gridsVisibleChanged(bool);
	void ensureDiagramVisible();
    void canvasStyleSelected(QBrush);

public Q_SLOTS:
	void onZoom(int);
	void onZoomOut();
	void onZoomIn();
	void onPercentage();
	void onPercentageChanged(int);
    void onShowCanvasQuickView();
    void onShowCanvasStylePanel();
    void onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect);
	void onAutoSceneExpand(bool);
	void onBenchResized(const QRect &);
    void onBenchFullscreen(bool);
    void onAddGraph(CDiagram *graph);
    void onChangeFile(CDiagramFile *file);
    void onGraphTabSelected(int index);
    void onNewGraphClicked();
    void onRemoveGraphClicked();
	void onRemoveGraph(CDiagram *graph);
	void onViewGraph(CDiagram *graph);
    void onHideGraphClicked();
    void onListHiddenGraphes();
    void onShowAllHiddenGraphes();
    void onTabMoved(int from, int to);

    void onTabMenu(QPoint);
    void onTitleEditDone();
    void onTitleEditCancel();

protected:
    Ui::CCanvasBand             *m_ui;

    QMenu                       *m_tabMenu;
    QLineEdit                   *m_tabEditor;
    QTabBar                     *m_sheetBar;
    CCanvas                     *m_canvas;
    CMindMapFile                *m_file;
    CCanvasQuickView         *m_quickView;
    CCanvasSettingPanel      *m_stylePanel;
    CUiPercentPicker            *m_percentageList;

    CDiagramItem                *m_item;

	friend class CCanvas;
};

#endif // PRODRAW_CCANVASBAND_H
