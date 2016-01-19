/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPQUICKTOOL_H
#define PRODRAW_CMINDMAPQUICKTOOL_H

#include <QtGui>
#include <core/CDiagramNode>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtQuickTool;
}

class CDiagram;
class CDiagramItem;
class CMindMapView;
class CMindMapNode;
class CUiNodeStylePanel;
class CMindMapUiShapeList;
class CMindMapUiIconList;
class CMindMapUiPathList;
class CMindMapUiLinkEditor;
class CMindMapUiNoteEditor;
class CMindMapUiLayoutList;
class CMindMapUiNewItemList;

class CMindMapDtQuickTool : public CDiagramTool
{
	Q_OBJECT

public:
    CMindMapDtQuickTool(QWidget *parent = 0);
    virtual ~CMindMapDtQuickTool();

	bool support(const QList<CDiagramItem*> &items);
	void reset();

Q_SIGNALS:
    void insertTopicBefore();
    void insertTopicAfter();
    void insertTopicParent();
    void insertTopicChild();
    void addBoundary();
    void fillStyleReset();
    void fillStyleHovered(const CDiagramNodeStyle &);
    void fillStyleSelected(const CDiagramNodeStyle &);
    void shapeSelected(int);
    void pathSelected(int);
    void layoutSelected(int);
    void browseIconLibrary();
    void iconSelected(QIcon, int);
    void showLinkEditor();
    void showNoteEditor();
    void showAttachFileDialog();
    void showTaskEditor();
    void browsePixmap();

public Q_SLOTS:
	void onNewNode();
    void onFillStyle();
    void onInsertPositionSelected(int, QIcon);

protected Q_SLOTS:
    void onShowIconMenu();
    void onShowFillStyleMenu();

private:
    Ui::CMindMapDtQuickTool *ui;
    Q_DISABLE_COPY(CMindMapDtQuickTool)

protected:
	CUiNodeStylePanel							*m_styleGrid;
	CMindMapUiShapeList							*m_shapeList;
	CMindMapUiIconList							*m_iconList;
	CMindMapUiLayoutList						*m_layoutList;
	CMindMapUiPathList							*m_pathList;
    CMindMapUiNewItemList						*m_newItemList;
	int											m_defaultNewItem;

}; // End of class CMindMapUiQuickTool

#endif // PRODRAW_CMINDMAPQUICKTOOL_H
