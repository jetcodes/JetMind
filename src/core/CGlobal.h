/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_GLOBAL_H
#define PRODRAW_GLOBAL_H

#include <QtCore/QtGlobal>
#include <QtXml/QtXml>

// MAKE THE PROJET A SHARED LIBRARY OR JUST LINK
// TO THE LIBRARY
#if defined(PRODRAWCORE_LIB)
	#define CORE_EXPORT Q_DECL_EXPORT
#else
	#define CORE_EXPORT Q_DECL_IMPORT
#endif

// OTHER STUFF
#define DIAGRAM_FILE_VERSION  "1.0"
#define SAFE_DELETE(p)		  if ((p)) { delete (p); (p) = NULL; }

#ifndef DS_PROPERTY
	#define DS_PROPERTY(name)  setProperty((#name), QVariant::fromValue((CDiagramSerializable*)(name)))
#endif //

#ifndef RS_PROPERTY
	#define RS_PROPERTY(name)   (property(name).value<CDiagramSerializable*>())
#endif //

#include <QtGui>

namespace Global
{
	const QString DND_ITEMMIMEDATA = QString("application/x-dnditemmimedata");
	const QString DND_FORMATMIMEDATA = QString("application/x-dndformatmimedata");
	const QString DND_DRAGITEMSMIMEDATA = QString("application/x-dnddragitemsmimedata");
	const QString CTG_COREITEMS = QString("Core");
	const int	  DEF_TITLEHEIGHT = 24;

	// System default command names
	const QString CMD_PASTE = QString("paste");
	const QString CMD_NEWITEM = QString("newitem");
	const QString CMD_DELETEITEMS = QString("deleteitems");
	const QString CMD_MOVEITEMS = QString("moveitems");
	const QString CMD_DRAGGINGITEMS = QString("draggingitems");
	const QString CMD_RESIZE = QString("resize");
	const QString CMD_RESIZE2 = QString("resize2");
	const QString CMD_ROTATE = QString("rotate");
	const QString CMD_ADJUSTROUTE = QString("adjustroute");
	const QString CMD_RECONNECT = QString("reconnect");
	const QString CMD_CHANGEPROPERTY = QString("changeproperty");
	const QString CMD_EDITTEXT = QString("edittext");
	const QString CMD_DRAGCOPYITEMS = QString("dragcopy");
	const QString CMD_DRAGADDITEM = QString("dragadd");
	const QString CMD_GROUP = QString("group");
	const QString CMD_UNGROUP = QString("ungroup");

	// Diagram Environment keys
	const QString SYM_DB_ROUTER = QString("database/router");
	const QString SYM_DB_ARROWSTYLE = QString("database/arrow");
	const QString SYM_DB_ITEM = QString("database/item");
	const QString SYM_DB_CONTEXT = QString("database/context");

	// Diagram context keys 
	const QString KEY_STARTARROW = QString("startarrow");
	const QString KEY_ENDARROW = QString("endarrow");
	const QString KEY_EDGEROUTER = QString("edgeStyle");
	const QString KEY_EDGECOLOR = QString("edgeColor");
	const QString KEY_EDGEWEIGHT = QString("edgeWeight");
	const QString KEY_EDGESTYLE = QString("edgeStyle");

	// Context keys
	const QString CXT_CTRLMANAGER = QString("ctrlmanager");
	const QString CXT_HANDLERMANAGER = QString("handlermanager");
	const QString CXT_EVENTOBSERVER = QString("eventobserver");

	// Directions
	enum CORE_EXPORT Directions {
		North = 0,
		West,
		South,
		East
	}; // End of enumeration of Direction

	// Dragger's direction
	enum CORE_EXPORT DragPolicy {
		BothDirections = 0,
		Vertically,
		Horizontally
	}; // End of enumeration DragPolicy

	// Graphics Scene item's type id
	enum CORE_EXPORT ItemType
	{
		DiagramItem = 100,
		DiagramItemHandler,
		DiagramItemHandlerDragger,
		DiagramPort,
		DiagramText,
		DiagramToolBox,
		DiagramToolBar,
		DiagramLayer
	}; // End of QtItemTypes

	// Internal types of different items
	enum CORE_EXPORT InterItemType {
		Undefined = 0,
		Node,
		Edge,
		Line,
		Widget,
		Group
	}; // End of Item Types

	// Format brush
	enum CORE_EXPORT FormatBrushType {
		Pen = 0,
		Font,
		Brush,
		StartArrow,
		EndArrow,
		EdgeStyle,
		TextAlign
	}; // End of FormatBrushType

	enum CORE_EXPORT MatchSizeFlag {
		MatchMinimumWidth = 0x1,
		MatchMinimumHeight = 0x2,
		MatchMaximumWidth = 0x3,
		MatchMaximumHeight = 0x4
	}; // End of EqualSizeFlag

	struct FormatBrush {
		int			type;
		QVariant	value;
	}; // End of FormatBrush

}; // End of namespace Global

Q_DECLARE_METATYPE(Global::FormatBrush)

#endif // PRODRAW_GLOBAL_H