#ifndef LVMAPARRAYDATA_H
#define LVMAPARRAYDATA_H

#include <QObject>
#include <QVariant>

namespace lv{

class MapArray;
class MapArrayData : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariant current READ current NOTIFY currentChanged)

public:
    MapArrayData(MapArray* parent = nullptr);

    void setCurrent(int index, const QVariant& value);

    const QVariant &current() const;
    MapArray* mapArray();

public slots:
    int index() const;
    void result(const QVariant& value);

signals:
    void currentChanged();

private:
    QVariant m_current;
    int      m_index;
};

inline const QVariant& MapArrayData::current() const{
    return m_current;
}

}// namespace

#endif // LVMAPARRAYDATA_H
