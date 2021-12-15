#ifndef LVQMLPROGRAM_H
#define LVQMLPROGRAM_H

#include <QObject>
#include <QUrl>
#include <QQmlContext>

#include "live/utf8.h"
#include "live/program.h"
#include "live/viewengine.h"

namespace lv{

class QmlProjectInfo;

class LV_VIEW_EXPORT QmlProgram : public QObject, public Program{

    Q_OBJECT

public:
    virtual ~QmlProgram();

    void setRunSpace(QObject* runSpace);
    void setArguments(const QStringList& arguments);
    void setFileReader(std::function<QByteArray(QUrl,QObject*)> reader, QObject* instance = nullptr);
    void setContextProvider(std::function<QQmlContext*(QObject*)> provider, QObject* instance = nullptr);

    const Utf8& mainPath() const;
    const Utf8& rootPath() const;

    void run(QQmlContext* ctx = nullptr, bool runAsync = true);
    void clearCache();
    bool isLoading();
    ViewEngine* viewEngine() const;

    void onExit(const std::function<void(int)>& exitHandler);

    static QmlProgram* create(ViewEngine* engine, const Utf8& path);
    static QmlProgram* create(ViewEngine* engine, const Utf8& path, const Utf8& filePath);

public slots:
    void __engineObjectAcquired(const QUrl& file, QObject* ref);
    void __engineObjectReady(QObject* object, const QUrl& file, QObject* ref, QQmlContext* context);
    void __engineObjectCreationError(QJSValue errors, const QUrl&, QObject* reference, QQmlContext* context);

signals:
    void objectAcquired();
    void objectReady(QObject* object, QQmlContext* context);
    void objectCreationError(QJSValue errors, QQmlContext* context);

private:
    static QString lookupProgram(const QString& path);
    QmlProgram(ViewEngine* engine, const Utf8& path, const Utf8& projectPath = "");

    ViewEngine*     m_viewEngine;
    Utf8            m_rootPath;
    Utf8            m_path;
    QmlProjectInfo* m_projectInfo;
    QObject*        m_runSpace;
    bool            m_isLoading;
    QStringList     m_arguments;

    QObject*        m_readerInstance;
    std::function<QByteArray(QUrl,QObject*)> m_reader;
    QObject*        m_contextProviderInstance;
    std::function<QQmlContext*(QObject*)> m_contextProvider;

};

inline const Utf8 &QmlProgram::rootPath() const{
    return m_rootPath;
}

inline bool QmlProgram::isLoading(){
    return m_isLoading;
}

inline ViewEngine *QmlProgram::viewEngine() const{
    return m_viewEngine;
}

}// namespace

#endif // LVQMLPROGRAM_H
