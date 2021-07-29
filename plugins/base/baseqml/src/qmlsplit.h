#ifndef LVQMLSPLIT_H
#define LVQMLSPLIT_H

#include <QObject>
#include <QJSValue>

namespace lv{

class QmlSplit : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue value     READ value     WRITE setValue     NOTIFY valueChanged)
    Q_PROPERTY(QObject* collector READ collector WRITE setCollector NOTIFY collectorChanged)

public:
    explicit QmlSplit(QObject *parent = nullptr);

    QJSValue value() const;
    void setValue(QJSValue value);

    QObject* collector() const;
    void setCollector(QObject* collector);

signals:
    void valueChanged();
    void collectorChanged();

private:
    QJSValue m_value;
    QObject* m_collector;
};

inline QJSValue QmlSplit::value() const{
    return m_value;
}

inline QObject *QmlSplit::collector() const{
    return m_collector;
}

inline void QmlSplit::setCollector(QObject *collector){
    m_collector = collector;
    emit collectorChanged();
}

}// namespace

#endif // LVQMLSPLIT_H
