/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTLAYOUTTOOL_H
#define PRODRAW_CMINDMAPDTLAYOUTTOOL_H

#include <QtGui>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtLayoutTool;
}

class CDiagram;
class CMindMapUiLayoutList;

class CMindMapDtLayoutTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtLayoutTool(QWidget *parent = 0);
	virtual ~CMindMapDtLayoutTool();

    bool support(const QList<CDiagramItem *> &items);
	void reset();

public Q_SLOTS:
	void onLayoutSelected(int);

private:
	Ui::CMindMapDtLayoutTool *ui;

	CMindMapUiLayoutList		*m_layoutList;

}; // End of class CMindMapDtLayoutTool

#endif // PRODRAW_CMINDMAPDTLAYOUTTOOL_H
