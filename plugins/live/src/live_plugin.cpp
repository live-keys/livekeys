#include "live_plugin.h"
//#include "qlivecvarguments.h"
//#include "qlivecvmain.h"
#include "qstaticloader.h"
#include "qstaticcontainer.h"
#include "qabstractcodeserializer.h"
#include "qnativevaluecodeserializer.h"
#include "qcodeconverter.h"
#include "qlivepalette.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
//    qmlRegisterUncreatableType<lcv::QLiveCVArguments>(
//        uri, 1, 0, "LiveArguments", "LiveArguments is available through the arguments property."
//    );
//    qmlRegisterType<lcv::QLiveCVMain>(  uri, 1, 0, "Main");
    qmlRegisterType<lcv::QStaticLoader>(uri, 1, 0, "StaticLoader");

    qmlRegisterUncreatableType<lcv::QAbstractCodeSerializer>(
        uri, 1, 0, "AbstractCodeSerializer", "Code serializer is of abstract type."
    );
    qmlRegisterType<lcv::QNativeValueCodeSerializer>(uri, 1, 0, "NativeValueCodeSerializer");
    qmlRegisterType<lcv::QCodeConverter>(            uri, 1, 0, "CodeConverter");
    qmlRegisterType<lcv::QLivePalette>(              uri, 1, 0, "LivePalette");
}

void LivePlugin::initializeEngine(QQmlEngine *engine, const char *){
    QStaticContainer* sc = new QStaticContainer(engine);
    engine->rootContext()->setContextProperty("staticContainer", sc);
}
