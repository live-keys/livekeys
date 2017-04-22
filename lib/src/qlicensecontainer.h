#ifndef QLICENSECONTAINER_H
#define QLICENSECONTAINER_H

#include <QObject>
#include <QHash>
#include "qliveglobal.h"

namespace lcv{

class Q_LIVE_EXPORT QLiveCVLicense{
public:
    QString id;
    QString type;
    QString file;
    bool    valid;
};

class Q_LIVE_EXPORT QLicenseContainer : public QObject{

    Q_OBJECT

public:
    explicit QLicenseContainer(QObject *parent = 0);

signals:
    void assertLicense();

public slots:
    void addLicense(const QString& id, const QString& type, const QString& file);
    bool hasLicense(const QString& id);

private:
    QHash<QString, QLiveCVLicense> m_licenses;
    QString                        m_licenseFile;
};

}// namespace

#endif // QLICENSECONTAINER_H
