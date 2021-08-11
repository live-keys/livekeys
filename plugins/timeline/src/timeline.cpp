#include "timeline.h"
#include "track.h"
#include "segmentmodel.h"
#include "timelineobjectproperty.h"

#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/shared.h"

#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/mlnodetoqml.h"

#include <QQmlPropertyMap>
#include <QQmlEngine>
#include <QFile>
#include <QJSValue>
#include <QJSValueIterator>

#include <math.h>

namespace lv{

namespace{
    static const int INDEX_NOTRACK = -1;
}

Timeline::Timeline(QObject *parent)
    : QObject(parent)
    , m_cursorPosition(0)
    , m_contentLength(0)
    , m_fps(1)
    , m_loop(false)
    , m_isRecording(false)
    , m_isRunning(false)
    , m_processingTrackAt(INDEX_NOTRACK)
    , m_waitingForTrackAt(INDEX_NOTRACK)
    , m_isComponentComplete(false)
    , m_config(new TimelineConfig(this))
    , m_trackList(new TrackListModel())
    , m_headerModel(new TimelineHeaderModel(this))
    , m_properties(new TimelineProperties(this))
{
    m_timer.setSingleShot(false);

    connect(&m_timer, &QTimer::timeout, this, &Timeline::__tick);
}

TimelineConfig *Timeline::config(){
    return m_config;
}

void Timeline::appendTrackToList(QQmlListProperty<QObject> *list, QObject *trackOb){
    Timeline* that = reinterpret_cast<Timeline*>(list->data);

    Track* track = qobject_cast<Track*>(trackOb);
    if (!track){
        THROW_QMLERROR(Exception, "Timeline: Trying to append a child that's not a track.", Exception::toCode("~Track"), that);
        return;
    }
    track->setHasApplicationReference(true);
    that->appendTrack(track);
}

int Timeline::trackCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Timeline*>(list->data)->m_trackList->rowCount(QModelIndex());
}

QObject *Timeline::trackAt(QQmlListProperty<QObject> *list, int index){
    return reinterpret_cast<Timeline*>(list->data)->m_trackList->trackAt(index);
}

void Timeline::clearTracks(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Timeline*>(list->data)->m_trackList->clearTracks();
}

void Timeline::removeTrack(int index){
    m_trackList->removeTrack(index);
}

void Timeline::start(){
    if ( m_isRunning && m_isRecording ){
        stop();
    }
    if ( !m_isRunning ){
        __tick();
        m_timer.start();
        m_isRunning = true;
        emit isRunningChanged();
    }
}

void Timeline::startRecording(){
    if ( m_isRunning && !m_isRecording ){
        stop();
    }
    if ( !m_isRunning ){

        int i = m_trackList->totalTracks() - 1;
        while ( i >= 0 ){
            m_trackList->trackAt(i)->recordingStarted();
            --i;
        }
        m_isRecording = true;
        emit isRecordingChanged();

        __tick();
        m_timer.start();
        m_isRunning = true;
        emit isRunningChanged();
    }
}

void Timeline::stop(){
    if ( m_isRunning ){
        if ( m_isRecording ){
            int i = m_trackList->totalTracks() - 1;
            while ( i >= 0 ){
                m_trackList->trackAt(i)->recordingStopped();
                --i;
            }
            m_isRecording = false;
            emit isRecordingChanged();
        }

        m_isRunning = false;
        m_timer.stop();
        emit isRunningChanged();
    }
}

QString Timeline::positionToLabel(qint64 frameNumber, bool shortZero){
    if ( frameNumber == 0 && shortZero )
        return "0";
    double ss = floor(frameNumber / m_fps);
    double mm = floor(ss / 60);
    double hh = floor(mm / 60);
    double ff = frameNumber - (ss * m_fps);


    QString res = QString::asprintf("%d:%d:%d.%d",
        static_cast<int>(hh),
        static_cast<int>(mm),
        static_cast<int>(ss),
        static_cast<int>(ff));
    return res;
}

void Timeline::__tick(){
    if ( m_waitingForTrackAt != INDEX_NOTRACK ){
        m_timer.stop();

    } else {
        if ( m_cursorPosition < m_contentLength ){
            updateCursorPosition(m_cursorPosition + 1);
        } else if ( m_loop ){
            updateCursorPosition(0);
        }
    }
}

