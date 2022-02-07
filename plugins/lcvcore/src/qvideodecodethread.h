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

#ifndef QVIDEODECODETHREAD_H
#define QVIDEODECODETHREAD_H

#include <QThread>

#include "qvideodecoder.h"

class QMat;
class QTimer;
class QVideoDecodeThreadPrivate;

/// \private
class QVideoDecodeThread: public QThread{

    Q_OBJECT

public:
    QVideoDecodeThread(const QString& file, QVideoDecoder::Properties* properties, QObject* parent = 0);
    ~QVideoDecodeThread();

    QMat*   output();
    QMat* takeMat();
    QTimer* timer() const;
    const QString& file() const;

    int     captureWidth() const;
    int     captureHeight() const;
    double  captureFps() const;

    bool    isCaptureOpened();
    void    processNextFrame();

signals:
    void matReady();
    void isSeekingChanged();

public slots:
    void tick();
    void seekTo(int frame);

protected:
    void run();

private:
    void initializeMatSize();

    void beginSeek();
    void endSeek();

    Q_DISABLE_COPY(QVideoDecodeThread)

    QVideoDecoder::Properties* m_properties;
    QVideoDecodeThreadPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QVideoDecodeThread)
};

#endif // QVIDEODECODETHREAD_H
