/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIITEMPICKER_H
#define PRODRAW_CUIITEMPICKER_H

#include <QtGui>
#include <core/CDiagramLibraryModel>
#include <widgets/CUiGlobal>

class CUiItemPicker;
class CCategoryQuickView;

class WIDGETS_EXPORT CUiItemPicker: public QMenu
{
	Q_OBJECT

public:
	CUiItemPicker(QWidget *parent);
	virtual ~CUiItemPicker();
	
	void setupMenus();
	CDiagramLibraryItem* defaultShape();

Q_SIGNALS:
	void shapeSelected(CDiagramLibraryItem *item);

public Q_SLOTS:
	void onShapeSelected(CDiagramLibraryItem *item);

private:
	Q_DISABLE_COPY(CUiItemPicker);

	QMap<QString, CCategoryQuickView*>	m_views;
	QString								m_defaultCategory;

}; // End of class CUiItemPicker

class WIDGETS_EXPORT  CCategoryQuickView : public QMenu
{
	Q_OBJECT

public:
	CCategoryQuickView(QWidget *parent);
	virtual ~CCategoryQuickView();

	void setLibraryCategoryModel(CDiagramLibraryCategoryModel *model);
	CDiagramLibraryItem* defaultShape();

	QSize sizeHint() const;

Q_SIGNALS:
	void shapeSelected(CDiagramLibraryItem *item);

protected:
	void setupLibrary();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

private:
	qreal				m_gridSize;
	qreal				m_space;
	int					m_columns;
	int					m_rows;
	int					m_currShape;
	QPointF				m_mousePos;
	QString				m_currCategory;

	QMap<QString, CDiagramLibraryItem*>		m_defaultShapes;
	QList<CDiagramLibraryItem*>				m_shapes;
	QList<CDiagramLibraryItem*>				m_cores;

}; // End of class CUiItemPicker

#endif // PRODRAW_CUIITEMPICKER_H
