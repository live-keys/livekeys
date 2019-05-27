#ifndef LVWORKSPACELAYER_H
#define LVWORKSPACELAYER_H

#include <QObject>
#include <QQmlPropertyMap>

#include "live/lvviewglobal.h"
#include "live/layer.h"
#include "live/keymap.h"

#include "commands.h"
#include "extensions.h"

namespace lv{

class WorkspaceLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* project            READ project    NOTIFY projectChanged)
    Q_PROPERTY(QObject* panes              READ panes      NOTIFY panesChanged)
    Q_PROPERTY(lv::Commands* commands      READ commands   CONSTANT)
    Q_PROPERTY(lv::KeyMap* keymap          READ keymap     CONSTANT)
    Q_PROPERTY(QQmlPropertyMap* extensions READ extensions CONSTANT)

public:
    explicit WorkspaceLayer(QObject *parent = nullptr);
    ~WorkspaceLayer();

    void loadView(ViewEngine *engine, QObject *parent) Q_DECL_OVERRIDE;
    QObject* nextViewParent() Q_DECL_OVERRIDE;

    QObject* project() const;
    QObject* panes() const;

    lv::Commands* commands() const;
    lv::KeyMap* keymap() const;
    QQmlPropertyMap* extensions() const;

public slots:
    QJSValue interceptMenu(QJSValue context);

signals:
    void projectChanged();
    void panesChanged();

private:
    QObject* m_nextViewParent;

    QObject* m_project;
    QObject* m_panes;

    Commands* m_commands;
    KeyMap*   m_keymap;

    lv::Extensions* m_extensions;
};

inline QObject *WorkspaceLayer::project() const{
    return m_project;
}

inline QObject *WorkspaceLayer::panes() const{
    return m_panes;
}

inline Commands *WorkspaceLayer::commands() const{
    return m_commands;
}

inline KeyMap *WorkspaceLayer::keymap() const{
    return m_keymap;
}

inline QQmlPropertyMap *WorkspaceLayer::extensions() const{
    if ( m_extensions )
        return m_extensions->globals();
    return nullptr;
}

}// namespace

#endif // LVWORKSPACELAYER_H