void Timeline::__trackCursorProcessed(Track* track, qint64 position){
    if ( m_waitingForTrackAt == INDEX_NOTRACK )
        return;

    if ( position != m_waitingForTrackAt ){
        ViewEngine* ve = ViewEngine::grab(this);
        if ( ve ){
            QmlError(
                ve,
                CREATE_EXCEPTION(lv::Exception, "Timeline: Track processed position is not the same as the current waiting track position.", Exception::toCode("~Mismatch")),
                this
            ).jsThrow();
        }
        return;
    }

    int i = m_trackList->totalTracks() - 1;

    if ( position == m_cursorPosition ){ // go to left over tracks
        while ( i >= 0 ){
            Track* tr = m_trackList->trackAt(i);
            --i;
            if ( tr == track )
                break;
        }
    }

    // process left over tracks (or all tracks in case a new cursor position is set)
    while ( i >= 0 ){
        Track* tr = m_trackList->trackAt(i);
        Track::CursorOperation co = tr->updateCursorPosition(m_cursorPosition);
        if ( co == Track::Delayed ){
            m_waitingForTrackAt = m_cursorPosition;
            emit waitingForTrack(m_cursorPosition);
            return;
        }
        --i;
    }

    m_waitingForTrackAt = INDEX_NOTRACK;

    for ( int i = 0; i < m_trackList->totalTracks(); ++i ){
        m_trackList->trackAt(i)->cursorPositionProcessed(position);
    }

    if ( !m_timer.isActive() && m_isRunning ){ // timer was stopped due to wait
        __tick();
        m_timer.start();
    }

    m_processingTrackAt = INDEX_NOTRACK;

    emit cursorPositionProcessed(position);
}

