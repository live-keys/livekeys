#include "live_plugin.h"
#include "qlivecvmain.h"
#include "qstaticloader.h"
#include "qenginemonitor.h"
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
    qmlRegisterType<lcv::QLiveCVMain>(  uri, 1, 0, "Main");
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
    lcv::QEngineMonitor* em = new lcv::QEngineMonitor(engine);
    engine->rootContext()->setContextProperty("engineMonitor", em);
}
