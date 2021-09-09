#ifndef LVQMLDATA_H
#define LVQMLDATA_H

#include <QObject>
#include "live/shared.h"

namespace lv{

class QmlData : public Shared{

    Q_OBJECT

public:
    explicit QmlData(QObject *parent = nullptr);
    ~QmlData() override;

signals:

};

}// namespace

#endif // LVQMLDATA_H
