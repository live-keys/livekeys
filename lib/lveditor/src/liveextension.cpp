#include "liveextension.h"

/**
 * \class lv::LiveExtension
 * \brief The class within Qml to be used when we want to add an extension
 * \ingroup lvview
 */
namespace lv{

/** Default contructor */
LiveExtension::LiveExtension(QObject *parent) : QObject(parent)
{

}

/** Returns true if a file interceptor has been set */
bool LiveExtension::hasFileInterceptor() const{
    return m_interceptFile.isCallable();
}

/** Calls the file interceptor with the given values as args */
QJSValue LiveExtension::callFileInterceptor(const QJSValueList &values){
    QJSValue result = m_interceptFile.call(values);
    if ( result.isError() ){
        qWarning("File intercept error: %s", qPrintable(result.toString()));
        return QJSValue();
    }

    return result;
}

/** Shows if we have a language interceptor */
bool LiveExtension::hasLanguageInterceptor() const{
    return m_interceptLanguage.isCallable();
}

/** Calls the language interceptor on the set of given values */
QObject *LiveExtension::callLanguageInterceptor(const QJSValueList& values){
    QJSValue result = m_interceptLanguage.call(values);
    if ( result.isError() ){
        qWarning("Language Intercept Error: %s", qPrintable(result.toString()));
        return nullptr;
    }

    return result.toQObject();
}

/** Shows if we have a menu interceptor */
bool LiveExtension::hasMenuInterceptor() const{
    return m_interceptMenu.isCallable();
}

/** Calls the menu interceptor on the given set of values */
QJSValue LiveExtension::callMenuInterceptor(const QJSValueList &values){
    QJSValue result = m_interceptMenu.call(values);
    if ( result.isError() ){
        qWarning("Menu Intercept Error: %s", qPrintable(result.toString()));
        return QJSValue();
    }

    return result;
}

}// namespace

