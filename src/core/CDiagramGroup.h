/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMGROUP_H
#define PRODRAW_CDIAGRAMGROUP_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramItem>

class QDomNode;
class CDiagramItem;
class CDiagramGroup;

class CORE_EXPORT CDiagramGroup : public CDiagramItem
{
	Q_OBJECT

public:
	Q_INVOKABLE CDiagramGroup(CDiagram	*parent);
	virtual ~CDiagramGroup();

	int interType() const { return Global::Group; }

	QRectF boundingRect() const;
	QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	void initGroup(const QList<CDiagramItem*> &items);
	void destroyGroup();
	QList<CDiagramItem*> items();

	bool isObscuredBy(const QGraphicsItem *item) const;
    QPainterPath opaqueArea() const;

	void fromXml(const QDomElement &n);
	void toXml(QDomElement &n);

public Q_SLOTS:
	void onDiagramModeChanged(CDiagram::DiagramMode mode);
	void onPropertyChanged(const QString &name, const QVariant &value);
	void onGeometryChanged();

protected:
	virtual void layoutContent();
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event );

private:
    Q_DISABLE_COPY(CDiagramGroup)

}; // End of class CDiagramGroup

#endif // PRODRAW_CDIAGRAMGROUP_H
