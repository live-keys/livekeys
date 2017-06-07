#ifndef QSTATICLOADERPROPERTY_H
#define QSTATICLOADERPROPERTY_H

#include <QObject>
#include <QVariant>

namespace lcv{

class QStaticLoaderProperty : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)


public:
    explicit QStaticLoaderProperty(QObject *parent = 0);
    ~QStaticLoaderProperty();

    const QVariant& value() const;
    void setValue(const QVariant& value);

signals:
    void valueChanged();

private:
    QVariant m_value;

};

inline const QVariant &QStaticLoaderProperty::value() const{
    return m_value;
}

}// namespace

#endif // QSTATICLOADERPROPERTY_H
