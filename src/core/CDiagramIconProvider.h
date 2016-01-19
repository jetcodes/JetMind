/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMICONPROVIDER_H
#define PRODRAW_CDIAGRAMICONPROVIDER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramArrow;
class CDiagramEdgeStyle;

class CORE_EXPORT CDiagramIconProvider
{
public:
	//
	QIcon getIconForPen(const QPen &pen);
	QIcon getIconForBrush(const QBrush &brush);
	QIcon getIconForFont(const QFont &font);

	//
	QIcon getIconForStartArrow(const CDiagramArrow *arrow);
	QIcon getIconForEndArrow(const CDiagramArrow *arrow);
	QIcon getIconForEdgeStyle(const CDiagramEdgeStyle *style);

	// Item related icons
	QIcon genIconForItem(QMetaObject *meta, const QSizeF &size = QSizeF(64,64));
	QIcon genIconForItem(QGraphicsItem *o, const QSizeF &size = QSizeF(64,64));
	
	// Path related icons
	QIcon genIconForPath(QPainterPath path, const QPen &pen = QPen( QColor("#666666"), 1), const QBrush &brush = QBrush( QColor("#ffffff") ), const QSize &size =  QSize(16, 16) );

	//
	QIcon genIconForImageFile(const QString &file, const QSize &size = QSize(64,64));
    QImage genImageForSvgFile(const QString &file, QSize &size);

	static CDiagramIconProvider* instance()
	{
		if (m_inst == NULL)
			m_inst = new CDiagramIconProvider();
		return m_inst;
	}

private:
	CDiagramIconProvider();
	Q_DISABLE_COPY(CDiagramIconProvider)

	bool							m_batchMode;
	QSizeF							m_size;
	QPen							m_pen;
	QBrush							m_brush;

	static CDiagramIconProvider		*m_inst;
}; // End of class CDiagramIconProvider

#endif // PRODRAW_CDIAGRAMICONPROVIDER_H