void Timeline::deserializeWithCheck(Timeline *timeline, ViewEngine *engine, const MLNode &node){
    timeline->stop();

    if ( timeline->m_properties && !timeline->m_properties->isEmpty() ){
        QJSValue val = engine->engine()->newObject();
        val.setProperty("message", "Failed to load file. Properties was already declared in this timeline.");
        emit timeline->fileLoadFailed(val);
        return;
    }

    const MLNode::ArrayType& nodeTracks = node["tracks"].asArray();

    QList<Track*> applicationTracks, commonTracks;
    QList<QString> fileTracks;
    for ( int i = 0; i < timeline->m_trackList->totalTracks(); ++i ){
        applicationTracks.append(timeline->m_trackList->trackAt(i));
    }

    for ( auto it = nodeTracks.begin(); it != nodeTracks.end(); ++it ){
        MLNode nodeTrack = *it;
        QString type = QString::fromStdString(nodeTrack["type"].asString());
        QString name = QString::fromStdString(nodeTrack["name"].asString());

        bool found = false;
        for ( auto it = applicationTracks.begin(); it != applicationTracks.end(); ++it ){
            Track* tr = *it;
            if ( tr->typeReference() == type && tr->name() == name ){
                applicationTracks.erase(it);
                commonTracks.append(tr);
                found = true;
                break;
            }
        }

        if ( !found ){
            fileTracks.append(name);
        }
    }

    if ( applicationTracks.size() > 0 || fileTracks.size() > 0 ){
        QJSValue val = engine->engine()->newObject();
        val.setProperty("message", "Failed to load file. Tracks do not overlap");
        QJSValue leftApplicationTracks = engine->engine()->newArray(applicationTracks.size());
        for ( int i = 0; i < applicationTracks.size(); ++i ){
            QJSValue applicationTrack = engine->engine()->newObject();
            applicationTrack.setProperty("name", applicationTracks[i]->name());
            applicationTrack.setProperty("type", applicationTracks[i]->typeReference());

            leftApplicationTracks.setProperty(i, applicationTrack);
        }

        QJSValue leftFileTracks = engine->engine()->newArray(fileTracks.size());
        for ( int i = 0; i < fileTracks.size(); ++i ){
            QJSValue fileTrack = engine->engine()->newObject();
            fileTrack.setProperty("name", fileTracks[i]);
            leftFileTracks.setProperty(i, fileTrack);
        }
        val.setProperty("tracksFromApplication", leftApplicationTracks);
        val.setProperty("tracksFromFile", leftFileTracks);
        emit timeline->fileLoadFailed(val);
        return;
    }

    for ( int i = 0; i < timeline->m_trackList->totalTracks(); ++i ){
        Track* tr = timeline->m_trackList->trackAt(i);
        tr->segmentModel()->clearSegments();
    }

    timeline->setCursor(0);

    delete timeline->m_properties;
    timeline->m_properties = nullptr;

    try{
        timeline->m_contentLength = node["length"].asInt();
        timeline->setFps(node["fps"].asFloat());

        TimelineProperties* properties = new TimelineProperties(timeline);
        timeline->m_properties = properties;

        if ( node.hasKey("properties") ){

            MLNode propertiesNode = node["properties"];

            for ( auto it = propertiesNode.begin(); it != propertiesNode.end(); ++it ){
                std::string key = it.key();

                QString factory = QString::fromStdString(it.value()["factory"].asString());

                ViewEngine::ComponentResult::Ptr factoryComp = engine->createPluginObject(factory, nullptr);
                if ( factoryComp->hasError() ){
                    factoryComp->jsThrowError();
                } else {

                    QVariant result;
                    QMetaObject::invokeMethod(factoryComp->object, "create", Qt::DirectConnection, Q_RETURN_ARG(QVariant, result));

                    TimelineObjectProperty* property = qobject_cast<TimelineObjectProperty*>(result.value<QObject*>());
                    if ( !property )
                        THROW_EXCEPTION(Exception, "Timeline: Failed to load property from file: " + key, Exception::toCode("~Timeline"));

                    QQmlEngine::setObjectOwnership(property, QQmlEngine::CppOwnership);

                    property->deserialize(engine, it.value());

                    properties->insert(QString::fromStdString(key.c_str()), QVariant::fromValue(property));
                }
            }

            emit timeline->propertiesChanged();
        }

        const MLNode::ArrayType& tracks = node["tracks"].asArray();

        for ( auto it = tracks.begin(); it != tracks.end(); ++it ){

            for ( int i = 0; i < timeline->m_trackList->totalTracks(); ++i ){
                Track* track = timeline->m_trackList->trackAt(i);

                QString type = QString::fromStdString((*it)["type"].asString());
                QString name = QString::fromStdString((*it)["name"].asString());
                if ( track->name() == name && track->typeReference() == type ){
                    Track::deserialize(track, engine, *it);
                }
            }
        }
    } catch ( Exception& e ){
        engine->throwError(&e, timeline);
        return;
    }
}

