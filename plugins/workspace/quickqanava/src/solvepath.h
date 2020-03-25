#ifndef SOLVEPATH_H
#define SOLVEPATH_H

#include <QString>
#include "live/applicationcontext.h"

inline QString solveResourcePath(const QString& path){
    QString modifiedUrl = path;
    if ( modifiedUrl.startsWith("qrc:/QuickQanava"))
        modifiedUrl =
            QString::fromStdString(lv::ApplicationContext::instance().pluginPath()) +
            "/workspace/quickqanava" + modifiedUrl.mid(QString("qrc:/QuickQanava").length());
    return modifiedUrl;
}

#endif // SOLVEPATH_H
