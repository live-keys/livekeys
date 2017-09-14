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

#ifndef QEDITORSETTINGS_H
#define QEDITORSETTINGS_H

#include <QObject>
#include <QHash>
#include "live/qlcveditorglobal.h"

namespace lcv{

class QProjectDocument;
class QEditorSettingsCategory;
class Q_LCVEDITOR_EXPORT QEditorSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString path READ path     CONSTANT)

public:
    explicit QEditorSettings(const QString& path, QObject *parent = 0);
    ~QEditorSettings();

    int fontSize() const;

    const QString& path() const;

    void fromJson(const QJsonObject& root);
    QJsonObject toJson() const;

    QEditorSettingsCategory* settingsFor(const QString& key);
    void addSetting(const QString& key, QEditorSettingsCategory* category);

public slots:
    void syncWithFile();
    void init(const QByteArray& data);
    const QByteArray& content() const;
    void documentOpened(lcv::QProjectDocument* document);
    void documentIsDirtyChanged();

signals:
    void initError(const QString& errorString);
    void fontSizeChanged(int fontSize);

private:
    int        m_fontSize;
    QString    m_path;
    QByteArray m_content;

    QHash<QString, QEditorSettingsCategory*> m_settings;
};

inline int QEditorSettings::fontSize() const{
    return m_fontSize;
}

inline const QString &QEditorSettings::path() const{
    return m_path;
}

inline QEditorSettingsCategory *QEditorSettings::settingsFor(const QString &key){
    QHash<QString, QEditorSettingsCategory*>::iterator it = m_settings.find(key);
    if ( it == m_settings.end() )
        return 0;
    return it.value();
}

inline void QEditorSettings::addSetting(const QString &key, QEditorSettingsCategory *category){
    m_settings[key] = category;
}

inline const QByteArray &QEditorSettings::content() const{
    return m_content;
}

}// namespace

#endif // QEDITORSETTINGS_H
