#ifndef LVMEMORY_H
#define LVMEMORY_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include <QObject>

namespace lv{

class LV_VIEW_EXPORT Memory : public QObject{

    Q_OBJECT

public:
    explicit Memory(QObject *parent = nullptr);
    ~Memory();

public slots:
    Shared* reloc(Shared* sd);
};

}// namespace

#endif // LVMEMORY_H
