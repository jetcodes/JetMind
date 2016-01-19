/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTNEWTOOL_H
#define PRODRAW_CMINDMAPDTNEWTOOL_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

namespace Ui {class CMindMapDtNewTool;}

class CMindMapDtNewTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtNewTool(QWidget *parent = 0);
	virtual ~CMindMapDtNewTool();

	bool support(const QList<CDiagramItem*> &items);
	void reset();

public Q_SLOTS:
	void onInsertBefore();
	void onInsertAfter();
	void onInsertParent();
	void onInsertChild();

private:
	Ui::CMindMapDtNewTool *ui;

}; // End of class CMindMapDtNewTool

#endif // PRODRAW_CMINDMAPDTNEWTOOL_H
