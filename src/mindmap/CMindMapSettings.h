/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPSETTINGS_H
#define PRODRAW_CMINDMAPSETTINGS_H

#include <QtGui>

class CMindMapSetting;

namespace Settings
{
    const QString           MindMapBackgroundBrush = "mindmap/background/brush";
    const QString           MindMapBackgroundShowGrid = "mindmap/background/showgrid";
    const QString           MindMapToolBarAutoHide = "mindmap/toolbar/autohide";
    const QString           MindMapToolBarCollapsible = "mindmap/toolbar/collapsible";
    const QString           MindMapToolBarAlignment = "mindmap/toolbar/alignment";
    const QString           MindMapFilterType = "mindmap/filter/type";
    const QString           MindMapFilterCaseSensitive = "mindmap/filter/casesensitive";

} // End of namespace Settings

class CMindMapSetting: public QObject
{
    Q_OBJECT

public:
    void set(const QString &key, const QVariant &value);
    QVariant get(const QString &key, const QVariant defaultValue = QVariant());

    static CMindMapSetting* instance()
    {
        if (!m_inst)
            m_inst = new CMindMapSetting();
        return m_inst;
    }

Q_SIGNALS:
    void configChanged();
    void configChanged(const QString &key);

private:
    CMindMapSetting();
    Q_DISABLE_COPY(CMindMapSetting)


    static CMindMapSetting          *m_inst;

}; // End of class CMindMapSetting

#endif // PRODRAW_CMINDMAPSETTINGS_H
