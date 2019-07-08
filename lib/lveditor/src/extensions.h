#ifndef LVEXTENSIONS_H
#define LVEXTENSIONS_H

#include <QObject>
#include <QQmlPropertyMap>

#include "live/lvviewglobal.h"
#include "live/workspaceextension.h"
#include "live/package.h"

namespace lv{

class ViewEngine;
class LV_EDITOR_EXPORT Extensions : public QObject{

    Q_OBJECT

public:
    Extensions(ViewEngine* engine, const QString& settingsPath, QObject *parent = 0);
    ~Extensions();

    QQmlPropertyMap *globals();

    void loadExtensions();

    QMap<std::string, WorkspaceExtension*>::iterator begin();
    QMap<std::string, WorkspaceExtension*>::iterator end();

private:
    WorkspaceExtension* loadPackageExtension(const std::string& path);
    WorkspaceExtension* loadPackageExtension(const Package::Ptr& package);

    QMap<std::string, WorkspaceExtension*> m_extensions;

    QQmlPropertyMap*     m_globals;
    QString              m_path;
    ViewEngine*          m_engine;
};

/** Globals getter */
inline QQmlPropertyMap* Extensions::globals(){
    return m_globals;
}

/** Begin iterator of extensions */
inline QMap<std::string, WorkspaceExtension*>::iterator Extensions::begin(){
    return m_extensions.begin();
}

/** End iterator of extensions */
inline QMap<std::string, WorkspaceExtension*>::iterator Extensions::end(){
    return m_extensions.end();
}

} // namespace

#endif // LVEXTENSIONS_H
