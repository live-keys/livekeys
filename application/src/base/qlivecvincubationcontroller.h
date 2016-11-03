#ifndef QLIVECVINCUBATIONCONTROLLER_H
#define QLIVECVINCUBATIONCONTROLLER_H

#include <QObject>
#include <QQmlIncubationController>

class QLiveCVIncubationController : public QObject, public QQmlIncubationController{

    Q_OBJECT

public:
    QLiveCVIncubationController(QObject* parent = 0);
    ~QLiveCVIncubationController();

protected:
    virtual void timerEvent(QTimerEvent*);
};

#endif // QLIVECVINCUBATIONCONTROLLER_H
