#ifndef LVGROUPCOLLECTORENTRY_H
#define LVGROUPCOLLECTORENTRY_H

#include <QObject>

namespace lv{

class GroupCollector;

/// \private
class GroupCollectorEntry : public QObject{

    Q_OBJECT

public:
    explicit GroupCollectorEntry(const QString& property, GroupCollector *parent = nullptr);
    ~GroupCollectorEntry();

    GroupCollector* groupCollector();

    void resetChange();

    const QString& propertyName() const;
    bool changed() const;

public slots:
    void propertyChanged();

private:
    QString m_propertyName;
    bool    m_changed;

};

inline void GroupCollectorEntry::resetChange(){
    m_changed = false;
}

inline const QString &GroupCollectorEntry::propertyName() const{
    return m_propertyName;
}

inline bool GroupCollectorEntry::changed() const{
    return m_changed;
}

}// namespace

#endif // LVGROUPCOLLECTORENTRY_H
