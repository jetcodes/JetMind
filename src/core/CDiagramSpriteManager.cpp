/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramSprite>
#include <core/CDiagramSpriteManager>

bool priorityGreaterThan(CDiagramSprite *s1, CDiagramSprite *s2)
{
	bool	r = true;

	if (s1 == NULL && s2 == NULL) 
		r = false;
	else if (s1 == NULL)
		r = true;
	else if (s2 == NULL)
		r = false;
	else
		r = s1->priority() > s2->priority(); 

	return r;
}

CDiagramSpriteManager::CDiagramSpriteManager()
	: m_d(NULL)
{
	m_sprites.clear();
	m_actives.clear();
}

CDiagramSpriteManager::~CDiagramSpriteManager()
{
}

void CDiagramSpriteManager::onSelectionChanged(QList<CDiagramItem*> &items, const QRectF &rect)
{
	int												type;
	CDiagramSprite									*sprite = NULL;
	QMap<int, CDiagramSprite*>::iterator			loop1;
	QMap<int, QList<CDiagramSprite*>* >::iterator	loop2;

	if (!m_d) 
		return;

	for (loop1 = m_actives.begin(); loop1 != m_actives.end(); ++loop1)
	{
		sprite = loop1.value();
		if (sprite != NULL)
		{
			type = sprite->spriteType();
			sprite->uninit(m_d);
			if (m_actives.contains(type))
				m_actives[type] = NULL;
		}
	}
	
	if (!items.isEmpty())
	{
		for (loop2 = m_sprites.begin(); loop2 != m_sprites.end(); ++loop2)
		{
			foreach (sprite, *loop2.value())
			{
				if (sprite->support(items))
				{
					type = sprite->spriteType();
					if (m_actives[type])
						m_actives[type]->uninit(m_d);
					m_actives[type] = sprite;
					sprite->init(m_d, items, rect);
					break;
				}
			}
		}
	}
}

void CDiagramSpriteManager::init(CDiagram *d)
{
	QList<CDiagramItem*>	items;

	if (!d || d == m_d)
		return;
	
	// setup the only connection
	connect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*> &, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF&)));
	m_d = d;

	// If there are already selected items, then just notify the
	// selection
	items = m_d->selectedTopLevelItems();
	if (items.length() > 0)
	{
		onSelectionChanged(items, m_d->getItemsBoundingRect(true));
	}
}

void CDiagramSpriteManager::uninit(CDiagram *d)
{
	CDiagramSprite							*sprite = NULL;
	QMap<int, CDiagramSprite*>::iterator	loop;

	if (!d || d != m_d) 
		return;
	
	// clear out all the sprites that are still active.
	for (loop = m_actives.begin(); loop != m_actives.end(); ++loop)
	{
		sprite = loop.value();
		if (sprite)
			sprite->uninit(d);
	}

	// clear out the only connection
	disconnect(d, SIGNAL(itemsSelectionChanged(QList<CDiagramItem*> &, const QRectF &)), this, SLOT(onSelectionChanged(QList<CDiagramItem*>&, const QRectF&)));

	m_d = NULL;
}

void CDiagramSpriteManager::addSprite(CDiagramSprite *sprite)
{
	int							type;
	QList<CDiagramSprite*>		sprites;

	if (!sprite) 
		return;

	type = sprite->spriteType();

	if (!m_sprites.contains(type))
	{
		m_sprites.insert(type, new QList<CDiagramSprite*>() );
		m_actives.insert(type, NULL);
	}

	if ( !m_sprites[type]->contains(sprite) )
	{
		m_sprites[type]->push_back(sprite);
		qSort(m_sprites[type]->begin(), m_sprites[type]->end(), priorityGreaterThan);
	}
}

void CDiagramSpriteManager::removeSprite(CDiagramSprite *sprite)
{
	int		type;

	if (!sprite)
		return;
	
	type = sprite->spriteType();
	if (m_sprites.contains(type))
	{
		m_sprites[type]->removeAll(sprite);
	}
}

QList<CDiagramSprite*>*	CDiagramSpriteManager::sprites(int type)
{
	QList<CDiagramSprite*>	*sprites = NULL;

	if (m_sprites.contains(type) )
	{
		sprites = m_sprites[type];
	}

	return sprites;
}