#ifndef QLIBRARYLOADPATH_H
#define QLIBRARYLOADPATH_H

#include <QString>

namespace lcv{

class QLibraryLoadPath{

public:
    static void add(const QString& path, const QString& linkPath = "");
    static void addRecursive(const QString& path, const QString& linkPath = "");

};

}// namespace

#endif // QLIBRARYLOADPATH_H
