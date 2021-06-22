#include "timelinesettings.h"

#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/visuallogqt.h"

#include "live/settings.h"
#include "live/viewengine.h"
#include "live/applicationcontext.h"

#include <QDir>
#include <QFile>

namespace lv{

TimelineSettings::TimelineSettings(const QString &settingsPath, QObject *parent)
    : QObject(parent)
{
    m_path = QDir::cleanPath(settingsPath + "/timeline.json");
    readFile();
}

TimelineSettings::~TimelineSettings(){
}

void TimelineSettings::addTrackType(const QString& typeReference, const QString &label, const QString &path, bool enabled, const QString& extension){
    TimelineSettings::TrackType track;
    track.label         = label;
    track.typeReference = typeReference;
    track.factory       = path;
    track.extensionPath = extension;
    track.enabled       = enabled;

    m_trackTypes[typeReference] = track;
}

TimelineSettings::TrackType TimelineSettings::trackType(const QString &typeReference) const{
    auto it = m_trackTypes.find(typeReference);
    if ( it == m_trackTypes.end() )
        return TimelineSettings::TrackType();

    return it.value();
}

QJSValue TimelineSettings::trackMenu(const QString &typeReference, ViewEngine* ve){
    if ( !ve )
        return QJSValue();

    auto it = m_trackTypes.find(typeReference);
    if ( it == m_trackTypes.end() )
        return QJSValue();

    if ( it.value().extensionPath.isEmpty() )
        return QJSValue();

    ViewEngine::ComponentResult::Ptr cr = ve->createPluginObject(it.value().extensionPath + ".qml", this);
    if ( cr->hasError() ){
        cr->jsThrowError();
        return QJSValue();
    }

    return ve->engine()->newQObject(cr->object).property("menu");
}

void TimelineSettings::addLoader(const QString &key, const QString &value){
    m_loaders[key] = value;
}

TimelineSettings *TimelineSettings::grabFrom(Settings *settings){
    TimelineSettings* ts = qobject_cast<TimelineSettings*>(settings->file("timeline"));
    if ( !ts ){
        ts = new TimelineSettings(settings->path());
        settings->addConfigFile("timeline", ts);
    }
    return ts;

}

void TimelineSettings::readFile(){
    QFile file(m_path);
    if ( file.exists() && file.open(QIODevice::ReadOnly) ){
        MLNode n;
        QByteArray fileContents = file.readAll();
        ml::fromJson(fileContents.data(), n);
        readFileContent(n);
    } else {
        const MLNode& startupcfg = ApplicationContext::instance().startupConfiguration();
        if ( startupcfg.hasKey("timeline") ){
            readFileContent(startupcfg["timeline"]);
        }
    }
}

void TimelineSettings::readFileContent(const MLNode &n){
    if ( n.type() == MLNode::Object ){
        MLNode loadersNode = n["loaders"];
        if ( loadersNode.type() == MLNode::Object ){
            for ( auto it = loadersNode.begin(); it != loadersNode.end(); ++it ){
                if ( it.value().type() == MLNode::String )
                    addLoader(QString::fromStdString(it.key()), QString::fromStdString(it.value().asString()));
            }
        }
    }
}

}// namespace
