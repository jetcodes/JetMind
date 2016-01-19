/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CTEXTBLOCK_H
#define PRODRAW_CTEXTBLOCK_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramNode>

class CDiagram;

class CORE_EXPORT CDiagramTextNode : public CDiagramNode
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramTextNode(CDiagram *parent);
	virtual ~CDiagramTextNode();

	QRectF boundingRect() const;
	QRectF contentsRect() const;

	void setEditing(bool editing);
	bool editing() { return m_editing; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void setHtml(const QString &s);
	QString html();

	void setPlainText(const QString &s);
	QString plainText();

	void startGrowing(const QPointF &p);
	bool onGrowing(const QPointF &p);
	bool stopGrowing(const QPointF &p);

	void setTextStyleable(bool value);
	bool textStyleable() const;

public Q_SLOTS:
	void onAddedToDiagram();
	void layoutContent();
	virtual void onTextSizeChanged();

protected:
	Q_DISABLE_COPY(CDiagramTextNode)

	bool		m_editing;

}; // End of class CDiagramTextNode

#endif // PRODRAW_CTEXTBLOCK_H
