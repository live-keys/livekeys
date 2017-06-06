#ifndef QLIVEPALLETE_H
#define QLIVEPALLETE_H

#include <QObject>
#include <QQuickItem>
#include "qlcveditorglobal.h"
#include "qcodeconverter.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QLivePalette : public QCodeConverter{

    Q_OBJECT
    Q_PROPERTY(QQuickItem* item READ item  WRITE setItem  NOTIFY itemChanged)
    Q_PROPERTY(QVariant value   READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QLivePalette(QObject *parent = 0);
    virtual ~QLivePalette();

    QQuickItem* item();
    void setItem(QQuickItem* item);

    const QVariant &value() const;
    void setValue(const QVariant& value);

    void initPallete(const QVariant &value);
    void setValueFromCode(const QVariant& value);

signals:
    void itemChanged();
    void valueChanged();

    void init(const QVariant& value);
    void codeChanged(const QVariant& value);

private:
    Q_DISABLE_COPY(QLivePalette)

    bool        m_codeChange;
    QQuickItem* m_item;
    QVariant    m_value;
};

inline QQuickItem *QLivePalette::item(){
    return m_item;
}

inline void QLivePalette::setItem(QQuickItem *item){
    if (m_item == item)
        return;

    m_item = item;
    emit itemChanged();
}

inline const QVariant& QLivePalette::value() const{
    return m_value;
}

inline void QLivePalette::setValue(const QVariant &value){
    if ( m_value != value && !m_codeChange ){
        m_value = value;
        emit valueChanged();
    }
}

}// namespace

#endif // QLIVEPALLETE_H
