/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <QSvgRenderer>

#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramArrow>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramIconProvider>

CDiagramIconProvider* CDiagramIconProvider::m_inst = NULL;

CDiagramIconProvider::CDiagramIconProvider()
	: m_batchMode(false)
{

}

QIcon CDiagramIconProvider::getIconForPen(const QPen &pen)
{
	qreal		w = 16, h = 16;
	QPainter	painter;
	QPixmap		image(w, h);
	
	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setPen( pen );
	painter.drawLine(0, h / 2.0, w, h / 2.0);
	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::getIconForBrush(const QBrush &brush)
{
	qreal		w = 16, h = 16;
	QPainter	painter;
	QPixmap		image(w, h);
	
	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setBrush( brush );
	painter.drawRect(-1, -1, w+2, h+2);
	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::getIconForFont(const QFont &font)
{
	qreal		w = 16, h = 16;
	QPainter	painter;
	QPixmap		image(w, h);
	
	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen( QPen(Qt::black, 2) );
	painter.drawText( QRectF(0, 0, w, h), Qt::AlignCenter, QString::fromUtf8("A") );
	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::getIconForStartArrow(const CDiagramArrow *arrow)
{
	QPointF				p1(16, 0), p2(-16, 0), p3;
	QPainterPath		path;
	QPainterPathStroker	stroker;
	qreal				w = 32, h = 32;
	qreal				padding = 2;
	QIcon				icon;
	QPixmap				image(w, h);
	QPainter			painter;

	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( w / 2.0, h / 2.0);
	painter.setPen( QPen(Qt::black, 1) );

	if (arrow)
	{
		p3 = arrow->draw(&painter, p1, p2, path);
		painter.drawLine(p1, p3);
	}
	else
	{
		painter.drawLine(p1, p2);
	}
	//path = stroker.createStroke(path);

	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::getIconForEndArrow(const CDiagramArrow *arrow)
{
	QPointF				p1(-16, 0), p2(16, 0), p3;
	QPainterPath		path;
	QPainterPathStroker	stroker;
	qreal				w = 32, h = 32;
	qreal				padding = 2;
	QIcon				icon;
	QPixmap				image(w, h);
	QPainter			painter;

	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( w / 2.0, h / 2.0);
	painter.setPen( QPen(Qt::black, 1) );

	if (arrow)
	{
		p3 = arrow->draw(&painter, p1, p2, path);
		painter.drawLine(p1, p3);
	}
	else
	{
		painter.drawLine(p1, p2);
	}
	//path = stroker.createStroke(path);

	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::getIconForEdgeStyle(const CDiagramEdgeStyle *style)
{
	int					s1 = Global::East, s2 = Global::North;
	QPointF				p1(-15, -15), p2(15, 15);
	QList<QPointF>		ps;
	QPainterPath		path;
	QPainterPathStroker stroker;
	qreal				w = 32, h = 32;
	qreal				padding = 2;
	QIcon				icon;
	QPixmap				image(w, h);
	QPainter			painter;

	style->route(p1, s1, p2, s2, ps);
	style->draw(NULL, ps, NULL, NULL, path);
	//path = stroker.createStroke(path);

	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( w / 2.0 + 0.5, h / 2.0 + 0.5);
	painter.setPen( QPen(Qt::black, 1) );
	painter.drawPath(path);
	painter.end();

	return QIcon(image);
}

QIcon CDiagramIconProvider::genIconForItem(QMetaObject *meta, const QSizeF &size)
{
	QIcon			icon;
	CDiagramItem	*item = NULL;
	qreal			dw, dh, padding = 4;
	qreal			dx, dy, ratio;
	qreal			itemWidth, itemHeight;
	qreal			imageWidth = size.width();
	qreal			imageHeight = size.height();

	item = dynamic_cast<CDiagramItem*>(meta->newInstance(Q_ARG(CDiagram*, NULL)));
    if (!item)
        return icon;
	
	item->setBrush( Qt::white );
	item->setPen( QPen( Qt::black, 1 ) );
	dw = dh = 2 * padding;
	ratio = item->sizeRatio();
	itemHeight = (imageWidth - dw) / ratio;
	itemWidth = (imageHeight - dh) * ratio;
	
	if (itemHeight >= (imageHeight - dh))
	{
		itemHeight = imageHeight - dh;
		itemWidth = itemHeight * ratio;
		dy = padding;
		dx = (imageWidth - itemWidth) / 2.0;
	}
	else
	{
		itemWidth = imageWidth - dw;
		itemHeight = itemWidth / ratio;
		dx = padding;
		dy = (imageHeight - itemHeight) / 2.0;
	}
	item->defaultInit();
	item->setGeometry( QRectF(0, 0, itemWidth, itemHeight) );
	return genIconForItem(item, size);
}

QIcon CDiagramIconProvider::genIconForItem(QGraphicsItem *o, const QSizeF &size)
{
	qreal		w = size.width(), h = size.height();
	qreal		rw, rh, ratio;
	QRectF		r;
	QIcon		icon;
	QPainter	painter;

	if (!o) return icon;

	r = o->boundingRect();
	rw = qMax(r.width(), w);
	rh = qMax(r.height(), h);

	ratio = rw / rh;
	if (ratio > 1.0)
		ratio = w / rw;
	else
		ratio = h / rh;

	QPixmap	image(w, h);

	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.translate( w / 2.0, h / 2.0);
	painter.scale(ratio, ratio);
	o->paint(&painter, NULL, NULL);
	painter.end();

	icon = QIcon( image );

	return icon;
}

QIcon CDiagramIconProvider::genIconForPath(QPainterPath path, const QPen &pen, const QBrush &brush, const QSize &size)
{
	qreal				iconW = size.width(), iconH = size.height();
	qreal				padding = 2;
	QIcon				icon;
	QPixmap				image(iconW, iconH);
	QPainter			painter;
	
	image.fill(Qt::transparent);
	painter.begin(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( iconW / 2.0, iconH / 2.0);
	painter.setPen( pen );
	painter.setBrush( brush );
	painter.drawPath(path);
	painter.end();

	icon = QIcon(image);
	return icon;
}

QIcon CDiagramIconProvider::genIconForImageFile(const QString &file, const QSize &size)
{
	QIcon		r;
	QPixmap		pixmap(file);

	r = QIcon(pixmap.scaled(size, Qt::KeepAspectRatioByExpanding));
	return r;
}

QImage CDiagramIconProvider::genImageForSvgFile(const QString &file, QSize &size)
{
	int				w, h, w1, h1;
	int				width, height;
	QImage			image;
	QPainter		painter;
	QSvgRenderer	svgRender( file );

	w1 = size.width();
	h1 = size.height();
	image = QImage(w1, h1, QImage::Format_ARGB32_Premultiplied);
	painter.begin(&image);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRect(QRect(0, 0, w1, h1));

	// default size assigned.
	size = svgRender.defaultSize();
	width = size.width();
	height = size.height();
	if (width > height)
	{
		w = w1;
		h = int(height * w / width);
	}
	else
	{
		h = h1;
		w = int(width * h / height);
	}
	svgRender.render(&painter, QRectF( (w1 - w) / 2.0, (h1 - h ) / 2.0, w, h) );
	painter.end();
	
	return image;
}
