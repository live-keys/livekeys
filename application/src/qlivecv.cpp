#include "qlivecv.h"
#include "qlivecvlog.h"
#include "qcodedocument.h"

#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

QLiveCV::QLiveCV(const QStringList& arguments)
    : m_engine(new QQmlApplicationEngine)
    , m_document(new QCodeDocument)
    , m_dir(QGuiApplication::applicationDirPath())
{
    parseArguments(arguments);
}

QLiveCV::~QLiveCV(){
    delete m_engine;
    delete m_document;
}

void QLiveCV::parseArguments(const QStringList &arguments){
    if ( !arguments.contains("-c" ) )
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
    m_lcvlib.setFileName(library);
    m_lcvlib.load();
}

void QLiveCV::loadQml(const QUrl &url){
    solveImportPaths();

    m_engine->rootContext()->setContextProperty("codeDocument", m_document);
    m_engine->rootContext()->setContextProperty("lcvlog", QLiveCVLog::instance());

    m_engine->load(url);
}
