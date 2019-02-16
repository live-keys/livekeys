#ifndef LVGROUPCOLLECTOR_H
#define LVGROUPCOLLECTOR_H

#include <QObject>
#include <QQmlComponent>
#include "group.h"
#include "groupcollectorentry.h"

namespace lv{

class GroupCollector : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlComponent* source READ source  WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Group* current      READ current NOTIFY currentChanged)
    Q_PROPERTY(Group* output       READ output  NOTIFY outputChanged)
    Q_CLASSINFO("DefaultProperty", "source")

public:
    GroupCollector(QObject* parent = nullptr);
    virtual ~GroupCollector();

    QQmlComponent* source() const;
    void setSource(QQmlComponent* source);

    Group* current() const;
    Group* output() const;

    virtual void componentComplete();
    virtual void classBegin(){}

    void groupPropertyChanged(GroupCollectorEntry* entry);

public slots:
    void reset();
    void release();

signals:
    void complete();
    void sourceChanged();
    void currentChanged();
    void outputChanged();

private:
    void clearEntries();

    bool           m_isComponentComplete;

    QQmlComponent* m_source;
    Group*         m_current;
    Group*         m_output;

    QList<GroupCollectorEntry*> m_properties;
};

inline QQmlComponent *GroupCollector::source() const{
    return m_source;
}

inline Group *GroupCollector::current() const{
    return m_current;
}

inline Group *GroupCollector::output() const{
    return m_output;
}

inline void GroupCollector::setSource(QQmlComponent *source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();

    reset();
}

}// namespace

#endif // GROUPCOLLECTOR_H
