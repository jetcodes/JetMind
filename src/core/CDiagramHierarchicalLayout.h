/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMHIERARCHICALLAYOUT_H
#define PRODRAW_CDIAGRAMHIERARCHICALLAYOUT_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramLayout>

class CORE_EXPORT CDiagramHierarchicalLayout : public CDiagramLayout
{
	Q_OBJECT

public:
	CDiagramHierarchicalLayout();
	virtual ~CDiagramHierarchicalLayout();

	enum LayoutMode {

	}; // End of enum

	virtual void layoutDiagram(CDiagram *diagram, const QVariant &params);
	virtual void layoutItem(CDiagramItem *item, const QVariant &params);

private:
	
}; // End of class CDiagramHierarchicalLayout

#endif // PRODRAW_CDIAGRAMHIERARCHICALLAYOUT_H
