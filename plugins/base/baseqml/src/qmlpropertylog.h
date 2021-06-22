#ifndef LVQMLPROPERTYLOG_H
#define LVQMLPROPERTYLOG_H

#include <QObject>
#include <QJSValue>

namespace lv{

class QmlPropertyLog : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue input READ input WRITE setInput NOTIFY inputChanged)

public:
    explicit QmlPropertyLog(QObject *parent = nullptr);
    ~QmlPropertyLog();

    QJSValue input() const;
    void setInput(QJSValue input);

signals:
    void inputChanged(QJSValue input);
};

inline QJSValue QmlPropertyLog::input() const{
    return QJSValue();
}

}// namespace

#endif // LVQMLPROPERTYLOG_H
