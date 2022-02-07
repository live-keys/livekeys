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

#ifndef LVEDITORSETTINGS_H
#define LVEDITORSETTINGS_H

#include <QObject>
#include <QHash>

#include "live/lveditorglobal.h"
#include "live/mlnode.h"

namespace lv{

class ProjectDocument;
class LV_EDITOR_EXPORT EditorSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString path READ path     CONSTANT)

public:
    explicit EditorSettings(const QString& path, QObject *parent = nullptr);
    ~EditorSettings();

    /** Returns the font size */
    int fontSize() const;
    const QString& path() const;

    void fromJson(const MLNode &root);
    MLNode toJson() const;

    MLNode readFor(const QString& key);
    void write(const QString& key, const MLNode& settings);

public slots:
    void syncWithFile();
    void init(const QByteArray& data);
    const QByteArray& content() const;
    void documentOpened(lv::ProjectDocument* document);
    void documentChanged();


signals:
    /** Signals error with given string */
    void initError(const QString& errorString);
    /** Font size changed */
    void fontSizeChanged(int fontSize);
    /** Signals when a resync is done from the file */
    void refresh();

private:
    int        m_fontSize;
    QString    m_path;
    QByteArray m_content;

    QHash<QString, MLNode> m_settings;
};

/**
 * \brief Returns the font size
 */
inline int EditorSettings::fontSize() const{
    return m_fontSize;
}

/** Returns path of the settings */
inline const QString &EditorSettings::path() const{
    return m_path;
}

/** Find settings for given key */
inline MLNode EditorSettings::readFor(const QString &key){
    auto it = m_settings.find(key);
    if ( it == m_settings.end() )
        return MLNode();
    return it.value();
}

/** Write settings for given key */
inline void EditorSettings::write(const QString &key, const MLNode& settings){
    m_settings[key] = settings;
}

/**
 * \brief Settings content getter
 */
inline const QByteArray &EditorSettings::content() const{
    return m_content;
}

}// namespace

#endif // LVEDITORSETTINGS_H
