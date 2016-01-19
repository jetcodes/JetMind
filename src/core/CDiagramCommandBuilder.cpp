/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramCommand>
#include <core/CDiagramCommandBuilder>

CDiagramCommandBuilder*	CDiagramCommandBuilder::m_builder = NULL;

CDiagramCommandBuilder::CDiagramCommandBuilder()
{

}

void CDiagramCommandBuilder::registerMetaObject(const QString &name, const QMetaObject *meta, bool replace)
{
	if (!name.isEmpty() && meta && (replace || !m_metaObjects.contains(name)))
	{
		m_metaObjects[name] = const_cast<QMetaObject*>(meta);
	}
}

CDiagramCommand* CDiagramCommandBuilder::createCommand(const QString &name, 
													   QGenericArgument val0,
													   QGenericArgument val1, 
													   QGenericArgument val2, 
													   QGenericArgument val3, 
													   QGenericArgument val4, 
													   QGenericArgument val5, 
													   QGenericArgument val6, 
													   QGenericArgument val7, 
													   QGenericArgument val8, 
													   QGenericArgument val9 )
{
	CDiagramCommand		*cmd = NULL;
	QMetaObject			*meta = NULL;

	if (m_metaObjects.contains(name))
	{
		meta = m_metaObjects[name];
		cmd = dynamic_cast<CDiagramCommand*>( meta->newInstance(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9) );
	}
	return cmd;
}