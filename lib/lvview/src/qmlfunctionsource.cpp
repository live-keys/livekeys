#include "qmlfunctionsource.h"

namespace lv{

QmlFunctionSource::QmlFunctionSource(const QJSValue &val,
        const QString &source,
        const QByteArray &imports,
        const QmlSourceLocation &location)
    : m_fn(val)
    , m_source(source)
    , m_imports(imports)
    , m_location(location)
{
}

QmlFunctionSource::~QmlFunctionSource(){
}

QmlFunctionSource *QmlFunctionSource::createCompiled(ViewEngine *ve, const QByteArray &imports, const QString &source, const QmlSourceLocation &location){
    QString filePath = location.file().mid(0, location.file().lastIndexOf(".qml")) + "Act.qml";

    ViewEngine::ComponentResult::Ptr cr = ve->compileJsModule(imports, source.toUtf8(), filePath);
    if ( cr->hasError() ){
        QmlError::join(cr->errors).jsThrow();
    }

    QJSValue compiledFn = cr->object->property("exports").value<QJSValue>();
    QmlFunctionSource* fns = new QmlFunctionSource(compiledFn, source, imports, location);
    return fns;
}

}// namespace
