#include "extensions.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/applicationcontext.h"

#include <QQmlComponent>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>

/**
 * \class lv::Extensions
 * \brief Object used to store all the extensions of LiveKeys
 * \ingroup lvview
 */

namespace lv{

/**
 * \brief Default constructor
 */
Extensions::Extensions(ViewEngine *engine, const QString &settingsPath, QObject *parent)
    : QObject(parent)
    , m_globals(new QQmlPropertyMap(this))
    , m_engine(engine)
{
    m_path = QDir::cleanPath(settingsPath + "/workspace.json");
}

/** Default destructor */
Extensions::~Extensions(){
    delete m_globals;
}

const QString &Extensions::path() const{
    return m_path;
}

} // namespace
