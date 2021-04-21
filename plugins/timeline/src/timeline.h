#ifndef LVTIMELINE_H
#define LVTIMELINE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QTimer>
#include <QQmlListProperty>

#include "tracklistmodel.h"
#include "timelineconfig.h"
#include "timelineheadermodel.h"
#include "timelineproperties.h"

namespace lv{

class ViewEngine;
class MLNode;
class Track;

/// \private
class Timeline : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int contentLength                READ contentLength  WRITE setContentLength NOTIFY contentLengthChanged)
    Q_PROPERTY(int cursorPosition               READ cursorPosition WRITE setCursor        NOTIFY cursorPositionChanged)
    Q_PROPERTY(bool isRunning                   READ isRunning      NOTIFY isRunningChanged)
    Q_PROPERTY(double fps                       READ fps            WRITE setFps           NOTIFY fpsChanged)
    Q_PROPERTY(bool loop                        READ loop           WRITE setLoop          NOTIFY loopChanged)
    Q_PROPERTY(bool isRecording                 READ isRecording    NOTIFY isRecordingChanged)
    Q_PROPERTY(QJSValue properties              READ properties     WRITE setProperties    NOTIFY propertiesChanged)
    Q_PROPERTY(QString file                     READ file           WRITE setFile          NOTIFY fileChanged)
    Q_PROPERTY(lv::TimelineConfig* config       READ config         CONSTANT)
    Q_PROPERTY(lv::TrackListModel* trackList    READ trackList      NOTIFY trackListChanged)
    Q_PROPERTY(TimelineHeaderModel* headerModel READ headerModel    CONSTANT)
    Q_PROPERTY(QQmlListProperty<QObject> tracks READ tracks         CONSTANT)
    Q_CLASSINFO("DefaultProperty", "tracks")

public:
    Timeline(QObject* parent = nullptr);

    TimelineConfig* config();
    TrackListModel* trackList();
    TimelineHeaderModel* headerModel();

    int contentLength() const;
    void setContentLength(int contentLength);

    double fps() const;
    void setFps(double fps);

    int cursorPosition() const;
    void setCursor(int cursorPosition);

    bool isRunning() const;

    bool loop() const;
    void setLoop(bool loop);

    QJSValue properties();
    TimelineProperties* propertiesObject();
    void setProperties(QJSValue properties);

    static void appendTrackToList(QQmlListProperty<QObject>*, QObject*);
    static int trackCount(QQmlListProperty<QObject>*);
    static QObject* trackAt(QQmlListProperty<QObject>*, int);
    static void clearTracks(QQmlListProperty<QObject>*);
    QQmlListProperty<QObject> tracks();

    const QString& file() const;
    void setFile(const QString& file);

    static void serialize(ViewEngine* engine, const QObject* o, MLNode &node);
    static QObject *deserialize(ViewEngine* engine, const MLNode &node);

    static void deserialize(Timeline* timeline, ViewEngine *engine, const MLNode& node);

    void classBegin() override{}
    void componentComplete() override;

    void signalTrackNameChanged(Track* track);

    bool isRecording() const;
    void refreshPosition();

public slots:
    void appendTrack(lv::Track* track);
    void removeTrack(int index);

    void start();
    void startRecording();
    void stop();

    QString positionToLabel(qint64 frameNumber, bool shortZero = false);

    void __tick();
    void __trackCursorProcessed(Track* track, qint64 position);

    void load();
    void save();
    void saveAs(const QString& path);

    void setRecord(bool isRecording);

signals:
    void contentLengthChanged();
    void fpsChanged();
    void waitingForTrack(qint64 position);
    void cursorPositionChanged(qint64 position);
    void cursorPositionProcessed(qint64 position);
    void isRunningChanged();
    void loopChanged();
    void propertiesChanged();
    void fileChanged();
    void trackListChanged();
    void trackNameChanged(Track* track);
    void isRecordingChanged();

private:
    void updateCursorPosition(qint64 position);

    qint64 m_cursorPosition;
    qint64 m_contentLength;
    double m_fps;
    bool   m_loop;
    bool   m_isRecording;
    bool   m_isRunning;
    qint64 m_waitingForTrackAt;
    bool   m_isComponentComplete;

    TimelineConfig*      m_config;
    TrackListModel*      m_trackList;
    TimelineHeaderModel* m_headerModel;
    QTimer               m_timer;
    TimelineProperties*  m_properties;
    QString              m_file;
};

inline int Timeline::contentLength() const{
    return m_contentLength;
}

inline TrackListModel *Timeline::trackList(){
    return m_trackList;
}

inline double Timeline::fps() const{
    return m_fps;
}

inline TimelineHeaderModel *Timeline::headerModel(){
    return m_headerModel;
}

inline void Timeline::setFps(double fps){
    if (qFuzzyCompare(m_fps, fps))
        return;

    if ( fps <= 0 ){
        m_timer.stop();
        qWarning("Timeline: Fps has to be > 0");
        return;
    }

    m_timer.setInterval(static_cast<int>(1000 / fps));
    if ( m_timer.isActive() )
        m_timer.start();

    m_fps = fps;
    emit fpsChanged();
}

inline int Timeline::cursorPosition() const{
    return m_cursorPosition;
}

inline void Timeline::setCursor(int cursorPosition){
    if (m_cursorPosition == cursorPosition)
        return;
    if (cursorPosition < 0 || cursorPosition > m_contentLength )
        return;

    if ( m_timer.isActive() )
        m_timer.start();

    updateCursorPosition(cursorPosition);
}

inline bool Timeline::isRunning() const{
    return m_isRunning;
}

inline bool Timeline::loop() const{
    return m_loop;
}

inline void Timeline::setLoop(bool loop){
    if (m_loop == loop)
        return;

    m_loop = loop;
    emit loopChanged();
}

inline void Timeline::setFile(const QString &file){
    if (m_file == file)
        return;

    m_file = file;
    emit fileChanged();

    if ( !m_file.isEmpty() && m_isComponentComplete ){
        load();
    }
}

inline bool Timeline::isRecording() const{
    return m_isRecording;
}

inline void Timeline::setRecord(bool record){
    if (m_isRecording == record)
        return;

    m_isRecording = record;
    emit isRecordingChanged();
}

}// namespace

#endif // LVTIMELINE_H
