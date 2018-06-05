#ifndef STRINGBASEDLOADER_H
#define STRINGBASEDLOADER_H

#include "qliveglobal.h"
#include <QQuickItem>

namespace lv{

class Q_LIVE_EXPORT StringBasedLoader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE  setSource NOTIFY sourceChanged)
    Q_PROPERTY(QObject* item  READ item   NOTIFY itemChanged)

public:
    explicit StringBasedLoader(QQuickItem *parent = nullptr);
    ~StringBasedLoader();

    const QString &source() const;
    void setSource(const QString& source);
    QObject* item() const;

signals:
    void sourceChanged();
    void itemChanged();

private:
    void createObject();

    QString        m_source;
    QQmlComponent* m_sourceComponent;
    QObject*       m_item;
};

inline const QString& StringBasedLoader::source() const{
    return m_source;
}

inline QObject *StringBasedLoader::item() const{
    return m_item;
}

}// namespace

#endif // STRINGBASEDLOADER_H
