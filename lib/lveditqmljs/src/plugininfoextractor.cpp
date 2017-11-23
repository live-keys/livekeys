/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

PluginInfoExtractor::~PluginInfoExtractor(){
    delete m_timeout;
}

void PluginInfoExtractor::waitForResult(int msTimeout){
    m_timeout->start(msTimeout);
    while (!m_timedOut && !m_isDone ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void PluginInfoExtractor::newProjectScope(){
    if ( !m_isDone ){
        m_timeout->start();
        m_result.clear();
        m_isDone = m_scanner->tryToExtractPluginInfo(m_path, &m_result);
        if ( m_isDone )
            m_timeout->stop();
    }
}

void PluginInfoExtractor::timeOut(){
    m_timedOut = true;
}

}// namespace
