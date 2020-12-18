#ifndef CUSTOMVIDEOSEGMENT_H
#define CUSTOMVIDEOSEGMENT_H

#include <QObject>
#include <QJSValue>

#include "live/segment.h"

#include "videotrack.h"
#include "qmat.h"

namespace lv{

/// private
class ScriptVideoSegment : public Segment{

    Q_OBJECT
    Q_ENUMS(CursorOperation)
    Q_PROPERTY(QJSValue cursorUpdate READ cursorUpdate WRITE setCursorUpdate NOTIFY cursorUpdateChanged)
    Q_PROPERTY(QJSValue cursorExit   READ cursorExit   WRITE setCursorExit   NOTIFY cursorExitChanged)
    Q_PROPERTY(QString scriptPath    READ scriptPath   WRITE setScriptPath   NOTIFY scriptPathChanged)

public:
    enum CursorOperation{
        CursorEnter = 0,
        CursorNext = 1,
        CursorMove = 2
    };

public:
    explicit ScriptVideoSegment(QObject *parent = nullptr);
    ~ScriptVideoSegment() override;

    QJSValue cursorUpdate() const;
    void setCursorUpdate(QJSValue cursorUpdate);

    QJSValue cursorExit() const;
    void setCursorExit(QJSValue cursorExit);

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit(qint64 position) override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    const QString& scriptPath() const;
    void setScriptPath(const QString& scriptPath);

public slots:
    void updateFrame(qint64 position, QMat* frame);

signals:
    void cursorUpdateChanged(QJSValue cursorUpdate);
    void cursorExitChanged(QJSValue cursorExit);
    void scriptPathChanged();

private:
    qint64            m_lastUpdatePosition;
    QJSValue          m_cursorUpdate;
    QJSValue          m_cursorExit;
    VideoTrack*       m_videoTrack;
    QString           m_scriptPath;
};

inline QJSValue ScriptVideoSegment::cursorUpdate() const{
    return m_cursorUpdate;
}

inline QJSValue ScriptVideoSegment::cursorExit() const{
    return m_cursorExit;
}

inline void ScriptVideoSegment::setCursorUpdate(QJSValue cursorUpdate){
    m_cursorUpdate = cursorUpdate;
    emit cursorUpdateChanged(m_cursorUpdate);
}

inline void ScriptVideoSegment::setCursorExit(QJSValue cursorExit){
    m_cursorExit = cursorExit;
    emit cursorExitChanged(m_cursorExit);
}

inline const QString &ScriptVideoSegment::scriptPath() const{
    return m_scriptPath;
}

inline void ScriptVideoSegment::setScriptPath(const QString &scriptPath){
    if (m_scriptPath == scriptPath)
        return;

    m_scriptPath = scriptPath;
    emit scriptPathChanged();
}

}// namespace

#endif // CUSTOMVIDEOSEGMENT_H
