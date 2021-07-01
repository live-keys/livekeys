#ifndef LVQMLTIME_H
#define LVQMLTIME_H

#include <QObject>
#include <QJSValue>

namespace lv{

class QmlTime : public QObject{

    Q_OBJECT

public:
    explicit QmlTime(QObject *parent = nullptr);
    ~QmlTime();

public slots:
    void sleep(int seconds);
    void msleep(int mseconds);
    void usleep(int useconds);
    void delay(int mseconds, QJSValue callback);
};

}// namespace

#endif // LVQMLTIME_H
