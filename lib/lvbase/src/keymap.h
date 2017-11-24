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

#ifndef LVKEYMAP_H
#define LVKEYMAP_H

#include <QObject>
#include <QMap>
#include "live/lvbaseglobal.h"

namespace lv{

class LV_BASE_EXPORT KeyMap : public QObject{

    Q_OBJECT
    Q_ENUMS(Modifier)
    Q_ENUMS(Os)

public:
    enum Modifier{
        Control = 1,
        Alt = 2,
        Command = 4,
        Shift = 8,
        Meta = 16
    };
    enum Os{
        Windows = 1,
        Linux = 2,
        Mac = 3
    };

    typedef unsigned long long KeyCode;

    class StoredCommand{
    public:
        StoredCommand() : isDefault(false){}
        StoredCommand(const QString& pcommand, bool pisDefault) : isDefault(pisDefault), command(pcommand){}

        bool       isDefault;
        QString    command;
    };

    static const KeyMap::Os KEYBOARD_OS;
    static const KeyMap::Modifier CONTROL_OR_COMMAND;

public:
    KeyMap(const QString& settingsPath, QObject* parent = 0);
    ~KeyMap();

    QString locateCommand(KeyCode key);
    void store(KeyCode key, const QString& command, bool isDefault = true);
    void store(const QString& keydescription, const QString& command, bool isDefault = true);
    void store(quint32 os, quint32 key, quint32 localModifer, const QString& command, bool isDefault = true);

public slots:
    QString locateCommand(quint32 key, quint32 modifiers);
    quint32 cleanKey(quint32 key);
    quint32 localModifier(quint32 modifier);
    Modifier controlOrCommand();
    void readFile();

private:
    quint32 modifierFromString(const QString& modifier);
    quint32 keyFromString(const QString& key);
    KeyCode composeKeyCode(quint32 key, quint32 modifiers);
    QPair<int, KeyCode> composeKeyCode(const QString& keydescription);
    QPair<quint32, quint32> splitKeyCode(KeyCode kc);
    QString getKeyCodeDescription(KeyCode kc);


    QMap<KeyCode, StoredCommand> m_commandMap;
    QString m_path;
};

}// namespace

#endif // LVKEYMAP_H
