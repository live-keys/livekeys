#ifndef QSTATICLOADER_H
#define QSTATICLOADER_H

#include "qliveglobal.h"
#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>

namespace lcv{

class QStaticLoaderItem;
class Q_LIVE_EXPORT QStaticLoader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* source READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(QObject*       item   READ item    NOTIFY itemChanged)

public:
    explicit QStaticLoader(QQuickItem *parent = 0);
    ~QStaticLoader();

    QQmlComponent* source() const;
    QObject* item() const;

signals:
    void sourceChanged();
    void itemChanged();
    void itemCreated();

public slots:
    void setSource(QQmlComponent* arg);
    void staticLoad(const QString& id);

private:
    void createObject();

    QQmlComponent*     m_source;
    QStaticLoaderItem* m_data;

};

inline QQmlComponent *QStaticLoader::source() const{
    return m_source;
}

}// namespace

#endif // QSTATICLOADER_H
