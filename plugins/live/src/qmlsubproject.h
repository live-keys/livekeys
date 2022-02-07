/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVSUBPROJECT_H
#define LVSUBPROJECT_H

#include <QQuickItem>
#include <QQmlComponent>

namespace lv{

class Project;
class Runnable;
class QmlSubproject : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString label                  READ label           WRITE setLabel           NOTIFY labelChanged)
    Q_PROPERTY(QQmlComponent* sourceComponent READ sourceComponent WRITE setSourceComponent NOTIFY sourceComponentChanged)
    Q_PROPERTY(QString source                 READ source          WRITE setSource          NOTIFY sourceChanged)
    Q_PROPERTY(bool sourceMonitor             READ sourceMonitor   WRITE setSourceMonitor   NOTIFY sourceMonitorChanged)
    Q_PROPERTY(QObject* root                  READ root            NOTIFY rootChanged)

public:
    explicit QmlSubproject(QQuickItem *parent = nullptr);
    ~QmlSubproject();

    QQmlComponent* sourceComponent() const;
    const QString& label() const;
    const QString& source() const;
    bool sourceMonitor() const;

    void setSourceComponent(QQmlComponent* sourceComponent);
    void setSource(const QString& source);
    void setLabel(const QString& label);
    void setSourceMonitor(bool sourceMonitor);

    QObject* root() const;

signals:
    void sourceComponentChanged();
    void sourceChanged();
    void labelChanged();
    void rootChanged();
    void sourceMonitorChanged();

public slots:
    void _runnableDestroyed();

protected:
    void componentComplete();

private:
    Runnable*      m_runnable;
    QString        m_source;
    QQmlComponent* m_sourceComponent;
    QString        m_label;
    Project*       m_project;
    bool           m_sourceMonitor;
};

inline QQmlComponent *QmlSubproject::sourceComponent() const{
    return m_sourceComponent;
}

inline const QString& QmlSubproject::label() const{
    return m_label;
}

inline const QString &QmlSubproject::source() const{
    return m_source;
}

inline void QmlSubproject::setSourceComponent(QQmlComponent *source){
    if (m_sourceComponent == source)
        return;

    m_sourceComponent = source;
    emit sourceComponentChanged();
}

inline void QmlSubproject::setSource(const QString &source){
    if ( m_source == source )
        return;

    m_source = source;
    emit sourceChanged();
}

inline void QmlSubproject::setLabel(const QString &label){
    if (m_label == label)
        return;

    m_label = label;
    emit labelChanged();
}

inline bool QmlSubproject::sourceMonitor() const{
    return m_sourceMonitor;
}

inline void QmlSubproject::setSourceMonitor(bool sourceMonitor){
    if (m_sourceMonitor == sourceMonitor)
        return;

    m_sourceMonitor = sourceMonitor;
    emit sourceMonitorChanged();
}

} // namespace

#endif // LVSUBPROJECT_H
