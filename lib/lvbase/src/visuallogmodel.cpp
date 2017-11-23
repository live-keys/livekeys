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

#include "visuallogmodel.h"
#include "visuallog.h"
#include <QFile>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>

#include <QDebug>

namespace lv{

VisualLogModel::VisualLogModel(QQmlEngine *engine)
    : QAbstractListModel(engine)
    , m_engine(engine)
    , m_textComponent(new QQmlComponent(m_engine))
{
    m_textComponent->setData(
        "import QtQuick 2.7\n\n"
        "Text{ "
            "y: 1; "
            "text: modelData; "
            "width: modelParent.width; "
            "color: '#eee'; "
            "wrapMode: Text.Wrap; "
            "font.family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier';"
            "font.pixelSize: 12; "
        "}\n",
        QUrl("TextDelegate.qml")
    );
}

VisualLogModel::~VisualLogModel(){
}

QVariant VisualLogModel::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_entries.size() )
        return QVariant();

    const VisualLogEntry& entry = m_entries[index.row()];
    if ( role == Qt::UserRole ){
        if ( entry.component == 0 ){
            if ( entry.context == 0 ){
                entry.context = new QQmlContext(m_engine, (QObject*)this);
                entry.context->setContextProperty("modelData", entry.data);
                entry.context->setContextProperty("modelParent", (QObject*)this);
            }

            QObject* ob = m_textComponent->create(entry.context);
            ob->setProperty("y", 5);
            return QVariant::fromValue(ob);
        } else {
            if ( entry.context == 0 ){
                entry.context = new QQmlContext(m_engine, (QObject*)this);
                entry.context->setContextProperty("modelData", *entry.objectData);
                entry.context->setContextProperty("modelParent", (QObject*)this);
            }

            QObject* ob = entry.component->create(entry.context);
            ob->setProperty("y", 5);
            return QVariant::fromValue(ob);
        }

    }
    return QVariant();
}

QHash<int, QByteArray> VisualLogModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "msg";
    return roles;
}

const VisualLogEntry &VisualLogModel::entryAt(int index){
    return m_entries.at(index);
}

void VisualLogModel::appendMessage(const QString& tag, const QString& prefix, const QString &message){
    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries.append(VisualLogEntry(tag, prefix, message));
    endInsertRows();
}

void VisualLogModel::appendView(
        const QString &tag,
        const QString &prefix,
        const QString &viewName,
        const QVariant &value)
{
    QQmlComponent* comp = component(viewName);
    if ( comp ){
        beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
        m_entries.append(VisualLogEntry(tag, prefix, new QVariant(value), comp));
        endInsertRows();
    }
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
        return 0;
    }

    QQmlComponent* component = new QQmlComponent(m_engine);
    component->setData(fileInput.readAll(), QUrl::fromLocalFile(fullPath));
    fileInput.close();

    if ( component->errors().size() > 0 ){
        qCritical("Failed to load component \'%s\': %s", qPrintable(key), qPrintable(component->errorString()));
        delete component;
        return 0;
    }

    m_components.insert(key, component);

    return component;
}

QString VisualLogModel::componentPath(const QString &componentKey){
    return QCoreApplication::applicationDirPath() + "/" + componentKey;
}

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, const QString &p):
    prefix(pprefix),
    tag(ptag),
    data(p),
    objectData(0),
    component(0),
    context(0)
{
}

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, QVariant *od, QQmlComponent *c)
    : tag(ptag)
    , prefix(pprefix)
    , objectData(od)
    , component(c)
    , context(0)
{
}

}// namespace
