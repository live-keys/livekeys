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

#include "live/editorsettings.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/mlnodetojson.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

namespace lv{

/**
 * \class lv::EditorSettings
 * \brief Wrapper around a settings file for the editor
 *
 * We create small category nodes to add custom settings
 * by assigning a key for each category in a JSON file and fetching it from a settings file.
 * \ingroup lveditor
 */

/**
 * \brief Default constructor
 */
EditorSettings::EditorSettings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_fontSize(12)
    , m_path(path)
{
}

/**
 * \brief Blank destructor
 */
EditorSettings::~EditorSettings(){
}

/**
 * \fn lv::EditorSettings::initError
 * \brief Signals error with the given string
 */

/**
 * \fn lv::EditorSettings::fontSizeChanged
 * \brief Signals the fontSize property changed.
 */

/**
 * \brief Populates the settings from a given JSON object
 */
void EditorSettings::fromJson(const MLNode &root){
    for( auto it = root.begin(); it != root.end(); ++it ){
        if ( it.key() == "font" ){
            MLNode n = it.value();
            if ( n.hasKey("size") ){
                m_fontSize = n["size"].asInt();
                emit fontSizeChanged(m_fontSize);
            }
        } else {
            m_settings[QString::fromStdString(it.key())] = it.value();
        }
    }
}

/**
 * @brief Creates a JSON object from the settings
 */
lv::MLNode EditorSettings::toJson() const{
    MLNode root(MLNode::Object);
    MLNode font(MLNode::Object);
    font["size"] = m_fontSize;
    root["font"] = font;

    for ( auto it = m_settings.begin(); it != m_settings.end(); ++it){
        root[it.key().toStdString()] = it.value();
    }

    return root;
}

/**
 * \brief Synchronizes the settings with the file in a bidirectional manner
 *
 * If no file, we store the settings in one. If the file is open, we read from it.
 */
void EditorSettings::syncWithFile(){
    QFile file(m_path);
    if ( !file.exists() ){
        std::string result;
        ml::toJson(toJson(), result);
        m_content = QByteArray::fromStdString(result);
        if ( file.open(QIODevice::WriteOnly) ){
            file.write(m_content);
            file.close();
        } else {
            qCritical("Failed to open settings file for writting: %s", qPrintable(m_path));
        }
    } else if ( file.open(QIODevice::ReadOnly) ){
        init(file.readAll());
        file.close();
    }
}

/**
 * \brief Initializes the settings from given data
 */
void EditorSettings::init(const QByteArray &data){
    MLNode n;
    ml::fromJson(data.constData(), n);
    fromJson(n);
    emit refresh();
}

/**
 * \brief Sets up the listener for changes to the project document
 *
 * This happens when the user opens the settings file in LiveKeys
 */
void EditorSettings::documentOpened(lv::ProjectDocument *document){
    connect(document, SIGNAL(isDirtyChanged()), this, SLOT(documentChanged()));
}

/**
 * \brief Triggers re-initialization of settings from the settings file on every document change
 */
void EditorSettings::documentChanged(){
    ProjectDocument* document = qobject_cast<ProjectDocument*>(sender());
    if ( document ){
        if ( !document->isDirty() && document->path() == m_path ){
            init(document->content());
        }
    }
}

}// namespace
