/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/plugininfoextractor.h"
#include "projectqmlscanner_p.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

namespace lv{


/**
 * \class lv::PluginInfoExtractor
 * \ingroup lveditqmljs
 * \brief Extracts plugin information from a given plugin path.
 *
 * Given an import string, i.e. ```import lcvcore 1.0```, the lv::PluginInfoExtractor will extract the information
 * from that plugin in a plugininfo file string.
 *
 * Usage:
 *
 * \code
 * lv::ProjectQmlExtension* qmlHandler = new lv::ProjectQmlExtension(settings, project, engine);
 *
 * lv::PluginInfoExtractor* extractor = qmlHandler->getPluginInfoExtractor(import);
 * if ( extractor ){
 *     extractor->waitForResult(10000);
 *     if (extractor->timedOut() ){
 *         return "Error: Timed out\n";
 *     }
 *
 * }
 * QByteArray result = extractor->result();
 * \endcode
 */

/**
 * \brief PluginInfoExtractor constructor.
 *
 * This is mainly used internally, the lv::PluginInfoExtractor should be captured from a lv::ProjectQmlExtension.
 * As soon as the extractor is created, the search is automaticalluy activated, so the user only has to wait for
 * the result.
 */
PluginInfoExtractor::PluginInfoExtractor(ProjectQmlScanner *scanner, const QString& path, QObject *parent)
    : QObject(parent)
    , m_scanner(scanner)
    , m_path(path)
    , m_timeout(new QTimer)
    , m_isDone(false)
    , m_timedOut(false)
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, SIGNAL(timeout()), this, SLOT(timeOut()));
}

/**
 * \brief PluginInfoExtractor destructor
 */
PluginInfoExtractor::~PluginInfoExtractor(){
    delete m_timeout;
}

/**
 * \brief Waits a specific time for the result
 */
void PluginInfoExtractor::waitForResult(int msTimeout){
    m_timeout->start(msTimeout);
    while (!m_timedOut && !m_isDone ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/**
 * @brief Internal slot used to capture a new project scope.
 */
void PluginInfoExtractor::newProjectScope(){
    if ( !m_isDone ){
        m_timeout->start();
        m_result.clear();
        m_isDone = m_scanner->tryToExtractPluginInfo(m_path, &m_result);
        if ( m_isDone )
            m_timeout->stop();
    }
}

/**
 * \brief Internal slot used to capture the timeout of the waiting time for the extractor.
 */
void PluginInfoExtractor::timeOut(){
    m_timedOut = true;
}

}// namespace
