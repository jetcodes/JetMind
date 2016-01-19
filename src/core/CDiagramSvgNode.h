/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CSVGNODE_H
#define PRODRAW_CSVGNODE_H

#include <QtGui>
#include <QSvgRenderer>
#include <core/CGlobal>
#include <core/CDiagramNode>

class CORE_EXPORT CDiagramSvgNode : public CDiagramNode
{
	Q_OBJECT
	Q_PROPERTY(QString svg READ getSvgContent WRITE loadFromString)
	Q_PROPERTY(bool skipSizeSettings READ skipSizeSettings WRITE setSkipSizeSettings)
	Q_PROPERTY(bool ignoreDefaultSize READ ignoreDefaultSize WRITE setIgnoreDefaultSize)

public:
	Q_INVOKABLE CDiagramSvgNode(CDiagram *parent = NULL);
	Q_INVOKABLE CDiagramSvgNode(CDiagram *parent,const QString &fileName);
	virtual ~CDiagramSvgNode();

	void loadFromFile(const QString &fileName);
	void loadFromBinary(const QByteArray &contents);
	void loadFromString(const QString &content);
	QString getSvgContent() { return m_svgContent; }

	void setIgnoreDefaultSize(bool value) { m_ignoreDefaultSize = value; }
	bool ignoreDefaultSize() { return m_ignoreDefaultSize; }

	void setSkipSizeSettings(bool value) { m_skipSizeSettings = value; }
	bool skipSizeSettings() { return m_skipSizeSettings; }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);
	void init(const QVariant &value);

private:
	void initSize();

	Q_DISABLE_COPY(CDiagramSvgNode)

	QSvgRenderer		*m_render;
	QString				m_svgContent;
	QSize				m_actSize;
	bool				m_ignoreDefaultSize;
	bool				m_skipSizeSettings;
};

#endif // PRODRAW_CSVGNODE_H
