/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

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
