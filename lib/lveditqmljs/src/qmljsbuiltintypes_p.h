#ifndef QMLJSBUILTINTYPES_H
#define QMLJSBUILTINTYPES_H

#include <QString>

namespace lv{

class QmlJsBuiltinTypes{

public:
    QmlJsBuiltinTypes();

    static QString nameFromCpp(const QString& type);

};

}// namespaces

#endif // QMLJSBUILTINTYPES_H
