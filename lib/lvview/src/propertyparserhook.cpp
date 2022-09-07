#include "propertyparserhook_p.h"
#include "live/qmlsourcelocation.h"
#include "live/qmlerror.h"

namespace lv{

PropertyParserHook::PropertyParserHook(const ViewEngine::ParsedPropertyValidator &propertyValidator, const ViewEngine::ParsedPropertyHandler &propertyHandler, const ViewEngine::ParsedChildrenHandler &childrenHandler, const ViewEngine::ParserReadyHandler &readyHandler)
    : QQmlCustomParser()
    , m_propertyValidator(propertyValidator)
    , m_propertyHandler(propertyHandler)
    , m_childrenHandler(childrenHandler)
    , m_readyHandler(readyHandler)
{
}

void PropertyParserHook::verifyBindings(const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &props){
    for (int i = 0; i < props.count(); ++i){
        const QV4::CompiledData::Binding* binding = props.at(i);
        if ( binding->type == QV4::CompiledData::Binding::Type_AttachedProperty ){
            error(binding, "Attached properties not supported by this type.");
        } else if ( binding->type == QV4::CompiledData::Binding::Type_GroupProperty ){
            error(binding, "Group properties not supported by this type.");
        } else {
            QString propertyName = compilationUnit->stringAt(binding->propertyNameIndex);
            if ( propertyName.isEmpty() && binding->type == QV4::CompiledData::Binding::Type_Object ){
                if ( !m_childrenHandler ){
                    error(binding, "This object does not have a default property set.");
                    continue;
                }
            } else {
                if ( !m_propertyHandler ){
                    error(binding, "Property \'" + propertyName + "\' does not exist for this component.");
                    continue;
                }
            }

            if ( m_propertyValidator ){
                QmlSourceLocation location(compilationUnit->url().toLocalFile(), binding->location.line, binding->location.column);
                QmlSourceLocation valueLocation(compilationUnit->url().toLocalFile(), binding->valueLocation.line, binding->valueLocation.column);

                try{
                    if ( !m_propertyValidator(location, valueLocation, propertyName) ){
                        error(binding, "Property \'" + propertyName + "\' does not exist for this component.");
                    }
                } catch ( lv::Exception& e ){
                    error(binding, QString::fromStdString(e.message()));
                }
            }
        }
    }
}

void PropertyParserHook::applyBindings(QObject *target, const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &props){

    QByteArray imports;
    for ( int i = 0; i < compilationUnit->importCount(); ++i ){
        auto imp = compilationUnit->importAt(i);

        QByteArray impStr = "import ";
        if ( imp->type == QV4::CompiledData::Import::ImportFile ){
            impStr += "\"" + compilationUnit->stringAt(imp->uriIndex).toUtf8() + "\"";
        } else if ( imp->type == QV4::CompiledData::Import::ImportLibrary ){
            impStr += compilationUnit->stringAt(imp->uriIndex).toUtf8();
        } else {
            continue;
        }
        if ( imp->majorVersion >= 0 && imp->minorVersion >= 0){
            impStr += " " + QByteArray::number(imp->majorVersion) + "." + QByteArray::number(imp->minorVersion);
        }
        QByteArray qualifier;
        if ( imp->qualifierIndex >= 0 ){
            qualifier = compilationUnit->stringAt(imp->qualifierIndex).toUtf8();
        }
        if ( !qualifier.isEmpty() )
            impStr += " as " + qualifier;

        imports += (i == 0 ? "" : "\n") + impStr;
    }

    QString file = compilationUnit->url().scheme() == "memory"  ? compilationUnit->url().path() : compilationUnit->url().toLocalFile();

    QQmlEngine* engine = compilationUnit->engine->qmlEngine();
    ViewEngine* ve = ViewEngine::grabFromQmlEngine(engine);

    QList<QmlSourceLocation> children;
    const QV4::CompiledData::Binding* lastChild = nullptr;

    for (int i = 0; i < props.count(); ++i){
        const QV4::CompiledData::Binding* binding = props.at(i);
        QString propertyName = compilationUnit->stringAt(binding->propertyNameIndex);
        QString propertySource = compilationUnit->bindingValueAsString(binding);

        QmlSourceLocation location(file, binding->location.line, binding->location.column);
        QmlSourceLocation valueLocation(file, binding->valueLocation.line, binding->valueLocation.column);

        if ( propertyName.isEmpty() && binding->type == QV4::CompiledData::Binding::Type_Object ){
            lastChild = binding;
            children.append(location);
        } else {
            if ( m_propertyHandler ){
                try{
                    m_propertyHandler(ve, target, imports, location, valueLocation, propertyName, propertySource);
                } catch ( lv::Exception& e ){
                    QmlError(ve, e, target).jsThrow();
                    return;
                }
            }
        }
    }

    if ( children.length() ){
        if ( m_childrenHandler ){
            try{
                m_childrenHandler(ve, target, imports, children);
            } catch ( lv::Exception& e ){
                error(lastChild, QString::fromStdString(e.message()));
            }
        }
    }

    if ( m_readyHandler )
        m_readyHandler(ve, target);
}

}// namespace
