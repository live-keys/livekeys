#ifndef QVideoCapture_HPP
#define QVideoCapture_HPP

#include "QMatDisplay.hpp"

class QVideoCaptureThread;
class QVideoCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString file   READ file   WRITE setFile   NOTIFY fileChanged)
    Q_PROPERTY(bool    paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps    READ fps    WRITE setFps    NOTIFY fpsChanged)

public:
    explicit QVideoCapture(QQuickItem *parent = 0);
    ~QVideoCapture();

    const QString& file() const;
    void setFile(const QString& file);

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

public slots:
    void switchMat();

signals:
    void fileChanged();
    void pausedChanged();
    void fpsChanged();

private:
    void initializeMatSize();

    QVideoCapture(const QVideoCapture& other);
    QVideoCapture& operator= (const QVideoCapture& other);

    QString m_file;
    bool    m_paused;
    qreal   m_fps;
    QMat*   m_restore;

    QVideoCaptureThread* m_thread;

};

inline const QString &QVideoCapture::file() const{
    return m_file;
}

inline bool QVideoCapture::paused() const{
    return m_paused;
}

inline qreal QVideoCapture::fps() const{
    return m_fps;
}

#endif // QVideoCapture_HPP
