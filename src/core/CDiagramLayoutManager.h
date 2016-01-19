/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMLAYOUTMANAGER_H
#define PRODRAW_CDIAGRAMLAYOUTMANAGER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramLayout;

class CORE_EXPORT CDiagramLayoutManager
{
public:
	CDiagramLayout* getLayout(const QString &key);
	void addLayout(const QString &key, CDiagramLayout *layout);
	QList<CDiagramLayout*> layouts();
	QList<QString> keys();

	static CDiagramLayoutManager* inst();

private:
	CDiagramLayoutManager();
	Q_DISABLE_COPY(CDiagramLayoutManager);

	QMap<QString, CDiagramLayout*>		m_layouts;
	static CDiagramLayoutManager		*m_inst;
}; // End of class CDiagramLayoutManager

#endif // PRODRAW_CDIAGRAMLAYOUTMANAGER_H
