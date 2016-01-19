/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include "CMindMapSettings.h"

CMindMapSetting* CMindMapSetting::m_inst = NULL;

CMindMapSetting::CMindMapSetting()
{

}

void CMindMapSetting::set(const QString &key, const QVariant &value)
{
    QSettings       setting;

    setting.setValue(key, value);
    emit configChanged(key);
    emit configChanged();
}

QVariant CMindMapSetting::get(const QString &key, const QVariant defaultValue)
{
    QSettings       setting;

    if (setting.contains(key))
        return setting.value(key);
    else
        return defaultValue;
}
