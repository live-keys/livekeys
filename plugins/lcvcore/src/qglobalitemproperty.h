#ifndef QGLOBALITEMPROPERTY_H
#define QGLOBALITEMPROPERTY_H

#include <QQuickItem>

class QGlobalItemProperty : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QGlobalItemProperty(QQuickItem *parent = 0);
    ~QGlobalItemProperty();

    const QVariant& value() const;
    void setValue(const QVariant& value);

signals:
    void valueChanged();

private:
    QVariant m_value;

};

inline const QVariant& QGlobalItemProperty::value() const{
    return m_value;
}

#endif // QGLOBALITEMPROPERTY_H
