#ifndef LVQMLOPENING_H
#define LVQMLOPENING_H

#include "act.h"

namespace lv{

/// \private
class QmlOpening : public Act{

    Q_OBJECT

public:
    QmlOpening(QObject* parent = nullptr);

protected:
    void componentComplete() override;
};

} // namespace

#endif // LVQMLOPENING_H
