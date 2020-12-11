#include "scriptvideosegment.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/project.h"

#include <QFileInfo>
#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

ScriptVideoSegment::ScriptVideoSegment(QObject *parent)
    : Segment(parent)
    , m_lastUpdatePosition(-1)
    , m_videoTrack(nullptr)
{
}

ScriptVideoSegment::~ScriptVideoSegment(){
}

void ScriptVideoSegment::assignTrack(Track *track){
    VideoTrack* nt = qobject_cast<VideoTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "VideoSegment needs VideoTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_videoTrack = nt;
    Segment::assignTrack(track);
}

void ScriptVideoSegment::cursorEnter(qint64 pos){
    if ( m_cursorUpdate.isCallable() ){
        QJSValueList args = QJSValueList() << QJSValue(static_cast<uint>(pos)) << static_cast<uint>(ScriptVideoSegment::CursorEnter);
        QJSValue result = m_cursorUpdate.call(args);
        if ( result.isError() ){
            lv::ViewContext::instance().engine()->throwError(result, this);
            return;
        }
        if ( m_lastUpdatePosition != pos ){
            setIsProcessing(true);
        }
    }
}

void ScriptVideoSegment::cursorExit(qint64 pos){
    if ( m_cursorExit.isCallable() ){
        QJSValueList args = QJSValueList() << QJSValue(static_cast<uint>(pos));
        QJSValue result = m_cursorExit.call(args);
        if ( result.isError() ){
            lv::ViewContext::instance().engine()->throwError(result, this);
            return;
        }
    }
}

void ScriptVideoSegment::cursorNext(qint64 pos){
    if ( m_cursorUpdate.isCallable() ){
        QJSValueList args = QJSValueList() << QJSValue(static_cast<uint>(pos)) << static_cast<uint>(ScriptVideoSegment::CursorNext);
        QJSValue result = m_cursorUpdate.call(args);
        if ( result.isError() ){
            lv::ViewContext::instance().engine()->throwError(result, this);
            return;
        }
        if ( m_lastUpdatePosition != pos ){
            setIsProcessing(true);
        }
    }
}

void ScriptVideoSegment::cursorMove(qint64 pos){
    if ( m_cursorUpdate.isCallable() ){
        QJSValueList args = QJSValueList() << QJSValue(static_cast<uint>(pos)) << static_cast<uint>(ScriptVideoSegment::CursorMove);
        QJSValue result = m_cursorUpdate.call(args);
        if ( result.isError() ){
            lv::ViewContext::instance().engine()->throwError(result, this);
            return;
        }
        if ( m_lastUpdatePosition != pos ){
            setIsProcessing(true);
        }
    }
}

void ScriptVideoSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);

    QString scriptPath = m_scriptPath;

    QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
    Project* project = qobject_cast<Project*>(projectOb);

    if ( project ){
        if ( scriptPath.startsWith(project->dir()) ){
            scriptPath = scriptPath.mid(project->dir().length() + 1);
        }
    }

    node["script"] = scriptPath.toStdString();
    node["type"] = "ScriptVideoSegment";
}

void ScriptVideoSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    QString scriptPath = QString::fromStdString(node["script"].asString());
    QFileInfo finfo(scriptPath);
    if ( finfo.isRelative() ){
        QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
        Project* project = qobject_cast<Project*>(projectOb);
        if ( project ){
            scriptPath = QFileInfo(project->dir() + "/" + scriptPath).canonicalFilePath();
        }
    }
    setScriptPath(scriptPath);
}

void ScriptVideoSegment::updateFrame(qint64 pos, QMat *frame){
    m_lastUpdatePosition = pos;
    if ( frame ){
        m_videoTrack->surface()->updateSurface(position() + pos, frame);
    }
    if ( isProcessing() ){
        setIsProcessing(false);
        m_videoTrack->notifyCursorProcessed(position() + pos);
    }
}

}// namespace
