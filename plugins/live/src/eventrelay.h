#ifndef LVEVENTRELAY_H
#define LVEVENTRELAY_H

#include <QObject>

namespace lv{

class EventRelay : public QObject{

    Q_OBJECT

public:
    explicit EventRelay(QObject *parent = nullptr);

signals:

public slots:
    void relayKeyEvent(QObject* object, int key, int modifiers);

};

}// namespace

#endif // LVEVENTRELAY_H
