#ifndef LVQMLPLUGININFOEXTRACTOR_H
#define LVQMLPLUGININFOEXTRACTOR_H

#include <QObject>
#include <QQmlEngine>

namespace lv{

class ProjectQmlExtension;
class QmlLanguageScanner;

class QmlPluginInfoExtractor : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString importUri READ importUri WRITE setImportUri NOTIFY importUriChanged)

public:
    explicit QmlPluginInfoExtractor(QObject *parent = nullptr);
    ~QmlPluginInfoExtractor(){}

    void setImportUri(const QString& importUri);
    QString importUri() const;

public slots:
    void printResult(const QString& data);

signals:
    void importUriChanged();
    void pluginInfoReady(const QString& data);
    void message(int level, QString value);

private:
    QmlLanguageScanner*  m_languageScanner;
    QString              m_importUri;
    QQmlEngine*          m_engine;
};

inline QString QmlPluginInfoExtractor::importUri() const{
    return m_importUri;
}

}// namespace

#endif // LVQMLPLUGININFOEXTRACTOR_H
