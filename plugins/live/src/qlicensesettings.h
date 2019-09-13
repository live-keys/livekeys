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

#ifndef QLICENSESETTINGS_H
#define QLICENSESETTINGS_H

#include <QObject>
#include <QHash>
#include <QAbstractListModel>
#include "live/lvliveglobal.h"

class QQuickItem;

/// \private
class LV_LIVE_EXPORT QLicenseEntry{
public:
    QString id;
    QString alias;
    QString text;
    bool    valid;
    bool    highlight;
};

/// \private
class LV_LIVE_EXPORT QLicenseSettings : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int highlights READ highlights NOTIFY highlightsChanged)
    enum Roles{
        Id = Qt::UserRole + 1,
        Alias,
        Text,
        Valid,
        Highlight
    };

public:
    explicit QLicenseSettings(const QString& settingsPath, QObject *parent = 0);
    ~QLicenseSettings();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    bool require(const QString& id, const QString& alias, const QString& text);

    void reparse();
    void save();

    void fromJson(const QJsonArray& root);
    QJsonArray toJson() const;

    int highlights() const;

    bool isDirty() const;

    static QLicenseSettings* grabFromContext(
        QObject* item,
        const QString& settingsProperty = "settings",
        const QString& contextProperty = "license"
    );

    bool isLicenseValid(const QString& id);

signals:
    void licenseAccepted(const QString& id);
    void highlightsChanged();

public slots:
    void acceptLicense(const QString& id);
    void addLicense(
        const QString& id,
        const QString& alias,
        const QString& text,
        bool valid = false,
        bool highlight = false
    );
    bool hasLicense(const QString& id);
    QString licenseText(const QString& id);

private:
    QHash<QString, QLicenseEntry> m_licenses;
    QHash<int, QByteArray>         m_roles;
    bool                           m_isDirty;
    int                            m_highlights;

    QString                        m_licenseFile;
    bool                           m_parseError;
    QString                        m_errorText;
};

inline QHash<int, QByteArray> QLicenseSettings::roleNames() const{
    return m_roles;
}

inline int QLicenseSettings::highlights() const{
    return m_highlights;
}

inline bool QLicenseSettings::isDirty() const{
    return m_isDirty;
}

inline bool QLicenseSettings::isLicenseValid(const QString &id){
    QHash<QString, QLicenseEntry>::Iterator it = m_licenses.find(id);
    if ( it == m_licenses.end() )
        return false;
    return it->valid;
}

#endif // QLICENSESETTINGS_H
