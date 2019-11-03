#ifndef LVQMLPLUGININFOEXTRACTOR_H
#define LVQMLPLUGININFOEXTRACTOR_H

#include <QObject>

namespace lv{

class ProjectQmlExtension;
class PluginInfoExtractor;

class QmlPluginInfoExtractor : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString importUri READ importUri WRITE setImportUri NOTIFY importUriChanged)

public:
    explicit QmlPluginInfoExtractor(QObject *parent = nullptr);
    ~QmlPluginInfoExtractor(){}

    void setImportUri(const QString& importUri);
    QString importUri() const;

signals:
    void importUriChanged();
    void pluginInfoReady(const QString& data);

public slots:
    void __pluginInfoReady();

private:
    ProjectQmlExtension* m_qmlExtension;
    PluginInfoExtractor* m_extractor;
    QString              m_importUri;
};

inline QString QmlPluginInfoExtractor::importUri() const{
    return m_importUri;
}

}// namespace

#endif // LVQMLPLUGININFOEXTRACTOR_H
