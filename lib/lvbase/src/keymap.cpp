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

#include "keymap.h"
#include "mlnodetojson.h"
#include <QFile>
#include <QDir>

namespace lv{

/*
Modifier keys 	cmd ctrl alt shift meta
Special keys 	enter escape backspace delete tab home end pageup pagedown left right up down space
Os keys         win mac linux
Example         win:ctrl+space : window.editor.assistCompletion
*/

#if defined(Q_OS_DARWIN)
#define LV_KEYBOARD_OS_MAC
#elif defined(Q_OS_MSDOS)
#define LV_KEYBOARD_OS_WINDOWS
#elif defined(Q_OS_WIN32)
#define LV_KEYBOARD_OS_WINDOWS
#elif defined(Q_OS_WINCE)
#define LV_KEYBOARD_OS_WINDOWS
#elif defined(Q_OS_CYGWIN)
#define LV_KEYBOARD_OS_WINDOWS
#else
#define LV_KEYBOARD_OS_LINUX
#endif


#if defined(LV_KEYBOARD_OS_MAC)
const KeyMap::Os KeyMap::KEYBOARD_OS = KeyMap::Mac;
const KeyMap::Modifier KeyMap::CONTROL_OR_COMMAND = KeyMap::Command;
#elif defined(LV_KEYBOARD_OS_WINDOWS)
const KeyMap::Os KeyMap::KEYBOARD_OS = KeyMap::Windows;
const KeyMap::Modifier KeyMap::CONTROL_OR_COMMAND = KeyMap::Control;
#else
const KeyMap::Os KeyMap::KEYBOARD_OS = KeyMap::Linux;
const KeyMap::Modifier KeyMap::CONTROL_OR_COMMAND = KeyMap::Control;
#endif

KeyMap::KeyMap(const QString &settingsPath, QObject *parent)
    : QObject(parent)
{
    m_path = QDir::cleanPath(settingsPath + "/keymap.json");
    readFile();
}

KeyMap::~KeyMap(){
}

QString KeyMap::locateCommand(KeyMap::KeyCode key){
    auto it = m_commandMap.find(key);
    if ( it != m_commandMap.end() )
        return it->command;

    return "";
}

void KeyMap::store(KeyMap::KeyCode key, const QString &command, bool isDefault){
    if ( isDefault && !locateCommand(key).isEmpty() )
        return;

    m_commandMap[key] = StoredCommand(command, isDefault);
}

void KeyMap::store(const QString &keydescription, const QString &command, bool isDefault){
    QPair<int, KeyMap::KeyCode> pkc = composeKeyCode(keydescription);
    if ( pkc.first == 0 || pkc.first == KeyMap::KEYBOARD_OS ){
        store(pkc.second, command, isDefault);
    }
}

void KeyMap::store(quint32 os, quint32 key, quint32 modifier, const QString &command, bool isDefault){
    if ( os != 0 && os != KEYBOARD_OS )
        return;

    store((KeyCode)modifier << 32 | key, command, isDefault);
}

QString KeyMap::locateCommand(quint32 key, quint32 modifiers){
    return locateCommand(composeKeyCode(key, modifiers));
}

quint32 KeyMap::cleanKey(quint32 key){
    return key;
}

quint32 KeyMap::localModifier(quint32 modifier){
    quint32 result = 0;

#ifdef LV_KEYBOARD_OS_MAC
    if ( modifier & Qt::ControlModifier )
        result |= KeyMap::Command;
    if ( modifier & Qt::AltModifier )
        result |= KeyMap::Alt;
    if ( modifier & Qt::MetaModifier )
        result |= KeyMap::Control;
    if ( modifier & Qt::ShiftModifier )
        result |= KeyMap::Shift;
#else
    if ( modifier & Qt::ControlModifier )
        result |= KeyMap::Control;
    if ( modifier & Qt::AltModifier )
        result |= KeyMap::Alt;
    if ( modifier & Qt::MetaModifier )
        result |= KeyMap::Meta;
    if ( modifier & Qt::ShiftModifier )
        result |= KeyMap::Shift;
#endif
    return result;
}

KeyMap::Modifier KeyMap::controlOrCommand(){
    if ( KEYBOARD_OS == KeyMap::Mac )
        return KeyMap::Command;

    return KeyMap::Control;
}

void KeyMap::readFile(){
    QFile file(m_path);
    if ( file.exists() && file.open(QIODevice::ReadOnly) ){
        MLNode n;
        ml::fromJson(file.readAll(), n);
        if ( n.type() == MLNode::Object ){
            for ( auto it = n.begin(); it != n.end(); ++it ){
                store(QString::fromStdString(it.key()), QString::fromStdString(it.value().asString()), false);
            }
        }
    }
}

quint32 KeyMap::modifierFromString(const QString &modifier){
    if ( modifier == "cmd" )
        return KeyMap::Command;
    else if ( modifier == "ctrl" ){
        return KeyMap::Control;
    } else if ( modifier == "alt" ){
        return KeyMap::Alt;
    } else if ( modifier == "shift" ){
        return KeyMap::Shift;
    } else if ( modifier == "meta" ){
        return KeyMap::Meta;
    } else {
        return 0;
    }
}

quint32 KeyMap::keyFromString(const QString &key){
    if ( key == "enter" ){
        return Qt::Key_Enter;
    } else if ( key == "escape" ){
        return Qt::Key_Escape;
    } else if ( key == "backspace" ){
        return Qt::Key_Backspace;
    } else if ( key == "delete" ){
        return Qt::Key_Delete;
    } else if ( key == "tab" ){
        return Qt::Key_Tab;
    } else if ( key == "home" ){
        return Qt::Key_Home;
    } else if ( key == "end" ){
        return Qt::Key_End;
    } else if ( key == "pageup" ){
        return Qt::Key_PageUp;
    } else if ( key == "pagedown" ){
        return Qt::Key_PageDown;
    } else if ( key == "left" ){
        return Qt::Key_Left;
    } else if ( key == "right" ){
        return Qt::Key_Right;
    } else if ( key == "escape" ){
        return Qt::Key_Escape;
    } else if ( key == "space" ){
        return Qt::Key_Space;
    } else if ( key.size() == 1 ){
        return key.at(0).toUpper().unicode();
    }
    return 0;
}

KeyMap::KeyCode KeyMap::composeKeyCode(quint32 key, quint32 modifiers){
    return (KeyCode)localModifier(modifiers) << 32 | cleanKey(key);
}

QPair<int, KeyMap::KeyCode> KeyMap::composeKeyCode(const QString &keydescription){
    quint32 modifier = 0;
    quint32 key = 0;
    int os = 0;

    QStringList keysegments;

    int osIndex = keydescription.indexOf(':');
    if ( osIndex != -1 ){
        QString osstr = keydescription.mid(0, osIndex);
        if ( osstr == "win" )
            os = KeyMap::Windows;
        else if ( osstr == "linux" )
            os = KeyMap::Linux;
        else if ( osstr == "mac" )
            os = KeyMap::Mac;
    } else {
        keysegments = keydescription.split("+");
    }

    if ( keysegments.isEmpty() )
        return QPair<int, KeyMap::KeyCode>(0, 0);

    for ( auto it = keysegments.begin(); it != keysegments.end(); ++it ){
        quint32 mapping = modifierFromString(*it);
        if ( mapping != 0 ){
            modifier |= mapping;
        } else {
            key = keyFromString(*it);
        }
    }

    return QPair<int, KeyMap::KeyCode>(os, (KeyCode)modifier << 32 | key);
}

QPair<quint32, quint32> KeyMap::splitKeyCode(KeyCode kc){
    return QPair<quint32, quint32>((quint32)(kc >> 32), (quint32)kc); //modifiers first
}

QString KeyMap::getKeyCodeDescription(KeyMap::KeyCode kc){
    auto split = splitKeyCode(kc); //TODO
    return "";
}

}// namespace
