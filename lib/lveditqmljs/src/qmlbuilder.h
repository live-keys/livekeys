#ifndef LVQMLBUILDER_H
#define LVQMLBUILDER_H

#include <QQuickItem>

namespace lv{

class QmlBuilder : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* sourceComponent READ sourceComponent WRITE setSourceComponent NOTIFY sourceComponentChanged)
    Q_PROPERTY(QString source                 READ source          WRITE setSource          NOTIFY sourceChanged)
    Q_PROPERTY(QObject* object                READ object          NOTIFY objectChanged)

public:
    QmlBuilder(QQuickItem* parent = nullptr);
    ~QmlBuilder() override;

    QQmlComponent* sourceComponent() const;
    const QString& source() const;
    QObject* object() const;

    void setSourceComponent(QQmlComponent* sourceComponent);
    void setSource(const QString& source);

public slots:
    void __updateImplicitWidth();
    void __updateImplicitHeight();

signals:
    void sourceComponentChanged();
    void sourceChanged();
    void objectChanged();

protected:
    void componentComplete() override;

private:
    QQmlComponent* m_sourceComponent;
    QString        m_source;
    QObject*       m_object;
};

inline const QString &QmlBuilder::source() const{
    return m_source;
}

inline QObject *QmlBuilder::object() const{
    return m_object;
}

inline QQmlComponent *QmlBuilder::sourceComponent() const{
    return m_sourceComponent;
}

}// namespace

#endif // LVQMLBUILDER_H
