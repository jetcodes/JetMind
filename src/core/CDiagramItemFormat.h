/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMITEMFORMAT_H
#define PRODRAW_CDIAGRAMITEMFORMAT_H

#include <QtGui>
#include <core/CGlobal>

class QDataStream;
class CDiagramItem;

class CDiagramItemFormatPrivate;

class CORE_EXPORT CDiagramItemFormat
{
public:
	CDiagramItemFormat();

	QString desc();
	QIcon icon();

	void toStream(QDataStream &stream);
	void fromStream(QDataStream &stream);

	bool initFrom(CDiagramItem* item);
	bool support(CDiagramItem *item);
	void apply(CDiagramItem *item);

	static QList<CDiagramItemFormat*> formats(CDiagramItem* item);

protected:
	CDiagramItemFormatPrivate	*m_p;

}; // End of class CDiagramItemFormat

#endif // PRODRAW_CDIAGRAMITEMFORMAT_H
