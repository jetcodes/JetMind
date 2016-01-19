/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMCOMMAND_H
#define PRODRAW_CDIAGRAMCOMMAND_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;

class CORE_EXPORT CDiagramCommand: public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	CDiagramCommand(CDiagram *d, QUndoCommand *parent = 0)
		: QUndoCommand(parent),
		  m_d(d)
	{
	};

protected:
	CDiagram		*m_d;

}; // End of class CDiagramCommand

#endif // PRODRAW_CDIAGRAMCOMMAND_H
