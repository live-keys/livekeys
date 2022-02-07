/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVWORKSPACEEXTENSION_H
#define LVWORKSPACEEXTENSION_H

#include "live/lveditorglobal.h"
#include <QObject>
#include <QJSValue>

namespace lv{

class LV_EDITOR_EXPORT WorkspaceExtension : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* globals           READ globals           WRITE setGlobals           NOTIFY globalsChanged)
    Q_PROPERTY(QJSValue interceptFile     READ interceptFile     WRITE setInterceptFile     NOTIFY interceptFileChanged)
    Q_PROPERTY(QJSValue interceptLanguage READ interceptLanguage WRITE setInterceptLanguage NOTIFY interceptLanguageChanged)
    Q_PROPERTY(QJSValue menuInterceptors  READ menuInterceptors  WRITE setMenuInterceptors  NOTIFY menuInterceptorsChanged)
    Q_PROPERTY(QJSValue fileFormats       READ fileFormats       WRITE setFileFormats       NOTIFY fileFormatsChanged)
    Q_PROPERTY(QJSValue panes             READ panes             WRITE setPanes             NOTIFY panesChanged)
    Q_PROPERTY(QJSValue commands          READ commands          WRITE setCommands          NOTIFY commandsChanged)
    Q_PROPERTY(QJSValue keyBindings       READ keyBindings       WRITE setKeyBindings       NOTIFY keyBindingsChanged)
    Q_PROPERTY(QJSValue configuration     READ configuration     WRITE setConfiguration     NOTIFY configurationChanged)
    Q_PROPERTY(QJSValue themes            READ themes            WRITE setThemes            NOTIFY themesChanged)

public:
    explicit WorkspaceExtension(QObject *parent = nullptr);

    /** Returns globals */
    QObject* globals() const{ return m_globals; }
    /** Returns language interceptor */
    QJSValue interceptLanguage() const { return m_interceptLanguage; }
    /** Returns commands */
    QJSValue commands() const{ return m_commands; }
    /** Returns configuration */
    QJSValue configuration() const{ return m_configuration; }
    /** Returns key bindings */
    QJSValue keyBindings() const{ return m_keybindings; }
    /** Returns the menu interceptor */
    QJSValue menuInterceptors() const{ return m_menuInterceptors; }
    /** Returns the file interceptor */
    QJSValue interceptFile() const{ return m_interceptFile; }
    /** Returns the panes */
    QJSValue panes() const{ return m_panes; }
    /** Returns the themes */
    QJSValue themes() const{ return m_themes; }
    /** Returns the file formats */
    QJSValue fileFormats() const{ return m_fileFormats; }

    void setGlobals(QObject* globals);
    void setInterceptLanguage(QJSValue interceptLanguage);
    void setCommands(QJSValue commands);
    void setConfiguration(QJSValue configuration);
    void setKeyBindings(QJSValue keybindings);
    void setMenuInterceptors(QJSValue menuInterceptors);
    void setInterceptFile(QJSValue interceptFile);
    void setPanes(QJSValue panes);
    void setThemes(QJSValue themes);
    void setFileFormats(QJSValue fileFormats);

    void setIdentifiers(const std::string& name, const std::string& path);
    /** Name getter */
    const std::string& name() const{ return m_name; }
    /** Path getter */
    const std::string& path() const{ return m_path; }

    bool hasFileInterceptor() const;
    QJSValue callFileInterceptor(const QJSValueList& values);

    bool hasLanguageInterceptor() const;
    QObject* callLanguageInterceptor(const QJSValueList& values);

    bool hasMenuInterceptors() const;
    QJSValue callMenuInterceptors(const QJSValueList& values);

signals:
    /** Signals that globals changed */
    void globalsChanged();
    /** Signals that language interceptor changed */
    void interceptLanguageChanged();
    /** Signals that comannds changed */
    void commandsChanged();
    /** Signals that config changed */
    void configurationChanged();
    /** Signals that key bindings changed */
    void keyBindingsChanged();
    /** Signals that menu interceptor changed */
    void menuInterceptorsChanged();
    /** Signals that the file interceptor changed */
    void interceptFileChanged();
    /** Signals that the panes property has changed */
    void panesChanged();
    /** Signals that the themes property has changed */
    void themesChanged();
    /** Signals that the fileFormats property has changed */
    void fileFormatsChanged();

private:
    std::string m_name;
    std::string m_path;

    QObject* m_globals;
    QJSValue m_interceptLanguage;
    QJSValue m_commands;
    QJSValue m_configuration;
    QJSValue m_keybindings;
    QJSValue m_menuInterceptors;
    QJSValue m_interceptFile;
    QJSValue m_panes;
    QJSValue m_themes;
    QJSValue m_fileFormats;
};

/** Globals setter */
inline void WorkspaceExtension::setGlobals(QObject* globals){
    m_globals = globals;
    emit globalsChanged();
}

/** Language interceptor setter */
inline void WorkspaceExtension::setInterceptLanguage(QJSValue interceptLanguage){
    m_interceptLanguage = interceptLanguage;
    emit interceptLanguageChanged();
}

/** Commands setter */
inline void WorkspaceExtension::setCommands(QJSValue commands){
    m_commands = commands;
    emit commandsChanged();
}

/** Configuration setter */
inline void WorkspaceExtension::setConfiguration(QJSValue configuration){
    m_configuration = configuration;
    emit configurationChanged();
}

/** Key bindings setter */
inline void WorkspaceExtension::setKeyBindings(QJSValue keybindings){
    m_keybindings = keybindings;
    emit keyBindingsChanged();
}

/** Identifiers setters (name and path that identify the extension) */
inline void WorkspaceExtension::setIdentifiers(const std::string &name, const std::string &path){
    m_name = name;
    m_path = path;
}

/** Sets the intercept file callback handler */
inline void WorkspaceExtension::setInterceptFile(QJSValue interceptFile){
    m_interceptFile = interceptFile;
    emit interceptFileChanged();
}

/** Set given intercept menu */
inline void WorkspaceExtension::setMenuInterceptors(QJSValue interceptors){
    m_menuInterceptors = interceptors;
    emit menuInterceptorsChanged();
}

/** Sets the panes */
inline void WorkspaceExtension::setPanes(QJSValue panes){
    m_panes = panes;
    emit panesChanged();
}

/** Sets the themes */
inline void WorkspaceExtension::setThemes(QJSValue themes){
    m_themes = themes;
    emit themesChanged();
}

/** Sets the fileFormats */
inline void WorkspaceExtension::setFileFormats(QJSValue fileFormats){
    m_fileFormats = fileFormats;
    emit fileFormatsChanged();
}

}// namespace

#endif // LVWORKSPACEEXTENSION_H
