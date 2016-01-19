/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagramNode>
#include <core/CDiagramPixmapNode>

CDiagramPixmapNode::CDiagramPixmapNode(CDiagram *parent)
	: CDiagramNode(parent)
{
	m_rotatable = true;

	m_labelPosition = CDiagramNode::Below;
	// m_label->setPlainText(tr("Label"));
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	setCategory("Core");
	setName("Pixmap");
}

CDiagramPixmapNode::CDiagramPixmapNode(CDiagram *parent, const QString &fileName)
	: CDiagramNode(parent)
{
	m_rotatable = true;

	m_label->setPlainText(tr("Label"));
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	loadFromFile(fileName);
	setCategory("Core");
	setName("Pixmap");
}

CDiagramPixmapNode::~CDiagramPixmapNode()
{

}

void CDiagramPixmapNode::loadFromFile(const QString &fileName)
{
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		m_pixmap.load(fileName);
		//resize(m_pixmap.width(), m_pixmap.height());
		update();
	}
}

void CDiagramPixmapNode::loadFromBinary(const QByteArray &bin)
{
	if (!bin.isEmpty())
	{
		m_pixmap.loadFromData(bin);
		//resize(m_pixmap.width(), m_pixmap.height());
	}
}

void CDiagramPixmapNode::setPixmap(const QPixmap &pixmap)
{
	m_pixmap = pixmap;
	//resize(m_pixmap.width(), m_pixmap.height());
	update();
}

void CDiagramPixmapNode::setImage(const QImage &image)
{
	if (m_pixmap.convertFromImage(image))
	{
		//resize(m_pixmap.width(), m_pixmap.height());
		update();
	}
}

void CDiagramPixmapNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QRectF	target = contentsRect();
	QRect	source;

	if (!m_pixmap.isNull())
	{
		source = m_pixmap.rect();
		painter->save();
		painter->setRenderHint(QPainter::SmoothPixmapTransform);
		painter->drawPixmap(target.toRect(), m_pixmap, source);
		painter->restore();
	}
}

void CDiagramPixmapNode::init(const QVariant &value)
{
	QString		file;
	QFileInfo	fileInfo;

	file = value.toString();
	fileInfo.setFile( file );
	if (fileInfo.exists() && fileInfo.isFile())
	{
		loadFromFile(file);
	}
	else
	{
		loadFromBinary(value.toByteArray());
	}
}

void CDiagramPixmapNode::defaultInit()
{
	if (!m_pixmap.isNull())
		resize(m_pixmap.width(), m_pixmap.height());
}