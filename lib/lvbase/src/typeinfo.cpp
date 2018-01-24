#include "typeinfo.h"

namespace lv{

TypeInfo::TypeInfo(const QByteArray &name)
    : m_name(name)
    , m_serialize(nullptr)
    , m_log(nullptr)
{
}

TypeInfo::~TypeInfo(){

}

TypeInfo::Ptr TypeInfo::create(const QByteArray &name){
    return TypeInfo::Ptr(new TypeInfo(name));
}

}// namespace
