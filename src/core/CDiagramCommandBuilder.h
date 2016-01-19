/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMCOMMANDBUILDER_H
#define PRODRAW_CDIAGRAMCOMMANDBUILDER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagramCommand;
class CDiagramCommandBuilder;

class CORE_EXPORT CDiagramCommandBuilder
{
public:
	void registerMetaObject(const QString &name, const QMetaObject* obj, bool replace = false);
	CDiagramCommand *createCommand(const QString &name, 
								   QGenericArgument val0 = QGenericArgument( 0 ), 
								   QGenericArgument val1 = QGenericArgument(), 
								   QGenericArgument val2 = QGenericArgument(), 
								   QGenericArgument val3 = QGenericArgument(), 
								   QGenericArgument val4 = QGenericArgument(), 
								   QGenericArgument val5 = QGenericArgument(), 
								   QGenericArgument val6 = QGenericArgument(), 
								   QGenericArgument val7 = QGenericArgument(), 
								   QGenericArgument val8 = QGenericArgument(), 
								   QGenericArgument val9 = QGenericArgument());

private:
	CDiagramCommandBuilder();
	Q_DISABLE_COPY(CDiagramCommandBuilder)
	QMap<QString, QMetaObject*>		m_metaObjects;

public:
	static CDiagramCommandBuilder* instance()
	{
		if (!m_builder)
			m_builder = new CDiagramCommandBuilder();
		return m_builder;
	};
	static CDiagramCommandBuilder	*m_builder;

}; // End of class CDiagramCommandBuilder

#endif // CDIAGRAMCOMMANDBUILDER_H
