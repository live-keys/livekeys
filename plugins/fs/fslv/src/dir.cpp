#include "dir.h"

#include <QFileInfo>
#include <QDir>

namespace lv{ namespace el{

Dir::Dir(Engine* engine)
    : Element(engine)
{
}

Dir::~Dir(){
}

bool Dir::mkDir(const std::string &path){
    return QDir(".").mkdir(QString::fromStdString(path));
}

bool Dir::mkPath(const std::string &path){
    return QDir(".").mkpath(QString::fromStdString(path));
}

bool Dir::remove(const std::string &path){
    return QDir(QString::fromStdString(path)).removeRecursively();
}

bool Dir::rename(const std::string &old, const std::string &nu){
    return QDir(".").rename(QString::fromStdString(old), QString::fromStdString(nu));
}

}} // namespace lv, el
