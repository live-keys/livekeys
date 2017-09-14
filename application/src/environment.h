#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QObject>
#include <QVariantMap>

namespace lcv{

class Environment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariantMap os READ os CONSTANT)

public:
    explicit Environment(QObject *parent = 0);
    ~Environment();

    QVariantMap os() const;

signals:

public slots:

private:
    QVariantMap m_os;
};


inline QVariantMap Environment::os() const{
    return m_os;
}

}// namespace

#endif // ENVIRONMENT_H
