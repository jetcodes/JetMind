/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMARROW_H
#define PRODRAW_CDIAGRAMARROW_H

#include <QtGui>
#include <core/CGlobal>

class CORE_EXPORT CDiagramArrow
{
public:

	// Describe the direction of the arrow
	enum ArrowDirection {
		Inward = 0x0,
		Outward,
		BothDirections
	}; // End of enum

	virtual QPointF draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const { return QPointF(); };

	const QString& getCategory() { return m_category; }
	void setCategory(const QString &value) { m_category = value; }
	
	const QString& getName() { return m_name; }
	void setName(const QString &value) { m_name = value; }

	const QString& getDesc() { return m_desc; }
	void setDesc(const QString &value) { m_desc = value; }

	QIcon getIcon() { return m_icon; }
	void setIcon(const QIcon &value) { m_icon = value; }
	
	void setDirection(int value) { m_direction = value; }
	int direction() { return m_direction; }

protected:
	QString			m_category;
	QString			m_name;
	QString			m_desc;
	QIcon			m_icon;
	qreal			m_length;
	qreal			m_width;
	int				m_direction;

}; // end of class CDiagramArrow

#endif // PRODRAW_CDIAGRAMARROW_H
