/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRRAW_WIDGETSGLOBAL_H
#define PRODRRAW_WIDGETSGLOBAL_H

#if defined(WIDGETS_LIB)
	#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
	#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif // 

#endif // PRODRRAW_WIDGETSGLOBAL_H