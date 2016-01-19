/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMPIXMAPITEM_H
#define PRODRAW_CDIAGRAMPIXMAPITEM_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramNode>

class CDiagram;
class CDiagramItem;
class CDiagramNode;

class CORE_EXPORT CDiagramPixmapNode : public CDiagramNode
{
	Q_OBJECT
	Q_PROPERTY(QPixmap pixmap READ getPixmap WRITE setPixmap)

public:
	Q_INVOKABLE CDiagramPixmapNode(CDiagram *parent = NULL);
	Q_INVOKABLE CDiagramPixmapNode(CDiagram *parent,const QString &fileName);
	virtual ~CDiagramPixmapNode();

	void loadFromFile(const QString &fileName);
	void loadFromBinary(const QByteArray &contents);
	void setPixmap(const QPixmap &pixmap);
	void setImage(const QImage &image);
	const QPixmap& getPixmap() { return m_pixmap; }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);
	void init(const QVariant &value);
	void defaultInit();

private:
	Q_DISABLE_COPY(CDiagramPixmapNode)

	QPixmap		m_pixmap;

}; // End of class CDiagramPixmapNode

#endif // PRODRAW_CDIAGRAMPIXMAPITEM_H
