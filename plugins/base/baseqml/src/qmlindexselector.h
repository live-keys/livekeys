#ifndef QMLINDEXSELECTOR_H
#define QMLINDEXSELECTOR_H

#include <QObject>
#include <QJSValue>

namespace lv{

class QmlIndexSelector : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue list    READ list     WRITE setList   NOTIFY listChanged)
    Q_PROPERTY(int index        READ index    WRITE setIndex  NOTIFY indexChanged)
    Q_PROPERTY(QJSValue current READ current  NOTIFY currentChanged)

public:
    explicit QmlIndexSelector(QObject *parent = nullptr);

    QJSValue list() const;
    void setList(QJSValue list);

    int index() const;
    void setIndex(int index);

    QJSValue current() const;

signals:
    void listChanged();
    void indexChanged();
    void currentChanged();

private:
    QJSValue m_list;
    int      m_index;
    QJSValue m_current;
};

inline QJSValue QmlIndexSelector::list() const{
    return m_list;
}

inline int QmlIndexSelector::index() const{
    return m_index;
}

inline QJSValue QmlIndexSelector::current() const{
    return m_current;
}

}// namespace

#endif // QMLINDEXSELECTOR_H
