#include "liveextension.h"

namespace lv{

LiveExtension::LiveExtension(QObject *parent) : QObject(parent)
{

}

bool LiveExtension::hasLanguageInterceptor() const{
    return m_interceptLanguage.isCallable();
}

QObject *LiveExtension::callLanguageInterceptor(const QJSValueList& values){
    QJSValue result = m_interceptLanguage.call(values);
    if ( result.isError() ){
        qWarning("Language Intercept Error: %s", qPrintable(result.toString()));
        return nullptr;
    }

    return result.toQObject();
}

bool LiveExtension::hasMenuInterceptor() const{
    return m_interceptMenu.isCallable();
}

QJSValue LiveExtension::callMenuInterceptor(const QJSValueList &values){
    QJSValue result = m_interceptMenu.call(values);
    if ( result.isError() ){
        qWarning("Menu Intercept Error: %s", qPrintable(result.toString()));
        return QJSValue();
    }

    return result;
}

}// namespace

