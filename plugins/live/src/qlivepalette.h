#ifndef QPALLETE_H
#define QPALLETE_H

#include <QObject>
#include <QQuickItem>
#include "qliveglobal.h"
#include "qcodeconverter.h"

namespace lcv{

class Q_LIVE_EXPORT QLivePalette : public QCodeConverter{

    Q_OBJECT
    Q_PROPERTY(QQuickItem* item READ item  WRITE setItem  NOTIFY itemChanged)
//    Q_PROPERTY(QVariant value   READ value NOTIFY valueChanged)

public:
    explicit QLivePalette(QObject *parent = 0);
    virtual ~QLivePalette();

    QQuickItem* item();
    void setItem(QQuickItem* item);

signals:
    void itemChanged();

public slots:
    void initPallete(const QString& code);

private:
    Q_DISABLE_COPY(QLivePalette)

    QQuickItem* m_item;
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

}// namespace

#endif // QPALLETE_H
