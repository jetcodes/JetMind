/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <QtXml/QtXml>
#include <core/CDiagramItem>
#include <core/CDiagramSvgNode>

CDiagramSvgNode::CDiagramSvgNode(CDiagram *parent)
	: CDiagramNode(parent),
	  m_render( new QSvgRenderer() ),
	  m_ignoreDefaultSize(true),
	  m_skipSizeSettings(true)
{
	m_rotatable = true;

	//connect(m_render, SIGNAL(repaintNeeded()), this, SLOT(update()));
	m_labelPosition = CDiagramNode::Below;
	//m_label->setPlainText(tr("Label"));
	m_label->setAutoWidth(true);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

CDiagramSvgNode::CDiagramSvgNode(CDiagram *parent, const QString &fileName)
	: CDiagramNode(parent),
	  m_render( new QSvgRenderer() ),
	  m_ignoreDefaultSize(false),
	  m_skipSizeSettings(true)
{
	m_rotatable = true;

	//connect(m_render, SIGNAL(repaintNeeded()), this, SLOT(update()));
	m_labelPosition = CDiagramNode::Below;
	m_label->setPlainText(tr("Label"));
	// m_label->setEditMode(CDiagramText::LineEditMode);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	loadFromFile(fileName);
}

CDiagramSvgNode::~CDiagramSvgNode()
{
	SAFE_DELETE(m_render);
}

void CDiagramSvgNode::loadFromString(const QString &str)
{
	m_svgContent = str;
	if (m_render)
	{
		m_render->load(m_svgContent.toUtf8());
	}
}

void CDiagramSvgNode::loadFromFile(const QString &fileName)
{
	QFile			file;
	QString			tmp;
	QTextStream		stream(&file);

	m_svgContent.clear();
	if (m_render)
	{
		m_render->load(fileName);
		file.setFileName(fileName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			tmp = stream.readAll();
			m_svgContent = tmp;
		}
	}
}

void CDiagramSvgNode::loadFromBinary(const QByteArray &bin)
{
	if (m_render)
	{
		m_render->load( bin );
		m_svgContent = QString(bin);
	}
}

void CDiagramSvgNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	
	QRectF	r = contentsRect();
	if (m_render)
	{
		painter->save();
		m_render->render(painter, r);	
		painter->restore();
	}
}

void CDiagramSvgNode::initSize()
{
	m_actSize = m_render->defaultSize();
	if (!m_ignoreDefaultSize)
		resize( m_actSize.width(), m_actSize.height() );
}

void CDiagramSvgNode::init(const QVariant &value)
{
	QString		file;
	QFileInfo	fileInfo;

	file = value.toString();
	fileInfo.setFile( file );

	// Try to load from file
	if (fileInfo.exists() && fileInfo.isFile())
	{
		loadFromFile(file);
	}
	else
	{
		loadFromString( file );
	}
}