/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include "CDiagramClipboard.h"

CDiagramClipboard* CDiagramClipboard::m_inst = NULL;

CDiagramClipboard::CDiagramClipboard()
	: m_data(NULL),
	  m_clipboard(NULL)
{
	m_clipboard = QApplication::clipboard();
	m_data = new QMimeData();
}

bool CDiagramClipboard::isNonEmpty()
{
    return (m_clipboard->mimeData()->hasText() ||
            m_clipboard->mimeData()->hasImage() ||
            m_data->hasImage() ||
            m_data->hasText());
}

void CDiagramClipboard::clear(CDiagramClipboard::Mode mode)
{
	switch(mode)
	{
	case Global:
		m_clipboard->clear();
		break;

	case Application:
		m_data->clear();
		break;

	default:
		m_clipboard->clear();
		m_data->clear();
		break;
	};
}

void CDiagramClipboard::setMimeData(QMimeData *data, CDiagramClipboard::Mode mode)
{
	QString		format;
	QStringList	formats;

	switch(mode)
	{
	case Global:
		m_clipboard->setMimeData(data);
		break;

	case Application:
		if (data)
		{
			formats = data->formats();
			for (int i = 0; i < formats.length(); ++i)
			{
				format = formats.at(i);
				m_data->setData(format, data->data(format));
			}
		}
		break;

	default:
		break;
	}; //
}

QMimeData* CDiagramClipboard::mimeData(CDiagramClipboard::Mode mode)
{
	QMimeData	*r = NULL;

	switch(mode)
	{
	case Global:
		r = const_cast<QMimeData*>( m_clipboard->mimeData() );
		break;

	case Application:
		r = m_data;
		break;

	default:
		break;
	}; //
	return r;
}

QImage CDiagramClipboard::image(CDiagramClipboard::Mode mode)
{
	QImage		img;
	QMimeData	*r = mimeData(mode);

	if (r && r->hasImage())
	{
		img = qvariant_cast<QImage>(r->imageData());
	}
	return img;
}

QPixmap CDiagramClipboard::pixmap(CDiagramClipboard::Mode mode)
{
	QPixmap		img;
	QMimeData	*r = mimeData(mode);
	
	if (r && r->hasImage())
	{
		img = qvariant_cast<QPixmap>(r->imageData());
	}
	return img;
}

QString CDiagramClipboard::text(CDiagramClipboard::Mode mode)
{
	QString		s;
	QMimeData	*r = mimeData(mode);

	if (r && r->hasText())
	{
		s = r->text();
	}
	return s;
}
