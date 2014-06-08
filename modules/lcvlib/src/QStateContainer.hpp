#ifndef QSTATECONTAINER_HPP
#define QSTATECONTAINER_HPP

#include "QLCVGlobal.hpp"
#include <QMap>

class Q_LCV_EXPORT QStateContainerBase{

public:
    QStateContainerBase(){}
    virtual ~QStateContainerBase(){}

};

class Q_LCV_EXPORT QStateContainerManager{

public:
    static QStateContainerManager& instance(); // already defined

private:
    static void cleanStateManager();

    QStateContainerManager();
    ~QStateContainerManager();

    // disable copy
    QStateContainerManager(QStateContainerManager const&);
    QStateContainerManager& operator=(QStateContainerManager const&);

    static QStateContainerManager* m_instance;
};


template<typename T>
class Q_LCV_EXPORT QStateContainer : public QStateContainerBase{

public:
    void RegisterState(const QString& key, T* state);
    T*   State(const QString& key);

private:
    QStateContainer();
    ~QStateContainer();

    QMap<QString, T*> m_states;

};

#endif // QSTATECONTAINER_HPP
