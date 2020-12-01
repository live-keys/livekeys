#ifndef LVQMLBUILD_H
#define LVQMLBUILD_H

#include "live/lvviewglobal.h"
#include <QObject>

namespace lv{

class Runnable;
class LV_VIEW_EXPORT QmlBuild : public QObject{

    Q_OBJECT

public:
    enum State{
        Compiling,
        Ready,
        Errored,
        Removed
    };

public:
    explicit QmlBuild(Runnable* runnable, QObject *parent = nullptr);
    ~QmlBuild();

    Runnable* runnable();
    void setState(State state);

signals:
    void error();
    void ready();
    void remove();

public slots:

private:
    Runnable* m_runnable;
    State     m_state;
};

inline Runnable *QmlBuild::runnable(){
    return m_runnable;
}

}// namespace

#endif // LVQMLBUILD_H
