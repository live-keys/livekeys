/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVKEYMAP_H
#define LVKEYMAP_H

#include "live/lveditorglobal.h"

#include <QObject>
#include <QMap>
#include <QJSValue>
#include <map>

namespace lv{

class ViewEngine;

class LV_EDITOR_EXPORT KeyMap : public QObject{

    Q_OBJECT
    Q_ENUMS(Modifier)
    Q_ENUMS(Os)

public:
    /** List of all possible modifiers */
    enum Modifier{
        Control = 1,
        Alt = 2,
        Command = 4,
        Shift = 8,
        Meta = 16
    };
    /** List of operating systems */
    enum Os{
        Windows = 1,
        Linux = 2,
        Mac = 3
    };

    /** unsigned long long */
    typedef unsigned long long KeyCode;

    /// \private
    class StoredCommand{
    public:
        StoredCommand() : isDefault(false){}
        StoredCommand(const QString& pcommand, bool pisDefault) : isDefault(pisDefault), command(pcommand){}

        bool       isDefault;
        QString    command;
        QString    whenActivePane;
        QString    whenActiveItem;
        QString    whenExpression;
    };

    /** Indicates which OS is in use, with matching keyboard */
    static const KeyMap::Os KEYBOARD_OS;
    /** Indicates if the common modifier is the Ctrl or Cmd */
    static const KeyMap::Modifier CONTROL_OR_COMMAND;
    /** Strings matching the modifiers */
    static std::map<qint32, QString> modifierStrings;
    /** Short names for common keys */
    static std::map<quint32, QString> stringsForKeys;
    /** Keys that match the given names */
    static std::map<QString, quint32> keysForStrings;

public:
    KeyMap(const QString& settingsPath, QObject* parent = nullptr);
    ~KeyMap();

    QString locateCommand(KeyCode key);
    QList<StoredCommand> locateCommands(KeyCode key);
    void store(KeyCode key, const QString& command, QJSValue when = QJSValue(), bool isDefault = true);
    void store(const QString& keydescription, const QString& command, QJSValue when = QJSValue(), bool isDefault = true);
    void store(const QJSValue &keyObject, bool isDefault = true);
    void store(quint32 os, quint32 key, quint32 localModifer, const QString& command, QJSValue when = QJSValue(), bool isDefault = true);

    /** Exposes the command mp */
    QMap<KeyCode, QList<StoredCommand> >& commandMap() { return m_commandMap; }
    QString getKeyCodeDescription(KeyCode kc);

public slots:
    QString locateCommand(quint32 key, quint32 modifiers);
    quint32 localModifier(quint32 modifier);
    Modifier controlOrCommand();
    void readFile();

signals:
    /** Signals a change in the keymap */
    void keymapChanged();

private:
    quint32 modifierFromString(const QString& modifier) const;
    QString stringFromModifier(const quint32& modifier) const;
    quint32 keyFromString(const QString& key) const;
    QString stringFromKey(const quint32& key) const;
    KeyCode composeKeyCode(quint32 key, quint32 modifiers);
    QPair<int, KeyCode> composeKeyCode(const QString& keydescription);
    QPair<quint32, quint32> splitKeyCode(KeyCode kc);


    QMap<KeyCode, QList<StoredCommand> > m_commandMap;
    QString     m_path;
    ViewEngine* m_engine;
};

}// namespace

#endif // LVKEYMAP_H
