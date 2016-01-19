/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUTILS_H
#define PRODRAW_CUTILS_H

#include <QtGui>
#include <QtXml>
#include <core/CGlobal>

class CDiagram;
class CDiagramEdgeStyle;

namespace Utils
{
	CORE_EXPORT void drawEdgeStyleHelper(QPainter *painter, CDiagramEdgeStyle *style, const QRectF &target, qreal padding);
	CORE_EXPORT QSizeF getPageSize(QPrinter::PageSize type, bool landscape = false);
	CORE_EXPORT QDomElement genXmlElement(QDomDocument &doc, const QString &name, const QString &text);
	CORE_EXPORT QString boolToString(bool value);
	CORE_EXPORT bool stringToBool(const QString &value);
	CORE_EXPORT	bool isValidWebUrl(const QString &value);
	CORE_EXPORT bool isValidLocalPath(const QString &value);
    CORE_EXPORT QPoint getPopupPoint(QGraphicsScene *scene, QWidget *hostedWidget, QToolButton *button, QWidget *widget);
    CORE_EXPORT bool getDiagramFromXml(const QDomElement &n, CDiagram *d);
    CORE_EXPORT bool getXmlFromDiagram(const CDiagram *d, QDomElement &n);

	// Font icon related functions
	CORE_EXPORT void setFontIcon(QToolButton *btn, const QChar &code, int ps = 12, const QString &family = "FontAwesome");
	CORE_EXPORT void setFontIcon(QPushButton *btn, const QChar &code, int ps = 12, const QString &family = "FontAwesome");

} // End of namespace CUtils

#endif // PRODRAW_CUTILS_H
