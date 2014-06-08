#ifndef QSTATECONTAINER_HPP
#define QSTATECONTAINER_HPP

#include "QLCVGlobal.hpp"
#include <QMap>

class Q_LCV_EXPORT QStateContainerBase{

public:
    QStateContainerBase();
    ~QStateContainerBase();

};

template<typename T>
class Q_LCV_EXPORT QStateContainer : public QStateContainerBase{

public:
    void RegisterState(const QString& key, T* state);
    T*   State(const QString& key);

private:
    QStateContainer();

    QMap<QString, T*> m_states;


};

#endif // QSTATECONTAINER_HPP
