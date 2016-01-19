/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTSHAPETOOL_H
#define PRODRAW_CMINDMAPDTSHAPETOOL_H

#include <QtGui>
#include <core/CDiagramNode>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtShapeTool;
}

class CDiagram;
class CDiagramItem;
class CUiColorPanel;
class CMindMapUiShapeList;

class CMindMapDtShapeTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtShapeTool(QWidget *parent = 0);
	virtual ~CMindMapDtShapeTool();

    bool support(const QList<CDiagramItem *> &items);
	void reset();

public Q_SLOTS:
	void onShapeSelected(int);
	void onFillColor();
	void onFillColorSelected(const QColor &c);
	void onFillGradientSelected(const QGradient &g);

private:
	Ui::CMindMapDtShapeTool *ui;

	CMindMapUiShapeList				*m_shapeList;
	CUiColorPanel					*m_fillColors;

}; // End of class CMindMapDtShapeTool

#endif // PRODRAW_CMINDMAPDTSHAPETOOL_H
