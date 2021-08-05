#include "qmlmetatypeinfo_p.h"

namespace lv{


QmlMetaTypeInfo::QmlMetaTypeInfo(const lv::QmlInheritanceInfo &typeInfo, ViewEngine* engine, QObject *parent)
    : QObject(parent)
    , m_inheritanceInfo(typeInfo)
    , m_engine(engine)
{
}


QmlMetaTypeInfo::~QmlMetaTypeInfo(){
}

QJSValue QmlMetaTypeInfo::typeName(const QString &typeReference){
    return QJSValue(QmlTypeReference::split(typeReference).name());
}

QJSValue QmlMetaTypeInfo::propertyInfo(const QString &propertyName){
    for ( auto it = m_inheritanceInfo.nodes.begin(); it != m_inheritanceInfo.nodes.end(); ++it ){
        QmlTypeInfo::Ptr ti = *it;
        QmlPropertyInfo pi = ti->propertyAt(propertyName);
        if ( pi.isValid() ){
            QJSValue result = m_engine->engine()->newObject();
            result.setProperty("isWritable", pi.isWritable);
            result.setProperty("isList", pi.isList);
            result.setProperty("name", pi.name);
            result.setProperty("type", pi.typeName.join());
            result.setProperty("parentType", pi.objectType.join());
            return result;
        }
    }
    return QJSValue();
}

} // namespace
