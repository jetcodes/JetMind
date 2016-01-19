/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTLINETOOL_H
#define PRODRAW_CMINDMAPDTLINETOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtLineTool;
}

class CUiPenStylePicker;
class CUiPenWeightPicker;
class CUiColorPanel;
class CMindMapUiPathList;
class CMindMapDtLineTool;

class CMindMapDtLineTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtLineTool(QWidget *parent = 0);
	virtual ~CMindMapDtLineTool();

	bool support(const QList<CDiagramItem*> &items);
	void reset();

public Q_SLOTS:
	void onPathSelected(int);
	void onLineColor();
	void onLineColorSelected(const QColor &c);
	void onLineStyleSelected(Qt::PenStyle style);
	void onLineWeightSelected(qreal w);

private:
	Ui::CMindMapDtLineTool *ui;

	CUiPenStylePicker			*m_lineStyles;
	CUiPenWeightPicker			*m_lineWeights;
	CUiColorPanel				*m_lineColors;
	CMindMapUiPathList			*m_pathList;

}; // End of class  CMindMapDtLineTool

#endif // PRODRAW_CMINDMAPDTLINETOOL_H
