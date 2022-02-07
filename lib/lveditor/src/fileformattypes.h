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

#ifndef LVFILEFORMATTYPES_H
#define LVFILEFORMATTYPES_H

#include <QObject>
#include <QJSValue>

namespace lv{

class FileFormatTypes : public QObject{

    Q_OBJECT

private:
    class Entry{
    public:
        QString matchName;
        QString matchExtension;
        QString matchWildCard;
        QString type;

        bool isMatch(const QString& fileName);
    };

public:
    explicit FileFormatTypes(QObject *parent = nullptr);
    ~FileFormatTypes();

    void add(const QJSValue& entries);
    void addEntry(const QString& type, const QString& name = "", const QString& extension = "", const QString& match = "");

public slots:
    QString find(const QString& path);
    QString fileFilterFromFormat(const QString& format);

private:
    QList<Entry> m_entries;
};

}// namespace

#endif // FILEFORMATTYPES_H
