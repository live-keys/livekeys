/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#ifndef QCAPTURECONTAINER_HPP
#define QCAPTURECONTAINER_HPP

#include <QObject>
#include "QLCVGlobal.hpp"

class QCamCaptureThread;
class QVideoCaptureThread;

/**
 * @brief The QCaptureContainer class
 * @todo Add support for QQuickWindow::afterRendering()
 */
class QCaptureContainer : public QObject{

    Q_OBJECT

public:
    ~QCaptureContainer();

    QVideoCaptureThread* captureThread(const QString& file);

    static QCaptureContainer *instance();
    static void destroyInstance();
    
signals:
    
public slots:

private:
    static QCaptureContainer* m_instance;

    explicit QCaptureContainer(QObject *parent = 0);
    QCaptureContainer(const QCaptureContainer& other);
    QCaptureContainer& operator= (const QCaptureContainer& other);

    QList<QVideoCaptureThread*> m_videoCaptureThreads;
    
};

inline QCaptureContainer *QCaptureContainer::instance(){
    if ( m_instance == 0 ){
        m_instance = new QCaptureContainer();
        std::atexit(&destroyInstance);
    }
    return m_instance;
}

inline void QCaptureContainer::destroyInstance(){
    delete m_instance;
}


#endif // QCAPTURECONTAINER_HPP
