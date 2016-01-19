/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMSERIALIZABLE_H
#define PRODRAW_CDIAGRAMSERIALIZABLE_H

#include <QtGui>

class CDiagramSerializable
{
public:
	virtual void toXml(QDomElement &e) = 0;
	virtual void fromXml(const QDomElement &e) = 0;

}; // End of class CDiagramSerializable

Q_DECLARE_METATYPE(CDiagramSerializable*);

#endif // PRODRAW_CDIAGRAMSERIALIZABLE_H
