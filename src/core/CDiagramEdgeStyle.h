/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMEDGESTYLE_H
#define PRODRAW_CDIAGRAMEDGESTYLE_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramArrow;
class CDiagramEdgeControlPoint;

/**
 * CDiagramEdgeStyle is the base class for handling the path
 * strategy.
 */
class CORE_EXPORT CDiagramEdgeStyle
{
public:
	CDiagramEdgeStyle(const QString &ctg, const QString &name, const QIcon &icon);

	virtual void route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const;
	virtual void draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const;
	
	void setCategory(const QString &value) { m_category = value; }
	QString getCategory() { return m_category; }
	
	void setName(const QString &value) { m_name = value; }
	QString getName() { return m_name; }

	void setIcon(const QIcon &value) { m_icon = value; }
	QIcon getIcon() { return m_icon; }

protected:
	QString			m_category;
	QString			m_name;
	QIcon			m_icon;
	qreal			m_ctrlLength;

}; // End of class CDiagramEdgeStyle

#endif // PRODRAW_CDIAGRAMEDGESTYLE_H
