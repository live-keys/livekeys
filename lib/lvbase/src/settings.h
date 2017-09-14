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

#ifndef LVSETTINGS_H
#define LVSETTINGS_H

#include <QObject>
#include <QMap>
#include "live/lvbaseglobal.h"

namespace lcv{

class LVBASE_EXPORT Settings : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool previewMode READ previewMode CONSTANT)
    Q_PROPERTY(QString path     READ path        CONSTANT)

public:
    ~Settings();

    static Settings* initialize(const QString& path, QObject* parent = 0);

    bool previewMode() const;
    void setPreviewMode(bool previewMode);

    const QString& path() const;

public slots:
    QObject* file(const QString& key);
    void addConfigFile(const QString& key, QObject* object);

private:
    Settings(const QString& path, QObject* parent = 0);

private:
    QString m_path;
    bool    m_previewMode;
    QMap<QString, QObject*>* m_configFiles;
};

inline bool Settings::previewMode() const{
    return m_previewMode;
}

inline void Settings::setPreviewMode(bool previewMode){
    m_previewMode = previewMode;
}

inline const QString &Settings::path() const{
    return m_path;
}

}// namespace

#endif // LVSETTINGS_H