void Timeline::loadDataFromFile(bool checkTracks){
    if ( m_file.isEmpty() ){
        return;
    }

    QFile file(m_file);
    if ( !file.exists() ){
        THROW_QMLERROR(Exception, "Timeline: Failed to find file: " + m_file.toStdString(), Exception::toCode("~File"), this);
        return;
    }

    if ( !file.open(QIODevice::ReadOnly) ){
        THROW_QMLERROR(Exception, "Failed to open file for reading: " + m_file.toStdString(), Exception::toCode("~File"), this);
        return;
    }

    QByteArray content = file.readAll();

    try {
        MLNode contentNode;
        ml::fromJson(content.data(), contentNode);
        if ( checkTracks ){
            deserializeWithCheck(this, ViewEngine::grab(this), contentNode);
        } else {
            deserialize(this, ViewEngine::grab(this), contentNode);
        }
    } catch (Exception& e) {
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
}

void Timeline::appendTrack(lv::Track *track){
    connect(track, &Track::cursorProcessed, this, &Timeline::__trackCursorProcessed);
    track->setParent(this);
    m_trackList->appendTrack(track);
    track->setContentLength(m_contentLength);

    if ( m_isComponentComplete ){
        track->timelineComplete();
        //TODO: WIll need to reupdate all tracks at that cursor position
        track->updateCursorPosition(m_cursorPosition);
    }
    emit trackListChanged();
}

QQmlListProperty<QObject> lv::Timeline::tracks(){
    return QQmlListProperty<QObject>(
        this,
        this,
        &Timeline::appendTrackToList,
        &Timeline::trackCount,
        &Timeline::trackAt,
        &Timeline::clearTracks);
}

const QString &Timeline::file() const{
    return m_file;
}

void Timeline::serialize(ViewEngine *engine, const QObject *o, MLNode &node){
    const Timeline* timeline = qobject_cast<const Timeline*>(o);
    node = MLNode(MLNode::Object);
    node["length"] = MLNode(static_cast<int>(timeline->m_contentLength));
    node["fps"] = MLNode(timeline->m_fps);


    if ( timeline->m_properties ){

        MLNode propertiesNode(MLNode::Object);

        const QMetaObject* meta = timeline->m_properties->metaObject();
        for ( int i = 0; i < meta->propertyCount(); i++ ){
            QMetaProperty property = meta->property(i);
            QByteArray name = property.name();
            if ( name != "objectName" ){
                QVariant value = property.read(timeline->m_properties);

                TimelineObjectProperty* obj = dynamic_cast<TimelineObjectProperty*>(value.value<QObject*>());
                if ( !obj )
                    THROW_EXCEPTION(Exception, "Property is not of object type: " + name.toStdString(), Exception::toCode("~QObject"));

                MLNode propNode(MLNode::Object);
                obj->serialize(engine, propNode);

                propertiesNode[name.toStdString()] = propNode;
            }
        }

        node["properties"] = propertiesNode;
    }

    MLNode tracksNode;
    TrackListModel::serialize(engine, timeline->m_trackList, tracksNode);
    node["tracks"] = tracksNode;
}

QObject *Timeline::deserialize(ViewEngine *engine, const MLNode &node){
    Timeline* timeline = new Timeline;
    deserialize(timeline, engine, node);
    return timeline;
}

void Timeline::deserialize(Timeline *timeline, ViewEngine *engine, const MLNode &node){
    timeline->m_trackList->clearTracks();
    timeline->setCursor(0);
    timeline->m_isRunning = false;

    delete timeline->m_properties;
    timeline->m_properties = nullptr;

    emit timeline->isRunningChanged();

    try{
        timeline->m_contentLength = node["length"].asInt();
        timeline->setFps(node["fps"].asFloat());

        TimelineProperties* properties = new TimelineProperties(timeline);
        timeline->m_properties = properties;

        if ( node.hasKey("properties") ){

            MLNode propertiesNode = node["properties"];

            for ( auto it = propertiesNode.begin(); it != propertiesNode.end(); ++it ){
                std::string key = it.key();

                QString factory = QString::fromStdString(it.value()["factory"].asString());

                ViewEngine::ComponentResult::Ptr factoryComp = engine->createPluginObject(factory, nullptr);
                if ( factoryComp->hasError() ){
                    factoryComp->jsThrowError();
                } else {

                    QVariant result;
                    QMetaObject::invokeMethod(factoryComp->object, "create", Qt::DirectConnection, Q_RETURN_ARG(QVariant, result));

                    TimelineObjectProperty* property = qobject_cast<TimelineObjectProperty*>(result.value<QObject*>());
                    if ( !property )
                        THROW_EXCEPTION(Exception, "Timeline: Failed to load property from file: " + key, Exception::toCode("~Timeline"));

                    QQmlEngine::setObjectOwnership(property, QQmlEngine::CppOwnership);

                    property->deserialize(engine, it.value());

                    properties->insert(QString::fromStdString(key.c_str()), QVariant::fromValue(property));
                }
            }

            emit timeline->propertiesChanged();

        }

        const MLNode::ArrayType& tracks = node["tracks"].asArray();

        for ( auto it = tracks.begin(); it != tracks.end(); ++it ){
            QString factory = QString::fromStdString((*it)["factory"].asString());

            ViewEngine::ComponentResult::Ptr factoryComp = engine->createPluginObject(factory, nullptr);
            if ( factoryComp->hasError() ){
                factoryComp->jsThrowError();
            } else {
                QVariant result;
                QMetaObject::invokeMethod(factoryComp->object, "create", Qt::DirectConnection, Q_RETURN_ARG(QVariant, result));

                Track* track = qobject_cast<Track*>(result.value<QObject*>());
                if ( !track )
                    THROW_EXCEPTION(Exception, "Timeline: Failed to load track from file: " + (*it)["name"].asString(), Exception::toCode("~Timeline"));

                timeline->appendTrack(track);
                track->setParent(timeline);
                track->timelineComplete();
                Track::deserialize(track, engine, *it);
            }

        }
    } catch ( Exception& e ){
        engine->throwError(&e, timeline);
        return;
    }
}

void Timeline::componentComplete(){
    m_isComponentComplete = true;

    if ( !m_file.isEmpty() ){
        if ( m_trackList->totalTracks() != 0 ){
            loadDataFromFile(true);
        } else {
            loadDataFromFile(false);
        }
    } else {
        for ( int i = 0; i < m_trackList->totalTracks(); ++i ){
            Track* track = m_trackList->trackAt(i);
            track->timelineComplete();
        }
    }
}

void Timeline::signalTrackNameChanged(Track *track){
    emit trackNameChanged(track);
}

void Timeline::refreshPosition(){
    // Make sure no tracks are being processed to avoid an infinite loop
    if ( m_processingTrackAt == INDEX_NOTRACK )
        updateCursorPosition(m_cursorPosition);
}

void Timeline::load(){
    loadDataFromFile(false);
}

void Timeline::save(){
    if ( m_file.isEmpty() ){
        return;
    }

    try {
        //TODO: Pick lockedio if available
        QFile file(m_file);
        if( file.open(QIODevice::WriteOnly) ){
            MLNode result;
            serialize(ViewContext::instance().engine(), this, result);

            std::string resultData;
            ml::toJson(result, resultData);
            file.write(resultData.c_str());
            file.close();
        }
    } catch (lv::Exception& e) {
        QmlError(e, this).jsThrow();
    }
}

void Timeline::saveAs(const QString &path){
    if ( m_file != path ){
        m_file = path;
        emit fileChanged();
    }

    save();
}

void Timeline::updateCursorPosition(qint64 position){
    m_cursorPosition = position;
    emit cursorPositionChanged(position);

    if ( m_waitingForTrackAt >= 0 )
        return;

    m_processingTrackAt = m_cursorPosition;

    int i = m_trackList->totalTracks() - 1;
    while ( i >= 0 ){
        Track* tr = m_trackList->trackAt(i);
        Track::CursorOperation co = tr->updateCursorPosition(position);
        if ( co == Track::Delayed ){
            m_waitingForTrackAt = position;
            emit waitingForTrack(position);
            return;
        }
        --i;
    }

    i = m_trackList->totalTracks() - 1;
    while ( i >= 0 ){
        Track* tr = m_trackList->trackAt(i);
        tr->cursorPositionProcessed(position);
        --i;
    }

    m_processingTrackAt = INDEX_NOTRACK;

    emit cursorPositionProcessed(position);
}

void Timeline::setContentLength(int contentLength){
    if (m_contentLength == contentLength)
        return;

    for ( int i = 0; i < m_trackList->totalTracks(); ++i ){
        Track* tr = m_trackList->trackAt(i);
        tr->setContentLength(contentLength);
    }

    m_contentLength = contentLength;
    emit contentLengthChanged();
}

QJSValue Timeline::properties(){
    ViewEngine* ve = ViewEngine::grab(this);
    if ( ve )
        return ve->engine()->newQObject(m_properties);
    return QJSValue();
}

TimelineProperties *Timeline::propertiesObject(){
    return m_properties;
}

void Timeline::setProperties(QJSValue properties){
    if ( properties.isQObject() ){
        QObject* ob = properties.toQObject();
        const QMetaObject* meta = ob->metaObject();

        for ( int i = 0; i < meta->propertyCount(); i++ ){
            QMetaProperty property = meta->property(i);
            QByteArray name = property.name();
            if ( name != "objectName" ){
                m_properties->insertAndCheckValue(property.name(), property.read(ob));
            }
        }

    } else if ( properties.isObject() ){
        ViewEngine* engine = ViewEngine::grab(this);
        if ( !engine )
            return;
        QJSValueIterator it(properties);
        QList<Shared*> shared;
        while ( it.hasNext() ){
            it.next();
            m_properties->insertAndCheckValue(it.name(), Shared::transfer(it.value(), shared));
        }
    }

    emit propertiesChanged();
}

}// namespace
