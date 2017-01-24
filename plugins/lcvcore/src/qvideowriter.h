#ifndef QVIDEOWRITER_H
#define QVIDEOWRITER_H

#include <QQuickItem>
#include <QQmlParserStatus>
#include <QJSValue>
#include "qmat.h"

class QVideoWriterThread;
class QVideoWriter : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QJSValue init     READ init          WRITE  setInit       NOTIFY initChanged)
    Q_PROPERTY(QString filename  READ filename      WRITE  setFilename   NOTIFY filenameChanged)
    Q_PROPERTY(QMat* input       READ input         WRITE  setInput      NOTIFY inputChanged)
    Q_PROPERTY(int framesWritten READ framesWritten NOTIFY framesWrittenChanged)

    friend class QVideoWriterThread;

public:
    explicit QVideoWriter(QQuickItem *parent = 0);
    virtual ~QVideoWriter();

    const QJSValue& init() const;
    const QString& filename() const;

    QMat* input() const;
    int framesWritten() const;

    QString getKey() const;

protected:
    void componentComplete();

signals:
    void initChanged();
    void filenameChanged();
    void inputChanged();
    void framesWrittenChanged();

public slots:
    void setInit(const QJSValue& init);
    void setFilename(const QString& filename);
    void setInput(QMat* input);
    void save();

    void write(QMat* image);

private:
    QVideoWriterThread* createThread();

    QJSValue m_init;
    QString  m_filename;
    QMat*    m_input;

    int      m_fourcc;
    double   m_fps;
    cv::Size m_frameSize;
    bool     m_isColor;

    QVideoWriterThread* m_thread;
};

inline const QJSValue& QVideoWriter::init() const{
    return m_init;
}

inline const QString &QVideoWriter::filename() const{
    return m_filename;
}

inline QMat *QVideoWriter::input() const{
    return m_input;
}

inline void QVideoWriter::setInput(QMat *input){
    m_input = input;
    emit inputChanged();
    write(input);
}



#endif // QVIDEOWRITER_H
