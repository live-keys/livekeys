#ifndef LVPROPERTYPARSERHOOK_H
#define LVPROPERTYPARSERHOOK_H

#include <QObject>

#include "live/viewengine.h"
#include "private/qqmlcustomparser_p.h"

namespace lv{

class PropertyParserHook : public QQmlCustomParser{

public:
    PropertyParserHook(
        const ViewEngine::ParsedPropertyValidator& propertyValidator,
        const ViewEngine::ParsedPropertyHandler& propertyHandler,
        const ViewEngine::ParsedChildrenHandler& childrenHandler,
        const ViewEngine::ParserReadyHandler&    readyHandler
    );

    void verifyBindings(const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &props) override;
    void applyBindings(QObject *obj, const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;

private:
    ViewEngine::ParsedPropertyValidator m_propertyValidator;
    ViewEngine::ParsedPropertyHandler   m_propertyHandler;
    ViewEngine::ParsedChildrenHandler   m_childrenHandler;
    ViewEngine::ParserReadyHandler      m_readyHandler;
};

}// namespace

#endif // PROPERTYPARSER_H
