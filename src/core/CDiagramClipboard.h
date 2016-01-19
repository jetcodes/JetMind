/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMCLIPBOARD_H
#define PRODRAW_CDIAGRAMCLIPBOARD_H

#include <QtGui>
#include <core/CGlobal>

class CORE_EXPORT CDiagramClipboard
{
public:
	enum Mode {
		Global = 0x01,
		Application,
		All
	}; // End of enum mode

    bool isNonEmpty();
	void clear(Mode mode = Global);
	void setMimeData(QMimeData *data, Mode mode = Global);
	QMimeData* mimeData(Mode mode = Global);
	
	QImage image(Mode mode = Global);
	QPixmap pixmap(Mode mode = Global);
	QString	text(Mode mode = Global);

	static CDiagramClipboard* clipboard()
	{
		if (!m_inst)
			m_inst = new CDiagramClipboard();
		return m_inst;
	}

private:
	CDiagramClipboard();
    Q_DISABLE_COPY(CDiagramClipboard)

	QMimeData	*m_data;
	QClipboard	*m_clipboard;

	static CDiagramClipboard	*m_inst;

}; // End of class CDiagramClipboard

#endif // PRODRAW_CDIAGRAMCLIPBOARD_H
