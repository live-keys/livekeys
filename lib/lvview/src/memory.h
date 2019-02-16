#ifndef LVMEMORY_H
#define LVMEMORY_H

#include <QObject>
#include "live/shared.h"

namespace lv{

class Memory : public QObject{

    Q_OBJECT

public:
    explicit Memory(QObject *parent = nullptr);
    ~Memory();

public slots:
    Shared* reloc(Shared* sd);
};

}// namespace

#endif // LVMEMORY_H
