/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPDTTEXTTOOL_H
#define PRODRAW_CMINDMAPDTTEXTTOOL_H

#include <QtGui>
#include <core/CDiagramWidget>

namespace Ui {
    class CMindMapDtTextTool;
}

class CUiColorPanel;
class CMindMapDtTextTool;

class CMindMapDtTextTool : public CDiagramTool
{
	Q_OBJECT

public:
	CMindMapDtTextTool(QWidget *parent = 0);
	virtual ~CMindMapDtTextTool();

    bool support(const QList<CDiagramItem *> &items);
	void reset();

public Q_SLOTS:
	void onListStyle();
	void onFontBold();
	void onFontItalic();
	void onFontUnderline();
	void onTextColor();
	void onTextColorSelected(QColor);
	void onTextAlignLeft();
	void onTextAlignRight();
	void onTextAlignCenter();
	void onFontFamilySelected(int);
	void onFontSizeSelected(const QString&);

private:
	Ui::CMindMapDtTextTool *ui;

	CUiColorPanel				*m_textColors;

}; // End of class CMindMapDtTextTool

#endif // PRODRAW_CMINDMAPDTTEXTTOOL_H
