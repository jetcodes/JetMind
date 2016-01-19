/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUINODESTYLEPANEL_H
#define PRODRAW_CUINODESTYLEPANEL_H

#include <QtGui>
#include <core/CDiagramNode>
#include <widgets/CUiGlobal>

namespace Ui {class CUiNodeStylePanel;};

class CDiagram;
class CDiagramItem;
class CUiNodeStylePicker;

class WIDGETS_EXPORT CUiNodeStylePanel : public QMenu
{
	Q_OBJECT

public:
	enum StyleCategory {
		Classic,
		Flat
	}; // enumeration

	CUiNodeStylePanel(QWidget *parent = 0);
	virtual ~CUiNodeStylePanel();
	
	void setColumns(int num);
	int columns();

	void addRecentStyle(const CDiagramNodeStyle &);
	void setRecentStyles(const QList<CDiagramNodeStyle> &);
	QList<CDiagramNodeStyle> recentStyles();
	const CDiagramNodeStyle defaultStyle() const;

	void setPenWidth(qreal value);
	qreal penWidth();

Q_SIGNALS:
	void styleReset();
	void styleHovered(const CDiagramNodeStyle &s);
	void styleSelected(const CDiagramNodeStyle &s);

protected Q_SLOTS:
	void onStyleHovered(const CDiagramNodeStyle &s);
	void onStyleSelected(const CDiagramNodeStyle &s);
	void onRecentStyleSelected(const CDiagramNodeStyle &s);
	void onStyleCategoryChanged(int value);

protected:
	virtual void hideEvent( QHideEvent *event );
	virtual void closeEvent( QCloseEvent *event );
	virtual void paintEvent( QPaintEvent *event );
	virtual void showEvent( QShowEvent * event );

private:
	void setupSystemStyles();

	Ui::CUiNodeStylePanel *ui;

	bool						m_styleReset;
	int							m_cols;

	CUiNodeStylePicker						*m_systemGrid;
	CUiNodeStylePicker						*m_recentGrid;
	QList<CDiagramNodeStyle>				m_recentStyles;
	CDiagramNodeStyle						m_defaultStyle;
	QMap<int, QList<CDiagramNodeStyle> >	m_systemStyles;

}; // End of class CUiNodeStylePanel

#endif // PRODRAW_CUINODESTYLEPANEL_H
