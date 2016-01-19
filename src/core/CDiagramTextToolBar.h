/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMTEXTTOOLBAR_H
#define PRODRAW_CDIAGRAMTEXTTOOLBAR_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramWidget>

class CDiagram;
class CDiagramText;

class CDiagramTextToolBar : public CDiagramWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramTextToolBar();
	virtual ~CDiagramTextToolBar();

	enum { WidgetType = CustomType + 1};
	virtual int widgetType() { return WidgetType; }

	void init(CDiagram *d);
	void uninit(CDiagram *d);

public Q_SLOTS:
	void onTextEditRequest(CDiagramText *text);
	void onTextSizeChanged();
	void onDiagramModeChanged(CDiagram::DiagramMode mode);

protected:
	void updatePosition();

private:
	CDiagramText			*m_text;

}; // End of class PRODRAW_CDIAGRAMTEXTTOOLBAR_H

#endif // PRODRAW_CCANVASTEXTEDITOR_H
