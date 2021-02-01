#ifndef JSMODULE_H
#define JSMODULE_H

#include <QObject>

#include <QJSValue>
#include <QMap>

class QQmlEngine;

class JsModule : public QObject{

    Q_OBJECT

public:
    explicit JsModule(QObject *parent = nullptr);
    void setEngine(QQmlEngine* engine);

    static QObject *provider(QQmlEngine* engine, QJSEngine*);

public slots:
    QJSValue require(const QString& path, const QJSValue& context, bool reload);

private:
    QQmlEngine* m_engine;

    QMap<QString, QJSValue> m_loadedModules;
};

#endif // JSMODULE_H
