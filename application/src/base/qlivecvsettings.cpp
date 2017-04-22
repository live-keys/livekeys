#include "qlivecvsettings.h"
#include "qeditorsettings.h"
#include "qlicensesettings.h"

#include <QDir>

namespace lcv{

QLiveCVSettings::QLiveCVSettings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_editor(0)
    , m_license(0)
{
    m_editor  = new QEditorSettings(path + "/editor.json");
    m_license = new QLicenseSettings(path + "/licenses.json");
}

QLiveCVSettings::~QLiveCVSettings(){
    delete m_editor;
    delete m_license;
}

QLiveCVSettings *QLiveCVSettings::initialize(const QString &path, QObject *parent){
    if ( !QDir(path).exists() ){
        if ( !QDir().mkdir(path) ){
            qWarning("Failed to create configuration directory \'config\'\n");
        }
    }
    return new QLiveCVSettings(path, parent);
}

QLicenseContainer* QLiveCVSettings::license(){
    return m_license->container();
}

}// namespace
