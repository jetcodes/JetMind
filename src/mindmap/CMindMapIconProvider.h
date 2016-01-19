/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPICONPROVIDER_H
#define PRODRAW_CMINDMAPICONPROVIDER_H

#include <QtGui>

/* CMindMapIconProvider is used to manage all the
icons availiable to the users. All Icons must be named
using numbers, like '000.png', '123.png', so each icon
can be identified by its file name and be procceed.
*/
class CMindMapIconProvider
{
public:
    // load icons from the resources
    void load();

    QList<QIcon> getAllIcons() const;
    QList<QIcon> getAllAlterIcons(int &index) const;
    QIcon getNextAlterIcon(int &index) const;
    QIcon getIconByIndex(int index);
    bool checkConflict(int idx1, int idx2);

	QList<QString> getAllPixmaps() const;
	QList<QIcon> getIconsForPixmaps() const;
	QPixmap getPixmapByIndex(int index);

    // retrieve the instance of icon provider
    static CMindMapIconProvider* instance()
    {
        if (!m_inst)
            m_inst = new CMindMapIconProvider();
        return m_inst;
    }

protected:
    CMindMapIconProvider();
    int getIconGroupByIndex(int &index) const;
	void loadIcons();
	void loadPixmaps();

private:
    Q_DISABLE_COPY(CMindMapIconProvider)

    static CMindMapIconProvider     *m_inst;
    QList<int>                      m_marks;
    QList<QList<QIcon> >            m_icons;
    QMap<int, int>                  m_typeMap;
	QList<QString>					m_pixmaps;
	QList<QIcon>					m_thumbnails;

}; // End of class CMindMapIconProvider

#endif // PRODRAW_CMINDMAPICONPROVIDER_H
