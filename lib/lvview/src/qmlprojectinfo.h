#ifndef LVQMLPROJECTINFO_H
#define LVQMLPROJECTINFO_H

#include <QObject>
#include <QUrl>
#include <QJSValue>

namespace lv{

class ViewEngine;
class QmlProjectInfo : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue scriptArgv READ scriptArgv CONSTANT)

public:
    explicit QmlProjectInfo(const QString& path, const QUrl& programPath, ViewEngine* viewEngine, QObject *parent = nullptr);
    ~QmlProjectInfo();

    void setArguments(const QStringList& arguments);
    const QStringList& arguments() const;

    QJSValue scriptArgv() const;

    void onExit(const std::function<void(int)>& exitHandler);

public slots:
    QUrl program() const;
    QString dir() const;
    QString path(const QString& relative) const;

    void exit(int code = 0);

private:
    QString     m_path;
    QUrl        m_programPath;
    ViewEngine* m_viewEngine;
    QStringList m_arguments;
    std::function<void(int)> m_exitHandler;
};

}// namespace

#endif // LVQMLPROJECTINFO_H
