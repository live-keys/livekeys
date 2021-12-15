#include "qmlprogram.h"
#include "qmlprojectinfo.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/exception.h"

#include <QQuickItem>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

namespace lv{

QmlProgram::QmlProgram(ViewEngine *engine, const Utf8 &file, const Utf8 &projectPath)
    : QObject(nullptr)
    , m_viewEngine(engine)
    , m_rootPath(projectPath)
    , m_path(file)
    , m_projectInfo(new QmlProjectInfo(
        QString::fromStdString(projectPath.data()),
        QUrl::fromLocalFile(QString::fromStdString(file.data())),
        engine,
        this))
    , m_runSpace(nullptr)
    , m_isLoading(false)
    , m_readerInstance(nullptr)
    , m_contextProviderInstance(nullptr)
{
    connect(m_viewEngine, &ViewEngine::objectAcquired,      this, &QmlProgram::__engineObjectAcquired);
    connect(m_viewEngine, &ViewEngine::objectReady,         this, &QmlProgram::__engineObjectReady);
    connect(m_viewEngine, &ViewEngine::objectCreationError, this, &QmlProgram::__engineObjectCreationError);
}

QmlProgram::~QmlProgram(){
}

void QmlProgram::setRunSpace(QObject *runSpace){
    m_runSpace = runSpace;
}

void QmlProgram::setArguments(const QStringList &arguments){
    m_arguments = arguments;
    m_projectInfo->setArguments(QStringList() << QString::fromStdString(m_path.data()) << arguments);
}

void QmlProgram::setFileReader(std::function<QByteArray (QUrl, QObject *)> reader, QObject *instance){
    m_reader = reader;
    m_readerInstance = instance;
}

void QmlProgram::setContextProvider(std::function<QQmlContext*(QObject*)> provider, QObject *instance){
    m_contextProvider = provider;
    m_contextProviderInstance = instance;
}

const Utf8 &QmlProgram::mainPath() const{
    return m_path;
}

void QmlProgram::run(QQmlContext *ctx, bool runAsync){
    QByteArray content;
    if ( m_reader ){
        content = m_reader(QUrl::fromLocalFile(QString::fromStdString(m_path.data())), m_readerInstance);
    } else {
        std::string contents = m_viewEngine->fileIO()->readFromFile(m_path.data());
        content = QByteArray::fromStdString(contents);
    }

    ctx = ctx
            ? ctx
            : m_contextProvider
              ? m_contextProvider(m_contextProviderInstance)
              : new QQmlContext(m_viewEngine->engine());

    ctx->setContextProperty("project", m_projectInfo);

    if ( runAsync ){
        m_isLoading = true;
        m_viewEngine->createObjectAsync(
            content,
            m_runSpace,
            QUrl::fromLocalFile(QString::fromStdString(m_path.data())),
            this,
            ctx
        );
    } else {
        m_isLoading = true;
        ViewEngine::ComponentResult::Ptr cr = m_viewEngine->createObject(
            QString::fromStdString(m_path.data()),
            content,
            m_runSpace,
            ctx
        );
        if ( cr->hasError() ){
            QmlError error = QmlError::join(cr->errors);
            emit objectCreationError(error.value(), ctx);
            return;
        }
        emit objectAcquired();

        QQuickItem *parentItem = qobject_cast<QQuickItem*>(m_runSpace);
        QQuickItem *item = qobject_cast<QQuickItem*>(m_runSpace);
        if (parentItem && item){
            item->setParentItem(parentItem);
        }
        m_isLoading = false;

        emit objectReady(cr->object, ctx);
    }
}

void QmlProgram::clearCache(){
    m_viewEngine->engine()->clearComponentCache();
}

void QmlProgram::onExit(const std::function<void (int)> &exitHandler){
    m_projectInfo->onExit(exitHandler);
}

QmlProgram *QmlProgram::create(ViewEngine *engine, const Utf8 &p){
    QString path = QString::fromStdString(p.data());
    QFileInfo finfo(path);
    if ( finfo.isDir() ){
        QFileInfo project(QDir::cleanPath(path + "/live.project.json"));
        if ( project.exists() ){
            std::string contents = engine->fileIO()->readFromFile(project.filePath().toStdString());
            MLNode node;
            ml::fromJson(contents, node);
            if ( node.hasKey("run") ){
                return new QmlProgram(engine, node["run"].asString(), p);
            }
        }

        QString lookupPath = lookupProgram(path);
        if ( !lookupPath.isEmpty() )
            return new QmlProgram(engine, lookupPath.toStdString(), path.toStdString());
    } else {
        return new QmlProgram(engine, finfo.filePath().toStdString(), finfo.path().toStdString());
    }
    return nullptr;
}

QmlProgram *QmlProgram::create(ViewEngine *engine, const Utf8 &path, const Utf8 &filePath){
    if ( filePath.find(':') == std::string::npos ){
        QFileInfo finfo(QString::fromStdString(path.data()));
        if ( !finfo.exists() )
            THROW_EXCEPTION(lv::Exception, Utf8("Cannot create program. Path does not exist: %").format(path), Exception::toCode("~Path"));
        QFileInfo programFileInfo(QString::fromStdString(filePath.data()));
        if ( !programFileInfo.exists() )
            THROW_EXCEPTION(lv::Exception, Utf8("Cannot create program. File does not exist: %").format(filePath), Exception::toCode("~Path"));
    }
    return new QmlProgram(engine, filePath, path);
}

QString QmlProgram::lookupProgram(const QString &path){
    QDirIterator dit(path);

    QString bestUpperMatch = "";
    QString bestlowerMatch = "";

    while ( dit.hasNext() ){
        dit.next();
        QString name = dit.fileName();
        QFileInfo finfo = dit.fileInfo();
        if ( name != "." && name != ".." && !finfo.isDir() && finfo.suffix() == "qml" ){
            if ( name == "main.qml" ){
                return path + "/main.qml";
            } else if ( name.size() > 0 && name.at(0).isLower() ){
                bestlowerMatch = name;
            } else {
                bestUpperMatch = name;
            }
        }
    }

    if ( !bestlowerMatch.isEmpty() )
        return path + "/" + bestlowerMatch;
    if ( !bestUpperMatch.isEmpty() )
        return path + "/" + bestUpperMatch;

    return "";
}

void QmlProgram::__engineObjectAcquired(const QUrl &, QObject *ref){
    if ( ref == this ){
        emit objectAcquired();
    }
}

void QmlProgram::__engineObjectReady(QObject *object, const QUrl &, QObject *ref, QQmlContext *context){
    if ( ref == this ){
        m_isLoading = false;
        emit objectReady(object, context);
    }
}

void QmlProgram::__engineObjectCreationError(QJSValue errors, const QUrl &, QObject *ref, QQmlContext *context){
    if ( ref == this ){
        m_isLoading = false;
        emit objectCreationError(errors, context);
    }
}

}// namespace
