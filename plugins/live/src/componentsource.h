#ifndef COMPONENTSOURCE_H
#define COMPONENTSOURCE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlComponent>

namespace lv{

class Project;

/// \private
class ComponentSource : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlComponent* source READ source WRITE setSource NOTIFY sourceChanged)
    Q_CLASSINFO("DefaultProperty", "source")

public:
    ComponentSource(QObject* parent = nullptr);
    ~ComponentSource();

    void componentComplete();
    void classBegin(){}

    const QString& sourceCode();
    const QString& importSourceCode();

    QQmlComponent* source();
    void setSource(QQmlComponent* source);

signals:
    void sourceChanged();

private:
    void extractSource();

    QQmlComponent*  m_source;
    bool            m_componentComplete;
    QString         m_sourceCode;
    QString         m_importSourceCode;
    lv::Project*    m_project;

};

inline const QString &ComponentSource::sourceCode(){
    extractSource();
    return m_sourceCode;
}

inline const QString &ComponentSource::importSourceCode(){
    extractSource();
    return m_importSourceCode;
}

inline QQmlComponent *ComponentSource::source(){
    return m_source;
}

}// namespace

#endif // COMPONENTSOURCE_H
