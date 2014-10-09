#ifndef QLIVECVLOG_HPP
#define QLIVECVLOG_HPP

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

class QLiveCVLog : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit QLiveCVLog(QObject *parent = 0);
    ~QLiveCVLog();

    const QString data() const;
    void setData(const QString& data);

    static QLiveCVLog* m_instance;
    static QLiveCVLog* instance();

    static void logFunction(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

signals:
    void dataChanged();

public slots:
    void logMessage(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

private:
    QString     m_data;
    QString     m_logFilePath;
    QFile       m_logFile;
    QTextStream m_textStream;

};

inline const QString QLiveCVLog::data() const{
    return m_data;
}

inline void QLiveCVLog::setData(const QString& data){
    if ( m_data != data ){
        m_data = data;
        emit dataChanged();
    }
}

inline QLiveCVLog*QLiveCVLog::instance(){
    if ( !m_instance )
        m_instance = new QLiveCVLog;
    return m_instance;
}

#endif // QLIVECVLOG_HPP

