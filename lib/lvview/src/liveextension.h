#ifndef LIVEEXTENSION_H
#define LIVEEXTENSION_H

#include "live/lvviewglobal.h"
#include <QObject>
#include <QJSValue>

namespace lv{

//TODO: Configuration

class LV_VIEW_EXPORT LiveExtension : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* globals           READ globals           WRITE setGlobals           NOTIFY globalsChanged)
    Q_PROPERTY(QJSValue interceptLanguage READ interceptLanguage WRITE setInterceptLanguage NOTIFY interceptLanguageChanged)
    Q_PROPERTY(QJSValue interceptMenu     READ interceptMenu     WRITE setInterceptMenu     NOTIFY interceptMenuChanged)
    Q_PROPERTY(QJSValue commands          READ commands          WRITE setCommands          NOTIFY commandsChanged)
    Q_PROPERTY(QJSValue keyBindings       READ keyBindings       WRITE setKeyBindings       NOTIFY keyBindingsChanged)
    Q_PROPERTY(QJSValue configuration     READ configuration     WRITE setConfiguration     NOTIFY configurationChanged)

public:
    explicit LiveExtension(QObject *parent = 0);

    QObject* globals() const{ return m_globals; }
    QJSValue interceptLanguage() const { return m_interceptLanguage; }
    QJSValue commands() const{ return m_commands; }
    QJSValue configuration() const{ return m_configuration; }
    QJSValue keyBindings() const{ return m_keybindings; }
    QJSValue interceptMenu() const{ return m_interceptMenu; }

    void setGlobals(QObject* globals);
    void setInterceptLanguage(QJSValue interceptLanguage);
    void setCommands(QJSValue commands);
    void setConfiguration(QJSValue configuration);
    void setKeyBindings(QJSValue keybindings);
    void setInterceptMenu(QJSValue interceptMenu);

    void setIdentifiers(const std::string& name, const std::string& path);
    const std::string& name() const{ return m_name; }
    const std::string& path() const{ return m_path; }

    bool hasLanguageInterceptor() const;
    QObject* callLanguageInterceptor(const QJSValueList& values);

    bool hasMenuInterceptor() const;
    QJSValue callMenuInterceptor(const QJSValueList& values);

signals:
    void globalsChanged();
    void interceptLanguageChanged();
    void commandsChanged();
    void configurationChanged();
    void keyBindingsChanged();
    void interceptMenuChanged();

private:
    std::string m_name;
    std::string m_path;

    QObject* m_globals;
    QJSValue m_interceptLanguage;
    QJSValue m_commands;
    QJSValue m_configuration;
    QJSValue m_keybindings;
    QJSValue m_interceptMenu;
};

inline void LiveExtension::setGlobals(QObject* globals){
    m_globals = globals;
    emit globalsChanged();
}

inline void LiveExtension::setInterceptLanguage(QJSValue interceptLanguage){
    m_interceptLanguage = interceptLanguage;
    emit interceptLanguageChanged();
}

inline void LiveExtension::setCommands(QJSValue commands){
    m_commands = commands;
    emit commandsChanged();
}

inline void LiveExtension::setConfiguration(QJSValue configuration){
    m_configuration = configuration;
    emit configurationChanged();
}

inline void LiveExtension::setKeyBindings(QJSValue keybindings){
    m_keybindings = keybindings;
    emit keyBindingsChanged();
}

inline void LiveExtension::setIdentifiers(const std::string &name, const std::string &path){
    m_name = name;
    m_path = path;
}

inline void LiveExtension::setInterceptMenu(QJSValue interceptMenu){
    m_interceptMenu = interceptMenu;
    emit interceptMenuChanged();
}

}// namespace

#endif // LIVEEXTENSION_H
