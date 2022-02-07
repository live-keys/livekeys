/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef LVSETTINGS_H
#define LVSETTINGS_H

#include <QObject>
#include <QMap>
#include "live/lvviewglobal.h"

namespace lv{

class LV_VIEW_EXPORT Settings : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString path    READ path       CONSTANT)

public:
    ~Settings();

    static Settings* create(const QString& path, QObject* parent = nullptr);

    /** Returns the path of the config folder */
    const QString& path() const;

public slots:
    QObject* file(const QString& key);
    void addConfigFile(const QString& key, QObject* object);

private:
    Settings(const QString& path, QObject* parent = nullptr);

private:
    QString m_path;
    QMap<QString, QObject*>* m_configFiles;
};

inline const QString &Settings::path() const{
    return m_path;
}

}// namespace

#endif // LVSETTINGS_H
