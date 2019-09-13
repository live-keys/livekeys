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

#include "keymap.h"
#include "live/mlnodetojson.h"
#include <QFile>
#include <QDir>
#include <QJSValue>
#include <QJSValueIterator>

/**
 * \class lv::KeyMap
 * \brief Abstraction of a map which pairs up commands with key shortcuts used to run them
 * \ingroup lvview
 */
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

std::map<qint32, QString> KeyMap::modifierStrings = {
    {KeyMap::Command, "cmd"},
    {KeyMap::Control, "ctrl"},
    {KeyMap::Alt, "alt"},
    {KeyMap::Shift, "shift"},
    {KeyMap::Meta, "meta"}
};

std::map<quint32, QString> KeyMap::stringsForKeys = {
    {Qt::Key_Enter, "enter"},
    {Qt::Key_Escape, "esc"},
    {Qt::Key_Backspace, "bksp"},
    {Qt::Key_Delete, "del"},
    {Qt::Key_Tab, "tab"},
    {Qt::Key_Home, "home"},
    {Qt::Key_End, "end"},
    {Qt::Key_PageUp, "pgup"},
    {Qt::Key_PageDown, "pgdn"},
    {Qt::Key_Left, "left"},
    {Qt::Key_Right, "right"},
    {Qt::Key_Space, "space"}
};

std::map<QString, quint32> KeyMap::keysForStrings = {
    {"enter", Qt::Key_Enter},
    {"escape", Qt::Key_Escape},
    {"backspace", Qt::Key_Backspace},
    {"delete", Qt::Key_Delete},
    {"tab", Qt::Key_Tab},
    {"home", Qt::Key_Home},
    {"end", Qt::Key_End},
    {"pageup", Qt::Key_PageUp},
    {"pagedown", Qt::Key_PageDown},
    {"left", Qt::Key_Left},
    {"right", Qt::Key_Right},
    {"space", Qt::Key_Space}
};

/** Default constructor */
KeyMap::KeyMap(const QString &settingsPath, QObject *parent)
    : QObject(parent)
{
    m_path = QDir::cleanPath(settingsPath + "/keymap.json");
    readFile();
}

/** Default destructor */
KeyMap::~KeyMap(){
}

/** Returns the command paired with the given key */
QString KeyMap::locateCommand(KeyMap::KeyCode key){
    auto it = m_commandMap.find(key);
    if ( it != m_commandMap.end() )
        return it->command;

    return "";
}


/**
 * \brief Store function which pairs the given command with the given key, unless it's not overriding a default command
 *
 * The main store function which eventually gets called by all the other variants.
 */
void KeyMap::store(KeyMap::KeyCode key, const QString &command, bool isDefault){
    if ( isDefault && !locateCommand(key).isEmpty() )
        return;

    m_commandMap[key] = StoredCommand(command, isDefault);
    emit keymapChanged();
}

/** Store function which pairs the key (given by a description) with a given command */
void KeyMap::store(const QString &keydescription, const QString &command, bool isDefault){
    QPair<int, KeyMap::KeyCode> pkc = composeKeyCode(keydescription);
    if ( pkc.first == 0 || pkc.first == KeyMap::KEYBOARD_OS ){
        store(pkc.second, command, isDefault);
    }
}


/** Store command given by the QJSValue object */
void KeyMap::store(const QJSValue &keyObject, bool isDefault){
    QJSValueIterator vit(keyObject);
    while( vit.hasNext() ){
        vit.next();
        store(vit.name(), vit.value().toString(), isDefault);
    }
}

/** Store command under the key given by its components */
void KeyMap::store(quint32 os, quint32 key, quint32 modifier, const QString &command, bool isDefault){
    if ( os != 0 && os != KEYBOARD_OS )
        return;

    store((KeyCode)modifier << 32 | key, command, isDefault);
}

/** Locate command for a key given by its components */
QString KeyMap::locateCommand(quint32 key, quint32 modifiers){
    return locateCommand(composeKeyCode(key, modifiers));
}

/** Returns the modifier containing flags corresponding to the background OS and keys */
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

/** Returns Control or Command depending on the background OS */
KeyMap::Modifier KeyMap::controlOrCommand(){
    if ( KEYBOARD_OS == KeyMap::Mac )
        return KeyMap::Command;

    return KeyMap::Control;
}

/** Adds contents of a file to the keymap */
void KeyMap::readFile(){
    QFile file(m_path);
    if ( file.exists() && file.open(QIODevice::ReadOnly) ){
        MLNode n;
        QByteArray fileContents = file.readAll();
        ml::fromJson(fileContents.data(), n);
        if ( n.type() == MLNode::Object ){
            for ( auto it = n.begin(); it != n.end(); ++it ){
                store(QString::fromStdString(it.key()), QString::fromStdString(it.value().asString()), false);
            }
        }
    }
}

quint32 KeyMap::modifierFromString(const QString &modifier) const {

    std::map<QString, quint32> inv;

    for (auto el: modifierStrings)
    {
        inv[el.second] = static_cast<unsigned int>(el.first);
    }

    return inv[modifier];
}

QString KeyMap::stringFromModifier(const quint32 &modifier) const
{
    std::vector<QString> parts;
    for (auto el: modifierStrings)
    {
        if (modifier & static_cast<unsigned int>(el.first))
            parts.push_back(el.second);
    }

    QString result = "";
    for (QString part: parts)
    {
        if (result != "") result += "+";
        result += part;
    }
    return result;
}

quint32 KeyMap::keyFromString(const QString &key) const {
    quint32 result = keysForStrings[key];
    if (result == 0 && key.size() == 1) result = key.at(0).toLower().unicode();

    return result;
}

QString KeyMap::stringFromKey(const quint32 &key) const
{
    auto result = stringsForKeys[key];
    if (result == "") result = static_cast<QChar>(QChar::toLower(key));

    return result;
}

KeyMap::KeyCode KeyMap::composeKeyCode(quint32 key, quint32 modifiers){
    return (KeyCode)localModifier(modifiers) << 32 | key;
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

/** Creates a description of a key code e.g. Ctrl+K */
QString KeyMap::getKeyCodeDescription(KeyMap::KeyCode kc){

    auto split = splitKeyCode(kc);
    return stringFromModifier(split.first) + "+" + stringFromKey(split.second);
}

}// namespace
