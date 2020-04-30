#ifndef LVQMLEXEC_H
#define LVQMLEXEC_H

#include <QObject>
#include <QQmlParserStatus>
#include <QProcess>

#include "live/qmlstream.h"

namespace lv{

class QmlWritableStream;
class QmlExec : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_ENUMS(ChannelMode)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString     path     READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QStringList args     READ args WRITE setArgs NOTIFY argsChanged)
    Q_PROPERTY(ChannelMode mode     READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(lv::QmlStream* input READ input   WRITE setInput   NOTIFY inChanged)
    Q_PROPERTY(lv::QmlStream* out   READ out CONSTANT)

public:
    enum ChannelMode{
        Silent,
        Read,
        Write,
        ReadWrite
    };

public:
    explicit QmlExec(QObject *parent = nullptr);
    ~QmlExec() override;

    const QString& path() const;
    void setPath(const QString& path);

    const QStringList& args() const;
    void setArgs(const QStringList& args);

    void setMode(ChannelMode mode);
    ChannelMode mode() const;

    QmlStream *out() const;

    void setInput(lv::QmlStream* input);
    lv::QmlStream* input() const;

    static void onInStream(QObject* that, const QJSValue& val);

protected:
    void classBegin() override{}
    void componentComplete() override;

signals:
    void ready();
    void finished(int code);
    void pathChanged();
    void argsChanged();
    void modeChanged();
    void inChanged();

public slots:
    void __processRead();
    void __processFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void run();
    void closeInput();


private:
    bool        m_componentComplete;
    QProcess*   m_process;
    QString     m_path;
    QStringList m_args;
    ChannelMode m_mode;

    lv::QmlStream* m_out;
    lv::QmlStream* m_in;
};

inline const QString &QmlExec::path() const{
    return m_path;
}

inline const QStringList &QmlExec::args() const{
    return m_args;
}

inline void QmlExec::setPath(const QString &path){
    if (m_path == path)
        return;

    m_path = path;
    emit pathChanged();
}

inline void QmlExec::setArgs(const QStringList &args){
    if (m_args == args)
        return;

    m_args = args;
    emit argsChanged();
}

inline QmlExec::ChannelMode QmlExec::mode() const{
    return m_mode;
}

inline QmlStream *QmlExec::out() const{
    return m_out;
}

inline QmlStream *QmlExec::input() const{
    return m_in;
}

}// namespace

#endif // LVQMLEXEC_H
