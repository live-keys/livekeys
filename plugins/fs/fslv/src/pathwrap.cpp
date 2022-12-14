#include "pathwrap.h"

#include <QFileInfo>

namespace lv{ namespace el{

PathWrap::PathWrap(Engine* engine)
    : Element(engine)
{
}

PathWrap::~PathWrap(){
}

std::string PathWrap::name(const std::string &name) const{
    return QFileInfo(QString::fromStdString(name)).fileName().toStdString();
}

bool PathWrap::exists(const std::string &path) const{
    return QFileInfo::exists(QString::fromStdString(path));
}


}} // namespace lv, el
