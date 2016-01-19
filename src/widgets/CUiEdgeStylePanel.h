/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIEDGESTYLEPANEL_H
#define PRODRAW_CUIEDGESTYLEPANEL_H

#include <QtGui>
#include <widgets/CUiGlobal>

namespace Ui {class CUiEdgeStylePanel;};

class CDiagram;
class CDiagramItem;
class CUiColorGrid;
class CUiPenStylePicker;
class CUiPenWeightPicker;

class CUiEdgeStylePanel : public QWidget
{
	Q_OBJECT

public:
	CUiEdgeStylePanel(QWidget *parent = 0);
	virtual ~CUiEdgeStylePanel();

Q_SIGNALS:
	void colorSelected(QColor);
	void styleSelected(Qt::PenStyle);
	void weightSelected(qreal);

public Q_SLOTS:
	void onColorSelectionDone();
	void onColorSelected(QColor);
	void onPenStyleSelected(Qt::PenStyle);
	void onPenWeightSelected(qreal);

protected:
	//virtual void closeEvent( QCloseEvent *event );
	virtual void paintEvent( QPaintEvent *event );

private:
	Ui::CUiEdgeStylePanel *ui;

	CUiColorGrid				*m_colorGrid;
	CUiPenStylePicker			*m_penStyles;
	CUiPenWeightPicker			*m_penWidths;

}; // End of class CUiEdgeStylePanel

#endif // PRODRAW_CUIEDGESTYLEPANEL_H
