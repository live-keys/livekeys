#include "qmldeclarationobject.h"

namespace lv{

QmlDeclarationObject::QmlDeclarationObject(const QmlDeclaration::Ptr& internal, QObject *parent)
    : QObject(parent)
    , m_internal(internal)
{
}

QmlDeclarationObject::~QmlDeclarationObject(){
}

void QmlDeclarationObject::log(ViewEngine *ve, QObject *ob, VisualLog &vl){
    const QmlDeclarationObject* lo = qobject_cast<const QmlDeclarationObject*>(ob);
    if ( !lo )
        return;
    vl << *lo;
}

QString QmlDeclarationObject::location() const{
    if ( m_internal->isForComponent() ){
        return "component";
    } else if ( m_internal->isForImports() ){
        return "imports";
    } else if ( m_internal->isForList() ){
        return "list";
    } else if ( m_internal->isForProperty() ){
        return "property";
    } else if ( m_internal->isForSlot() ){
        return "slot";
    }
    return "";
}

}// namespace
