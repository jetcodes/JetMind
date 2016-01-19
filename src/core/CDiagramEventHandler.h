/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMEVENTHANDLER_H
#define PRODRAW_CDIAGRAMEVENTHANDLER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramPrivateData;

class CORE_EXPORT CDiagramEventHandler
{
public:
	CDiagramEventHandler()
		: m_priority(0)
	{
	}

	virtual ~CDiagramEventHandler()
	{
	}

    QString name() { return m_name; }

	int priority() const { return m_priority; }
	void setPriority(int value) { m_priority = value >= 0 ? value : m_priority; }

	virtual bool onEvent(CDiagramPrivateData *d, QEvent *event) { return false; }

protected:
	int			m_priority;
    QString     m_name;

private:
    Q_DISABLE_COPY(CDiagramEventHandler)

}; // End of class CDiagramEventHandler

#endif // PRODRAW_CDIAGRAMEVENTHANDLER_H
