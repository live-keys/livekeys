#ifndef MAPARRAY_H
#define MAPARRAY_H

#include <QObject>
#include <QQmlComponent>

namespace lv{

class MapArrayData;

/// \private
class MapArray : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariantList input  READ input  WRITE setInput  NOTIFY inputChanged)
    Q_PROPERTY(QQmlComponent* f    READ f      WRITE setF      NOTIFY fChanged)
    Q_PROPERTY(QVariantList output READ output NOTIFY outputChanged)

public:
    explicit MapArray(QObject *parent = nullptr);
    ~MapArray();

    void setInput(const QVariantList& input);
    void setF(QQmlComponent* f);

    const QVariantList& input() const;
    const QVariantList& output() const;
    QQmlComponent* f() const;

    void process();

    void assignResult(MapArrayData* mad, const QVariant& v);

    void clearCurrent();

signals:
    void inputChanged();
    void fChanged();
    void outputChanged();

private:
    QVariantList   m_input;
    QVariantList   m_output;

    QList<QObject*>      m_fObjects;
    QList<QQmlContext*>  m_fContexts;
    QList<MapArrayData*> m_fData;

    QQmlComponent* m_f;
};

inline const QVariantList &MapArray::input() const{
    return m_input;
}

inline QQmlComponent *MapArray::f() const{
    return m_f;
}

inline void MapArray::setInput(const QVariantList &input){
    if (m_input == input)
        return;

    process();
    m_input = input;
    emit inputChanged();
}

inline void MapArray::setF(QQmlComponent *f){
    if (m_f == f)
        return;

    process();
    m_f = f;
    emit fChanged();
}

inline const QVariantList &MapArray::output() const{
    return m_output;
}

}// namespace

#endif // MAPARRAY_H
