/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUINODESTYLEPICKER_H
#define PRODRAW_CUINODESTYLEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>

class CDiagramNodeStyle;

class WIDGETS_EXPORT CUiNodeStylePicker : public QFrame
{
	Q_OBJECT

public:
	CUiNodeStylePicker(QWidget *parent);
	virtual ~CUiNodeStylePicker();

	void setSelfClose(bool vlaue);
	bool isSelfClose();

	void setRows(int num);
	int rows();
	void setColumns(int num);
	int columns();

	void setPenWidth(qreal value);
	qreal penWidth();

	void addStyle(const CDiagramNodeStyle &style);
	void setStyles(const QList<CDiagramNodeStyle> &styles);
	QList<CDiagramNodeStyle> styles();

	virtual QSize minimumSizeHint() const;

Q_SIGNALS:
	void styleHovered(const CDiagramNodeStyle &s);
	void styleSelected(const CDiagramNodeStyle &s);

protected:
	void setupStyles();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mouseDoubleClickEvent( QMouseEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

private:
	bool						m_selfClose;
	qreal						m_gridSize;
	qreal						m_space;
	qreal						m_penWidth;
	int							m_columns;
	int							m_rows;
	int							m_currStyle;
	QList<CDiagramNodeStyle>	m_styles;
	QPointF						m_mousePos;
};

#endif // PRODRAW_CUINODESTYLEPICKER_H
