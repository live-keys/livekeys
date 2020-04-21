#ifndef LVTRACK_H
#define LVTRACK_H

#include <QObject>
#include <QQmlListProperty>

namespace lv{

class Timeline;
class Segment;
class SegmentModel;
class Track : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name                       READ name WRITE setName NOTIFY nameChanged)
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

    const QString& name() const;
    void setName(const QString& name);

    SegmentModel* segmentModel();

    static void appendSegmentToList(QQmlListProperty<QObject>*, QObject*);
    static int segmentCount(QQmlListProperty<QObject>*);
    static QObject* segmentAt(QQmlListProperty<QObject>*, int);
    static void clearSegments(QQmlListProperty<QObject>*);
    QQmlListProperty<QObject> segments();

    CursorOperation updateCursorPosition(qint64 newPosition);

public slots:
    bool addSegment(lv::Segment* segment);
    lv::Segment* takeSegment(lv::Segment* segment);
    qint64 availableSpace(qint64 position);
    lv::Timeline* timeline();

    void __segmentModelItemsChanged(qint64 startPosition, qint64 endPosition);

signals:
    void nameChanged();
    void cursorProcessed(Track* track, qint64 position);

private:
    QString       m_name;
    SegmentModel* m_segmentModel;

    qint64        m_cursorPosition;

    Segment*      m_activeSegment;
};

inline const QString &Track::name() const{
    return m_name;
}

inline void Track::setName(const QString &name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

inline SegmentModel *Track::segmentModel(){
    return m_segmentModel;
}

}// namespace

#endif // LVTRACK_H
