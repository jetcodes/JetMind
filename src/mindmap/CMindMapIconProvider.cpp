/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include "CMindMapIconProvider.h"

CMindMapIconProvider* CMindMapIconProvider::m_inst = NULL;

CMindMapIconProvider::CMindMapIconProvider()
{
}

void CMindMapIconProvider::load()
{
	loadIcons();
	loadPixmaps();
}

void CMindMapIconProvider::loadIcons()
{
    QString             file;
    QString             prefix(":/mindstorm/library/default/%1.png");
	QString				pixmapPrefix(":/mindstorm/library/pixmaps/*1.png");
    QMap<int, QIcon>    icons;
    QList<int>          groups;
    int                 group = 0, count = 0, iconCount = 173;
    QList<QIcon>        iconList;

    m_marks.clear();
    m_icons.clear();
    m_typeMap.clear();

    // config icon types belongs to the same group
    groups <<  3 << 4
          <<  5 << 10
          << 11 << 17
          << 18 << 19
          << 20 << 24
          << 25 << 26
          << 27 << 28
          << 29 << 30
          << 31 << 32
          << 33 << 34
          << 35 << 40
          << 43 << 44
          << 46 << 47
          << 52 << 53
          << 71 << 77
          << 97 << 100
          << 101 << 102
          << 103 << 104
          << 105 << 108
          << 128 << 131
          << 132 << 140;

    // fetch all the icons inside the
    // resources
    for (int i = 1; i <= iconCount - 1; ++i)
    {
        file = prefix.arg( QString::number(i).rightJustified(3, QChar('0')) );
        icons.insert(i, QIcon(file));
    }

    // first, load the icons belongs to some
    // type group
    for (int i = 0; i < groups.length(); i+=2)
    {
        int             start, end;
        QList<QIcon>    tmp;

        start = groups.at(i);
        end = groups.at(i + 1);
        for (int j = start; j <= end; ++j)
        {
            tmp.push_back(icons[j]);
            icons.remove(j);
            m_typeMap.insert(count, group);
            ++count;
        }

        // m_marks records the number of icons
        // in the specified group
        m_marks.push_back(end - start + 1);

        // m_icons records the icons of a specified
        // group
        m_icons.push_back(tmp);

        ++group;
    }

    // second, load the icons do not belong to
    // some kind of group
    iconList = icons.values();
    foreach (QIcon icon, iconList)
    {
        QList<QIcon>   tmp;

        tmp << icon;
        m_icons.push_back(tmp);
    }
}

void CMindMapIconProvider::loadPixmaps()
{

}

QList<QIcon> CMindMapIconProvider::getAllIcons() const
{
    QList<QIcon>            icons;

    foreach (QList<QIcon> ics, m_icons)
    {
        icons.append(ics);
    }

    return icons;
}

QList<QIcon> CMindMapIconProvider::getAllAlterIcons(int &index) const
{
    int             group = 0, recIdx = 0;
    QList<QIcon>    icons;

    recIdx = index;
    group = getIconGroupByIndex(index);
    if (group >= 0 && group < m_marks.length())
    {
        icons = m_icons[group];
        index = recIdx - index;
    }

    return icons;
}

QIcon CMindMapIconProvider::getNextAlterIcon(int &index) const
{
    int             group = 0, idx = 0, recIdx = 0;
    QList<QIcon>    icons;

    recIdx = index;
    group = getIconGroupByIndex(index);
    if (group >= 0 && group < m_marks.length() )
    {
        icons = m_icons.at(group);
        idx = (index + 1) % icons.length();
        index = recIdx - index + idx;
        return icons.at(idx);
    }

    return QIcon();
}

QIcon CMindMapIconProvider::getIconByIndex(int index)
{
    QIcon           icon;
    QList<QIcon>    icons;

    icons = getAllIcons();
    if (index >= 0 && index < icons.length())
        icon = icons.at(index);
    return icon;
}

int CMindMapIconProvider::getIconGroupByIndex(int &index) const
{
    int     i = 0;

    while (true && i < m_marks.length() )
    {
        if (index + 1 - m_marks[i] <= 0 )
            break;
        index -= m_marks[i];
        ++i;
    }

    return i;
}

bool CMindMapIconProvider::checkConflict(int idx1, int idx2)
{
    bool    r = false;

    if (idx1 == idx2)
        return true;

    if (m_typeMap.contains(idx1) && m_typeMap.contains(idx2))
    {
        r = (m_typeMap[idx1] == m_typeMap[idx2]);
    }

    return r;
}

QList<QString> CMindMapIconProvider::getAllPixmaps() const
{
	return m_pixmaps;
}

QList<QIcon> CMindMapIconProvider::getIconsForPixmaps() const
{
	return m_thumbnails;
}

QPixmap CMindMapIconProvider::getPixmapByIndex(int index)
{
	return QPixmap();
}