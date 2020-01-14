#ifndef LVQMLOPENING_H
#define LVQMLOPENING_H

#include "qmlact.h"

namespace lv{

/// \private
class QmlOpening : public QmlAct{

    Q_OBJECT

public:
    QmlOpening(QObject* parent = nullptr);

protected:
    void componentComplete() override;
};

} // namespace

#endif // LVQMLOPENING_H
