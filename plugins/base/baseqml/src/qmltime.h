#ifndef LVQMLTIME_H
#define LVQMLTIME_H

#include <QObject>

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
};

}// namespace

#endif // LVQMLTIME_H
