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

#include "visuallogmodel.h"
#include "live/visuallog.h"
#include "live/applicationcontext.h"
#include <QFile>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QThread>
#include <QColor>


/**
 * \class lv::VisualLogModel
 * \brief Main model used in visualizing log entries within LiveKeys when the log window is opened
 *
 * Receives any type message, whether if it's a string (or a string-displayable object) or an image.
 * \ingroup lvview
 */

namespace lv{

/** Default constructor */
VisualLogModel::VisualLogModel(QQmlEngine *engine)
    : VisualLogBaseModel(engine)
    , m_engine(engine)
    , m_textComponent(new QQmlComponent(m_engine))
{
    m_textStyle.infoColor  = "#eee";
    m_textStyle.warnColor  = "#fcd612";
    m_textStyle.errorColor = "#ff4e4e";
    m_textStyle.font       = "Source Code Pro, Ubuntu Mono, Courier New, Courier";
    m_textStyle.fontSize   = 12;

    resetTextComponent();
}


/** Blank destructor */
VisualLogModel::~VisualLogModel(){
}

/** Implementation of the respective QAbstractListModel function */
QVariant VisualLogModel::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_entries.size() )
        return QVariant();

    if ( role == VisualLogModel::Msg ){
        return entryDataAt(index.row());
    } else if ( role == VisualLogModel::Prefix ) {
        return m_entries[index.row()].prefix;
    } else if ( role == VisualLogModel::Location )
        return m_entries[index.row()].location;
    return QVariant();
}

/**
 * \brief Implementation of the respective function from VisualLog::ViewTransport
 *
 * Appends the given string message to the list of log entries.
 */
void VisualLogModel::onMessage(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const std::string &message)
{
    QString location;
    if ( !messageInfo.sourceFileName().empty() ){
        location = QString::fromStdString(messageInfo.sourceFileName()) + ":" + QString::number(messageInfo.sourceLineNumber());
        if ( location.startsWith("memory:/") ){
            location = location.mid(8);
        }
    }
    if ( thread() == QThread::currentThread() ){
        beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
        m_entries.append( VisualLogEntry(
            QString::fromStdString(messageInfo.tag(configuration)),
            messageInfo.level(),
            QString::fromStdString(messageInfo.prefix(configuration)),
            location,
            QString::fromStdString(message)
        ));
        endInsertRows();
    } else {
        m_entries.append( VisualLogEntry(
            QString::fromStdString(messageInfo.tag(configuration)),
            messageInfo.level(),
            QString::fromStdString(messageInfo.prefix(configuration)),
            location,
            QString::fromStdString(message)
        ));
    }
}

/**
 * \brief Implementation of the respective function from VisualLog::ViewTransport
 *
 * Appends the log view given via name to the entries
 */
void VisualLogModel::onView(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const std::string &viewName,
        VisualLog::ViewObject *vo)
{
    QQmlComponent* comp = component(QString::fromStdString(viewName));
    if ( comp ){
        QString location;
        if ( !messageInfo.sourceFileName().empty() ){
            location = QString::fromStdString(messageInfo.sourceFileName()) + ":" + QString::number(messageInfo.sourceLineNumber());
            if ( location.startsWith("memory:/") ){
                location = location.mid(8);
            }
        }

        QObject* ob = dynamic_cast<QObject*>(vo);
        if ( ob ){
            beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
            m_entries.append(VisualLogEntry(
                QString::fromStdString(messageInfo.tag(configuration)),
                messageInfo.level(),
                QString::fromStdString(messageInfo.prefix(configuration)),
                location,
                new QVariant(QVariant::fromValue(ob)),
                comp
            ));
            endInsertRows();
        }
    }
}

/** Implementation of the respective VisualLogBaseModel function */
QVariant VisualLogModel::entryDataAt(int index) const{
    const VisualLogEntry& entry = m_entries[index];
    if ( entry.component == 0 ){
        if ( entry.context == 0 ){
            entry.context = new QQmlContext(m_engine, (QObject*)this);
            entry.context->setContextProperty("modelData", entry.data);
            entry.context->setContextProperty("modelParent", (QObject*)this);
        }

        QObject* ob = m_textComponent->create(entry.context);
        if ( entry.level == VisualLog::MessageInfo::Error ){
            ob->setProperty("color", m_textStyle.errorColor);
        } else if ( entry.level == VisualLog::MessageInfo::Warning ){
            ob->setProperty("color", m_textStyle.warnColor);
        }
        return QVariant::fromValue(ob);
    } else {
        if ( entry.context == 0 ){
            entry.context = new QQmlContext(m_engine, (QObject*)this);
            entry.context->setContextProperty("modelData", *entry.objectData);
            entry.context->setContextProperty("modelParent", (QObject*)this);
        }

        QObject* ob = entry.component->create(entry.context);
        if ( entry.level == VisualLog::MessageInfo::Error ){
            ob->setProperty("color", m_textStyle.errorColor);
        } else if ( entry.level == VisualLog::MessageInfo::Warning ){
            ob->setProperty("color", m_textStyle.warnColor);
        }
        return QVariant::fromValue(ob);
    }
}

