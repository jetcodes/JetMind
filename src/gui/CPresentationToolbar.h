/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CPRESENTATIONTOOLBAR_H
#define PRODRAW_CPRESENTATIONTOOLBAR_H

#include <QtGui>
#include <core/CDiagramLayer>
#include <core/CDiagramWidget>

namespace Ui {class CPresentationToolbar;};

class CPresentationToolbar : public CDiagramHostedWidget
{
	Q_OBJECT

public:
	CPresentationToolbar(QWidget *parent = 0);
	virtual ~CPresentationToolbar();

	bool support(CDiagram *d);

public Q_SLOTS:
	void onDraw();
	void onComment();
	void onClear();
	void onPointer();
	void onHand();
	void onExit();
	void onNext();
	void onPrev();

private:
	void setLayerEditMode(CDiagramTagLayer::EditMode mode);
	void setViewMode(QGraphicsView::DragMode mode);

	Ui::CPresentationToolbar *ui;

}; // End of class CPresentationToolbar

#endif // PRODRAW_CPRESENTATIONTOOLBAR_H
