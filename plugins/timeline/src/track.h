#ifndef LVTRACK_H
#define LVTRACK_H

#include <QObject>
#include <QQmlListProperty>
#include <QJSValue>

#include "lvtimelineglobal.h"

class QQmlContext;

namespace lv{

class Timeline;
class Segment;
class SegmentModel;
class ViewEngine;
class MLNode;

class LV_TIMELINE_EXPORT Track : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name                       READ name                    WRITE setName                    NOTIFY nameChanged)
    Q_PROPERTY(bool hasApplicationReference       READ hasApplicationReference WRITE setHasApplicationReference NOTIFY hasApplicationReferenceChanged)
    Q_PROPERTY(lv::SegmentModel* segmentModel     READ segmentModel CONSTANT )
    Q_PROPERTY(QQmlListProperty<QObject> segments READ segments)
    Q_CLASSINFO("DefaultProperty", "segments")

public:
    enum CursorOperation{
        Ready = 0,
        Delayed
    };

public:
    explicit Track(QObject *parent = nullptr);
    virtual ~Track();

    const QString& name() const;
    void setName(const QString& name);

    SegmentModel* segmentModel();

    static void appendSegmentToList(QQmlListProperty<QObject>*, QObject*);
    static int segmentCount(QQmlListProperty<QObject>*);
    static QObject* segmentAt(QQmlListProperty<QObject>*, int);
    static void clearSegments(QQmlListProperty<QObject>*);
    QQmlListProperty<QObject> segments();

    virtual CursorOperation updateCursorPosition(qint64 newPosition);
    virtual void cursorPositionProcessed(qint64 position);
    virtual void recordingStarted();
    virtual void recordingStopped();

    virtual void setContentLength(qint64 contentLength);

    static void serialize(ViewEngine* engine, const QObject* o, MLNode &node);
    static void deserialize(Track* track, ViewEngine* engine, const MLNode &node);

    virtual void serialize(ViewEngine* engine, MLNode& node) const;
    virtual void deserialize(ViewEngine* engine, const MLNode& node);

    virtual void setSegmentPosition(Segment* segment, unsigned int position);
    virtual void setSegmentLength(Segment* segment, unsigned int length);

    QObject *timelineProperties() const;

    virtual void timelineComplete();
    void notifyCursorProcessed(qint64 position);

    void setHasApplicationReference(bool value);
    bool hasApplicationReference() const;

    QQmlContext* timelineContext();

    void refreshPosition();

public slots:
    bool addSegment(lv::Segment* segment);
    lv::Segment* takeSegment(lv::Segment* segment);
    qint64 availableSpace(qint64 position);
    lv::Timeline* timeline();

    void __segmentModelItemsChanged(qint64 startPosition, qint64 endPosition);

    virtual QString typeReference() const;
    virtual QJSValue configuredProperties(lv::Segment*) const;

signals:
    void nameChanged();
    void hasApplicationReferenceChanged();
    void cursorProcessed(Track* track, qint64 position);

protected:
    qint64 cursorPosition() const;
    void assignCursorPosition(qint64 position);

private:
    QString       m_name;
    SegmentModel* m_segmentModel;

    qint64        m_cursorPosition;
    bool          m_timelineReady;
    bool          m_applicationReference;

    Segment*      m_activeSegment;
};

inline const QString &Track::name() const{
    return m_name;
}

inline SegmentModel *Track::segmentModel(){
    return m_segmentModel;
}

inline bool Track::hasApplicationReference() const{
    return m_applicationReference;
}

inline qint64 Track::cursorPosition() const{
    return m_cursorPosition;
}

}// namespace

#endif // LVTRACK_H
