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

#ifndef QVideoCapture_HPP
#define QVideoCapture_HPP

#include "QMatDisplay.hpp"
#include "QLCVGlobal.hpp"

class QVideoCaptureThread;
class QVideoCapture : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat*   output       READ output       WRITE setOutput       NOTIFY outChanged)
    Q_PROPERTY(bool    linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)
    Q_PROPERTY(QString file         READ file         WRITE setFile         NOTIFY fileChanged)
    Q_PROPERTY(bool    paused       READ paused       WRITE setPaused       NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps          READ fps          WRITE setFps          NOTIFY fpsChanged)
    Q_PROPERTY(int     currentFrame READ currentFrame WRITE seekTo          NOTIFY outChanged)
    Q_PROPERTY(int     totalFrames  READ totalFrames  WRITE setTotalFrames  NOTIFY totalFramesChanged)
    Q_PROPERTY(bool    loop         READ loop         WRITE setLoop         NOTIFY loopChanged)

public:
    explicit QVideoCapture(QQuickItem *parent = 0);
    virtual ~QVideoCapture();

    const QString& file() const;
    void setFile(const QString& file);

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

    int totalFrames() const;
    void setTotalFrames(int totalFrames);

    int currentFrame() const;

    QMat* output();
    void setOutput(QMat* mat);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

    bool loop() const;
    void setLoop(bool loop);

public slots:
    void switchMat();
    void seekTo(int frame);

signals:
    void outChanged();
    void linearFilterChanged();
    void fileChanged();
    void pausedChanged();
    void fpsChanged();
    void totalFramesChanged();
    void loopChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

private:
    void initializeMatSize();

    QVideoCapture(const QVideoCapture& other);
    QVideoCapture& operator= (const QVideoCapture& other);

    QString m_file;
    qreal   m_fps;
    QMat*   m_restore;
    QMat*   m_output;
    bool    m_linearFilter;
    bool    m_loop;

    QVideoCaptureThread* m_thread;

};

inline const QString &QVideoCapture::file() const{
    return m_file;
}

inline qreal QVideoCapture::fps() const{
    return m_fps;
}

inline QMat *QVideoCapture::output(){
    return m_output;
}

inline void QVideoCapture::setOutput(QMat *mat){
    m_output = mat;
    emit outChanged();
}

inline bool QVideoCapture::linearFilter() const{
    return m_linearFilter;
}

inline void QVideoCapture::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

inline bool QVideoCapture::loop() const{
    return m_loop;
}

#endif // QVideoCapture_HPP
