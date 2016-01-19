/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTSTYLETOOL_H
#define PRODRAW_CMINDMAPDTSTYLETOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramNode>

namespace Ui {class CMindMapDtStyleTool;};

class CDiagram;
class CDiagramItem;
class CUiNodeStylePanel;

class CMindMapDtStyleTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtStyleTool(QWidget *parent = 0);
	virtual ~CMindMapDtStyleTool();

	void reset();

public Q_SLOTS:
	void onFillStyle();
	void onFillStyleReset();
	void onFillStyleHovered(const CDiagramNodeStyle &style);
	void onFillStyleSelected(const CDiagramNodeStyle &style);

private:
	Ui::CMindMapDtStyleTool *ui;
	
	CUiNodeStylePanel							*m_styleGrid;
	QMap<CDiagramItem*, CDiagramNodeStyle>		m_oldStyles;

}; // End of class CMindMapDtStyleTool

#endif // PRODRAW_CMINDMAPDTSTYLETOOL_H
