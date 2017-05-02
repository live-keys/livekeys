#ifndef QLICENSECONTAINER_H
#define QLICENSECONTAINER_H

#include <QObject>
#include <QHash>
#include <QAbstractListModel>
#include "qliveglobal.h"

class QQuickItem;

namespace lcv{

class Q_LIVE_EXPORT QLiveCVLicense{
public:
    QString id;
    QString alias;
    QString text;
    bool    valid;
    bool    highlight;
};

class Q_LIVE_EXPORT QLicenseContainer : public QAbstractListModel{

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
    explicit QLicenseContainer(QObject *parent = 0);
    ~QLicenseContainer();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    bool require(const QString& id, const QString& alias, const QString& text);

    void fromJson(const QJsonArray& root);
    QJsonArray toJson() const;

    int highlights() const;

    bool isDirty() const;

    static QLicenseContainer* grabFromContext(
        QQuickItem* item,
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
    QHash<QString, QLiveCVLicense> m_licenses;
    QHash<int, QByteArray>         m_roles;
    bool                           m_isDirty;
    int                            m_highlights;
};

inline QHash<int, QByteArray> QLicenseContainer::roleNames() const{
    return m_roles;
}

inline int QLicenseContainer::highlights() const{
    return m_highlights;
}

inline bool QLicenseContainer::isDirty() const{
    return m_isDirty;
}

inline bool QLicenseContainer::isLicenseValid(const QString &id){
    QHash<QString, QLiveCVLicense>::Iterator it = m_licenses.find(id);
    if ( it == m_licenses.end() )
        return false;
    return it->valid;
}

}// namespace

#endif // QLICENSECONTAINER_H
