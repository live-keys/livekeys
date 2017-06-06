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

#include "qlicensecontainer.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QQuickItem>
#include <QQmlContext>
#include <QVariant>

namespace lcv{

QLicenseContainer::QLicenseContainer(QObject *parent)
    : QAbstractListModel(parent)
    , m_isDirty(false)
    , m_highlights(0)
{
    m_roles[QLicenseContainer::Id]        = "id";
    m_roles[QLicenseContainer::Alias]     = "alias";
    m_roles[QLicenseContainer::Text]      = "text";
    m_roles[QLicenseContainer::Valid]     = "valid";
    m_roles[QLicenseContainer::Highlight] = "highlight";
}

QLicenseContainer::~QLicenseContainer(){
}

int QLicenseContainer::rowCount(const QModelIndex &) const{
    return m_licenses.size();
}

QVariant QLicenseContainer::data(const QModelIndex &index, int role) const{
    if ( !index.isValid() )
        return QVariant();

    int row = index.row();
    QHash<QString, QLiveCVLicense>::ConstIterator it = m_licenses.begin() + row;

    if ( role == QLicenseContainer::Id ){
        return it.key();
    } else if ( role == QLicenseContainer::Alias ){
        return it.value().alias;
    } else if ( role == QLicenseContainer::Text ){
        return it.value().text;
    } else if ( role == QLicenseContainer::Valid ){
        return it.value().valid;
    } else if ( role == QLicenseContainer::Highlight ){
        return it.value().highlight;
    }
    return QVariant();
}

bool QLicenseContainer::require(const QString &id, const QString &alias, const QString &text){
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
                (QVector<int>() << QLicenseContainer::Highlight << QLicenseContainer::Valid)
            );
            m_isDirty = true;
        }
        return false;
    }
}

void QLicenseContainer::fromJson(const QJsonArray& root){
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

QJsonArray QLicenseContainer::toJson() const{
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

QLicenseContainer *QLicenseContainer::grabFromContext(
        QQuickItem *item,
        const QString &settingsProperty,
        const QString &contextProperty)
{
    QQmlContext* context = qmlContext(item);
    if ( !context )
        return 0;
    QObject* settings = context->contextProperty(settingsProperty).value<QObject*>();
    if ( !settings )
        return 0;
    return static_cast<QLicenseContainer*>(settings->property(contextProperty.toUtf8()).value<QObject*>());
}

void QLicenseContainer::acceptLicense(const QString &id){
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
                (QVector<int>() << QLicenseContainer::Highlight << QLicenseContainer::Valid)
            );
            emit licenseAccepted(id);
            m_isDirty = true;
        }
    }
}

void QLicenseContainer::addLicense(
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

bool QLicenseContainer::hasLicense(const QString &id){
    return m_licenses.contains(id);
}

QString QLicenseContainer::licenseText(const QString &id){
    if ( hasLicense(id) )
        return m_licenses[id].text;
    return QString();
}

}// namespace
