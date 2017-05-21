#include "qlibraryloadpath.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>

#define QLIBRARY_LOAD_PATH_DEBUG_FLAG
#ifdef QLIBRARY_LOAD_PATH_DEBUG_FLAG
#define QLIBRARY_LOAD_PATH_DEBUG(_param) qDebug() << "LIBRARY PATH:" << (_param)
#else
#define QLIBRARY_LOAD_PATH_DEBUG(_param)
#endif

namespace lcv{

namespace{

#if defined(Q_OS_UNIX)

bool assertLinkPathExists(const QString& linkPath){
    if ( !QDir(linkPath).exists() )
        return QDir().mkdir(linkPath);
    return true;
}

void addPath(const QString& path, const QString& linkPath, bool recursive = false){
    if ( !assertLinkPathExists(linkPath) ){
        qCritical("Failed to create link directory. Some library dependencies may fail to load.");
        return;
    }

    QDirIterator dit(path);
    while ( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        if (  (info.isFile() || info.isSymLink()) &&
               info.fileName().startsWith("lib") &&
               info.fileName().contains(".so") )
        {
            QFile f(dit.filePath());
            f.link(linkPath + "/" + info.fileName());

            QLIBRARY_LOAD_PATH_DEBUG("Added \'" + linkPath + "/" + info.fileName() + "\' -> \'" + f.fileName() + "\'");

        } else if ( info.isDir() && recursive ){
            addPath(info.filePath(), linkPath, recursive);
        }
    }
}

#elif defined(Q_OS_WIN)



#endif


}

void QLibraryLoadPath::add(const QString &path, const QString &linkPath){
    addPath(path, linkPath);
}

void QLibraryLoadPath::addRecursive(const QString &path, const QString &linkPath){
    addPath(path, linkPath, true);
}

}// namespace
