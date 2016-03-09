#include "qlivecv.h"
#include "qlivecvlog.h"
#include "qcodedocument.h"

#include <QLibrary>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

QLiveCV::QLiveCV(int argc, char *argv[])
    : m_app(new QGuiApplication(argc, argv))
    , m_engine(new QQmlApplicationEngine)
    , m_document(new QCodeDocument)
    , m_dir(QGuiApplication::applicationDirPath())
{
    QGuiApplication::setApplicationName("Live CV");
    QGuiApplication::setApplicationVersion(versionString());

//    parseArguments();
}

QLiveCV::~QLiveCV(){
//    delete m_engine;
//    delete m_document;
//    delete m_app;
}

void QLiveCV::parseArguments(){
    if ( !m_app->arguments().contains("-c" ) )
        qInstallMessageHandler(&QLiveCVLog::logFunction);
}

void QLiveCV::solveImportPaths(){
    QStringList importPaths = m_engine->importPathList();
    m_engine->setImportPathList(QStringList());
    for ( QStringList::iterator it = importPaths.begin(); it != importPaths.end(); ++it ){
        if ( *it != dir() )
            m_engine->addImportPath(*it);
    }
    m_engine->addImportPath(dir() + "/plugins");
}

void QLiveCV::loadLibrary(const QString &library){
    static QLibrary lib(library);
    lib.load();
}

void QLiveCV::loadQml(const QUrl &url){

    solveImportPaths();

    m_engine->rootContext()->setContextProperty("codeDocument", m_document);
    m_engine->rootContext()->setContextProperty("lcvlog", QLiveCVLog::instance());

    m_engine->load(QUrl(QStringLiteral("plugins/main.qml")));

//    m_app->exec();
}

int QLiveCV::exec(){
    return m_app->exec();
}