/** Implementation of the respective VisualLogBaseModel function */
QString VisualLogModel::entryPrefixAt(int index) const{
    return m_entries[index].prefix;
}

/** Implementation of the respective VisualLogBaseModel function */
const VisualLogEntry &VisualLogModel::entryAt(int index) const{
    return m_entries.at(index);
}

/** Sets the style of this log model */
void VisualLogModel::setStyle(QJSValue style){
    if ( style.hasProperty("infoColor") ){
        m_textStyle.infoColor = QColor(style.property("infoColor").toString());
    }
    if ( style.hasProperty("warnColor") ){
        m_textStyle.warnColor = QColor(style.property("warnColor").toString());
    }
    if ( style.hasProperty("errorColor") ){
        m_textStyle.errorColor = QColor(style.property("errorColor").toString());
    }
    if ( style.hasProperty("font") ){
        m_textStyle.font = style.property("font").toString().toUtf8();
    }
    if ( style.hasProperty("fontSize") ){
        m_textStyle.fontSize = style.property("fontSize").toInt();
    }

    resetTextComponent();
    emit styleChanged();
}

/** Returns the style of this log model */
QJSValue VisualLogModel::style() const{
    QJSValue style = m_engine->newObject();
    style.setProperty("infoColor",  m_textStyle.infoColor.name());
    style.setProperty("warnColor",  m_textStyle.warnColor.name());
    style.setProperty("errorColor", m_textStyle.errorColor.name());
    style.setProperty("font",       QString::fromUtf8(m_textStyle.font));
    style.setProperty("fontSize",   m_textStyle.fontSize);
    return style;
}

void VisualLogModel::resetTextComponent(){
    m_textComponent->setData(
        "import QtQuick 2.3\n\n"
        "Text{ "
            "y: 1; "
            "text: modelData; "
            "width: modelParent.width; "
            "color: '" + m_textStyle.infoColor.name().toUtf8() + "'; "
            "wrapMode: Text.Wrap; "
            "font.family: '" + m_textStyle.font + "';"
            "font.pixelSize: " + QByteArray::number(m_textStyle.fontSize) + "; "
        "}\n",
        QUrl("TextDelegate.qml")
    );
}

QString VisualLogModel::messageAt(int index) const{
    if ( index >= m_entries.size() )
        return QString();

    return m_entries.at(index).data;
}

QString VisualLogModel::locationAt(int index) const{
    if ( index >= m_entries.size() )
        return QString();

    return m_entries.at(index).location;
}

/** Erases all log entries from the model */
void VisualLogModel::clearValues(){
    beginResetModel();
    m_entries.clear();
    endResetModel();
}

QQmlComponent *VisualLogModel::component(const QString &key){
    QHash<QString, QQmlComponent*>::iterator it = m_components.find(key);
    if ( it != m_components.end() )
        return it.value();

    QString fullPath = componentPath(key);

    QFile fileInput(fullPath);
    if ( !fileInput.exists() && !fullPath.endsWith(".qml") )
        fileInput.setFileName(fullPath + ".qml");

    if ( !fileInput.open(QIODevice::ReadOnly) ){
        qCritical("Cannot open file: %s", qPrintable(fullPath));
        return nullptr;
    }

    QQmlComponent* component = new QQmlComponent(m_engine);
    component->setData(fileInput.readAll(), QUrl::fromLocalFile(fullPath));
    fileInput.close();

    if ( component->errors().size() > 0 ){
        qCritical("Failed to load component \'%s\': %s", qPrintable(key), qPrintable(component->errorString()));
        delete component;
        return nullptr;
    }

    m_components.insert(key, component);

    return component;
}

QString VisualLogModel::componentPath(const QString &componentKey){
    QString ck = componentKey + ".qml";
    ck = ck.replace('@', '/');
    return QString::fromStdString(ApplicationContext::instance().pluginPath()) + "/" + ck;
}

}// namespace
