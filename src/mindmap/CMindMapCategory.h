/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPCATEGORY_H
#define PRODRAW_CMINDMAPCATEGORY_H

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>

class CMindMapCategory :public QObject, public CDiagramItemTypeCategory
{
	Q_OBJECT

public:
    CMindMapCategory();
	void load();

}; // End of class CMindMapCategory

#endif // PRODRAW_CMINDMAPCATEGORY_H
