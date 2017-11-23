/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef LVEDITORSETTINGS_H
#define LVEDITORSETTINGS_H

#include <QObject>
#include <QHash>
#include "live/lveditorglobal.h"
#include <QDebug>
namespace lv{

class ProjectDocument;
class EditorSettingsCategory;
class LV_EDITOR_EXPORT EditorSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString path READ path     CONSTANT)

public:
    explicit EditorSettings(const QString& path, QObject *parent = 0);
    ~EditorSettings();

    int fontSize() const;

    const QString& path() const;

    void fromJson(const QJsonObject& root);
    QJsonObject toJson() const;

    EditorSettingsCategory* settingsFor(const QString& key);
    void addSetting(const QString& key, EditorSettingsCategory* category);

public slots:
    void syncWithFile();
    void init(const QByteArray& data);
    const QByteArray& content() const;
    void documentOpened(lv::ProjectDocument* document);
    void documentChanged();

signals:
    void initError(const QString& errorString);
    void fontSizeChanged(int fontSize);

private:
    int        m_fontSize;
    QString    m_path;
    QByteArray m_content;

    QHash<QString, EditorSettingsCategory*> m_settings;
};

inline int EditorSettings::fontSize() const{
    return m_fontSize;
}

inline const QString &EditorSettings::path() const{
    return m_path;
}

inline EditorSettingsCategory *EditorSettings::settingsFor(const QString &key){
    QHash<QString, EditorSettingsCategory*>::iterator it = m_settings.find(key);
    if ( it == m_settings.end() )
        return 0;
    return it.value();
}

inline void EditorSettings::addSetting(const QString &key, EditorSettingsCategory *category){
    m_settings[key] = category;
}

inline const QByteArray &EditorSettings::content() const{
    return m_content;
}

}// namespace

#endif // LVEDITORSETTINGS_H
