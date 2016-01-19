/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramIconProvider>

#include "CMindMapNode.h"
#include "CMindMapCategory.h"

CMindMapCategory::CMindMapCategory()
    : CDiagramItemTypeCategory( NODE_CATEGORY )
{

}

void CMindMapCategory::load()
{
	QString					name, desc;
	QMetaObject				*meta = NULL;
	CDiagramItem			*item = NULL;
	CDiagramIconProvider	*p = NULL;

	p = CDiagramIconProvider::instance();
	
	meta = const_cast<QMetaObject*>(&CMindMapNode::staticMetaObject);
    //item = static_cast<CDiagramItem*>(meta->newInstance(Q_ARG(CDiagram*, NULL)));
    name = QString::fromUtf8( NODE_NAME );
    desc = QString::fromUtf8( NODE_NAME );
    addType(new CDiagramItemType(name, desc, QIcon(), meta) );
}
