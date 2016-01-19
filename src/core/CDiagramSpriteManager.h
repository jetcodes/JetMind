/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_DIAGRAMSPRITEMANAGER_H
#define PRODRAW_DIAGRAMSPRITEMANAGER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramItem;
class CDiagramSprite;
class CDiagramSpriteManager;

class CORE_EXPORT CDiagramSpriteManager: public QObject
{
	Q_OBJECT

public:
	CDiagramSpriteManager();
	virtual ~CDiagramSpriteManager();

	virtual void init(CDiagram *d);
	virtual void uninit(CDiagram *d);

	void addSprite(CDiagramSprite *sprite);
	void removeSprite(CDiagramSprite *c);

	int maxPriority(int type);

public	Q_SLOTS:
	void onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect);

protected:
	QList<CDiagramSprite*>* sprites(int type);

	QMap<int, QList<CDiagramSprite*>* >		m_sprites;
	QMap<int, CDiagramSprite*>				m_actives;
	CDiagram								*m_d;

private:
	Q_DISABLE_COPY(CDiagramSpriteManager)
	
}; // End of class CDiagramSpriteManager

#endif // PRODRAW_DIAGRAMSPRITEMANAGER_H