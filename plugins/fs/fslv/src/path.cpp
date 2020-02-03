#include "path.h"

#include <QFileInfo>

namespace lv{ namespace el{

Path::Path(Engine* engine)
    : Element(engine)
{
}

Path::~Path(){
}

std::string Path::name(const std::string &name) const{
    return QFileInfo(QString::fromStdString(name)).fileName().toStdString();
}

bool Path::exists(const std::string &path) const{
    return QFileInfo::exists(QString::fromStdString(path));
}


}} // namespace lv, el
