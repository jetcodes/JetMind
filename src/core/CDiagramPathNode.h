/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMPATHITEM_H
#define PRODRAW_CDIAGRAMPATHITEM_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagramNode>

class CDiagram;

class CORE_EXPORT CDiagramPathNode : public CDiagramNode
{
	Q_OBJECT
	Q_PROPERTY(bool closed READ isClosed WRITE setClosed)
	
public:
	CDiagramPathNode(CDiagram *parent = NULL);
	virtual ~CDiagramPathNode();

	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void setClosed(bool value);
	bool isClosed() { return m_closed; }

protected:
	virtual QPainterPath computePath();

	QPainterPath		m_path;
	bool				m_closed;

}; // End of class CDiagramPathNode

#endif // PRODRAW_CDIAGRAMPATHITEM_H
