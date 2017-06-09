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

#include "qlicensesettings.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>





QLicenseSettings::QLicenseSettings(const QString &settingsPath, QObject *parent)
    : QAbstractListModel(parent)
    , m_isDirty(false)
    , m_highlights(0)
{
    m_licenseFile = QDir::cleanPath(settingsPath + "/licenses.json");

    m_roles[QLicenseSettings::Id]        = "id";
    m_roles[QLicenseSettings::Alias]     = "alias";
    m_roles[QLicenseSettings::Text]      = "text";
    m_roles[QLicenseSettings::Valid]     = "valid";
    m_roles[QLicenseSettings::Highlight] = "highlight";

    reparse();
}

QLicenseSettings::~QLicenseSettings(){
    save();
}

int QLicenseSettings::rowCount(const QModelIndex &) const{
    return m_licenses.size();
}

QVariant QLicenseSettings::data(const QModelIndex &index, int role) const{
    if ( !index.isValid() )
        return QVariant();

    int row = index.row();
    QHash<QString, QLiveCVLicense>::ConstIterator it = m_licenses.begin() + row;

    if ( role == QLicenseSettings::Id ){
        return it.key();
    } else if ( role == QLicenseSettings::Alias ){
        return it.value().alias;
    } else if ( role == QLicenseSettings::Text ){
        return it.value().text;
    } else if ( role == QLicenseSettings::Valid ){
        return it.value().valid;
    } else if ( role == QLicenseSettings::Highlight ){
        return it.value().highlight;
    }
    return QVariant();
}

bool QLicenseSettings::require(const QString &id, const QString &alias, const QString &text){
    QHash<QString, QLiveCVLicense>::Iterator it = m_licenses.find(id);
    if ( it == m_licenses.end() ){
        addLicense(id, alias, text, false, true);
        return false;
    } else {
        if ( it->valid )
            return true;

        if ( !it->highlight ){
            int row = 0;
            for (
                QHash<QString, QLiveCVLicense>::Iterator itcount = m_licenses.begin();
                itcount != it;
                ++itcount, ++row
            );
            it->highlight = true;
            ++m_highlights;
            emit highlightsChanged();
            emit dataChanged(
                createIndex(row, 0),
                createIndex(row, 0),
                (QVector<int>() << QLicenseSettings::Highlight << QLicenseSettings::Valid)
            );
            m_isDirty = true;
        }
        return false;
    }
}

void QLicenseSettings::reparse(){
    QFile file(m_licenseFile);
    if ( file.open(QIODevice::ReadOnly) ){
        QJsonParseError error;
        QJsonDocument jsondoc = QJsonDocument::fromJson(file.readAll(), &error);
        if ( error.error != QJsonParseError::NoError ){
            m_parseError = true;
            m_errorText  = error.errorString();
            qCritical(
                "Failed to parse stored licenses file(config/licenses.json): %s. Licenses will not be saved.",
                qPrintable(m_errorText)
            );
        }
        fromJson(jsondoc.array());
        file.close();
    }
}

void QLicenseSettings::save(){
    qDebug() << m_licenseFile;
    if ( m_parseError || !isDirty() )
        return;

    QFile file(m_licenseFile);
    if ( file.open(QIODevice::WriteOnly) ){
        QJsonArray root = toJson();
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qCritical("Failed to save license settings to to file: \'%s\'. Make sure the "
                  "path exists and you have set the correct access rights.", qPrintable(m_licenseFile));
    }
}

void QLicenseSettings::fromJson(const QJsonArray& root){
    beginResetModel();
    m_licenses.clear();
    m_highlights = 0;
    endResetModel();

    for ( QJsonArray::ConstIterator it = root.begin(); it != root.end(); ++it ){
        QJsonObject ob = it->toObject();
        addLicense(
            ob["id"].toString(),
            ob["alias"].toString(),
            ob["text"].toString(),
            ob["valid"].toBool()
        );
        m_isDirty = false;
    }
}

QJsonArray QLicenseSettings::toJson() const{
    QJsonArray root;
    for( QHash<QString, QLiveCVLicense>::ConstIterator it = m_licenses.begin(); it != m_licenses.end(); ++it ){
        QJsonObject licenseOb;
        licenseOb["id"] = it->id;
        licenseOb["alias"] = it->alias;
        licenseOb["text"] = it->text;
        licenseOb["valid"] = it->valid;
        root.append(licenseOb);
    }
    return root;
}

QLicenseSettings *QLicenseSettings::grabFromContext(QObject *item,
        const QString &settingsProperty,
        const QString &contextProperty)
{
    QQmlEngine* engine = qmlEngine(item);
    if ( !engine )
        return 0;
    QQmlContext* context = engine->rootContext();
    if ( !context )
        return 0;
    QObject* settings = context->contextProperty(settingsProperty).value<QObject*>();
    if ( !settings )
        return 0;
    return static_cast<QLicenseSettings*>(settings->property(contextProperty.toUtf8()).value<QObject*>());
}

void QLicenseSettings::acceptLicense(const QString &id){
    QHash<QString, QLiveCVLicense>::Iterator it = m_licenses.find(id);
    if ( it != m_licenses.end() ){
        if ( it->valid == false ){
            int row = 0;
            for (QHash<QString, QLiveCVLicense>::Iterator itcount = m_licenses.begin(); itcount != it; ++itcount, ++row );
            it->valid     = true;
            if ( it->highlight ){
                it->highlight = false;
                m_highlights--;
                emit highlightsChanged();
            }
            emit dataChanged(
                createIndex(row, 0),
                createIndex(row, 0),
                (QVector<int>() << QLicenseSettings::Highlight << QLicenseSettings::Valid)
            );
            emit licenseAccepted(id);
            m_isDirty = true;
        }
    }
}

void QLicenseSettings::addLicense(
        const QString &id,
        const QString &alias,
        const QString &text,
        bool valid,
        bool highlight)
{
    if ( valid )
        highlight = false;

    beginResetModel();
    QLiveCVLicense license;
    license.id        = id;
    license.alias     = alias;
    license.text      = text;
    license.valid     = valid;
    license.highlight = highlight;
    if ( highlight ){
        ++m_highlights;
        emit highlightsChanged();
    }
    m_licenses[id] = license;
    endResetModel();
    m_isDirty = true;
}

bool QLicenseSettings::hasLicense(const QString &id){
    return m_licenses.contains(id);
}

QString QLicenseSettings::licenseText(const QString &id){
    if ( hasLicense(id) )
        return m_licenses[id].text;
    return QString();
}

