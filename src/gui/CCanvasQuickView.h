/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CCANVASQUICKVIEW_H
#define PRODRAW_CCANVASQUICKVIEW_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <widgets/CUiAbstractDialog>

namespace Ui {class CCanvasQuickView;}

class CDiagram;
class CDiagramView;

class CCanvasQuickView : public CUiAbstractDialog
{
	Q_OBJECT

public:
    CCanvasQuickView(QWidget *parent = 0);
    virtual ~CCanvasQuickView();

	void update(CDiagram *d);

Q_SIGNALS:
	void ensureDiagramVisible();
	
protected:
    void changeEvent(QEvent *event);

private:
    Ui::CCanvasQuickView *ui;

	CDiagram		*m_d;
	CDiagramView	*m_view;

}; // End of class CCanvasQuickView

#endif // PRODRAW_CCANVASQUICKVIEW_H
