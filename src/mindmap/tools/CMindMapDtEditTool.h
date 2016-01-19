/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTEDITTOOL_H
#define PRODRAW_CMINDMAPDTEDITTOOL_H

#include <QtGui>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtEditTool;
}

class CDiagram;

class CMindMapDtEditTool : public CDiagramTool
{
	Q_OBJECT

public:
    CMindMapDtEditTool(QWidget *parent = 0);
    virtual ~CMindMapDtEditTool();

	bool support(const QList<CDiagramItem*> &items);
	void init(CDiagram *d);
	void uninit(CDiagram *d);
	void reset();

public Q_SLOTS:
	virtual void onClipboardChanged(QClipboard::Mode mode);
	virtual void onCanRedoChanged(bool value);
	virtual void onCanUndoChanged(bool value);
	virtual void onCopy();
    virtual void onClone();
	virtual void onCut();
	virtual void onDelete();
	virtual void onPaste();
	virtual void onRedo();
	virtual void onUndo();

private:
    Ui::CMindMapDtEditTool *ui;

}; // End of class CMindMapDtEditTool

#endif // PRODRAW_CMINDMAPDTEDITTOOL_H
