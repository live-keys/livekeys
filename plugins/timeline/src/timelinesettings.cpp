#include "timelinesettings.h"

#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/visuallogqt.h"

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

void TimelineSettings::addLoader(const QString &key, const QString &value){
    m_loaders[key] = value;
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
