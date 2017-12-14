#include "visuallogbasemodel.h"

namespace lv{

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, const QString &p)
    : prefix(pprefix)
    , tag(ptag)
    , data(p)
    , objectData(0)
    , component(0)
    , context(0)
{
}

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, QVariant *od, QQmlComponent *c)
    : prefix(pprefix)
    , tag(ptag)
    , objectData(od)
    , component(c)
    , context(0)
{
}

VisualLogBaseModel::VisualLogBaseModel(QObject *parent)
    : QAbstractListModel(parent){

}

VisualLogBaseModel::~VisualLogBaseModel(){
}

QHash<int, QByteArray> VisualLogBaseModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[VisualLogBaseModel::Msg]    = "msg";
    roles[VisualLogBaseModel::Prefix] = "prefix";
    return roles;
}

}// namespace
