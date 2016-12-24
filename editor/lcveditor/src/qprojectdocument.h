#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include "qlcveditorglobal.h"

namespace lcv{

class QProject;
class QProjectFile;
class Q_LCVEDITOR_EXPORT QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectFile* file    READ file        CONSTANT)
    Q_PROPERTY(QString            content READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored           READ isMonitored NOTIFY isMonitoredChanged)
    Q_ENUMS(OpenMode)

public:
    enum OpenMode{
        Edit = 0,
        Monitor,
        EditIfNotOpen
    };

public:
    explicit QProjectDocument(QProjectFile* file, bool isMonitored, QProject *parent);
    ~QProjectDocument();

    lcv::QProjectFile* file() const;
    bool isMonitored() const;
    void setIsMonitored(bool monitored);

    const QString& content() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    QProject* parentAsProject();

public slots:
    void dumpContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    void contentChanged();
    void isMonitoredChanged();

private:
    QProjectFile* m_file;
    QString       m_content;
    QDateTime     m_lastModified;
    bool          m_isMonitored;
};

inline QProjectFile *QProjectDocument::file() const{
    return m_file;
}

inline bool QProjectDocument::isMonitored() const{
    return m_isMonitored;
}

inline void QProjectDocument::setIsMonitored(bool monitored){
    if ( m_isMonitored != monitored ){
        m_isMonitored = monitored;
        emit isMonitoredChanged();
    }
}

inline const QString &QProjectDocument::content() const{
    return m_content;
}

inline const QDateTime &QProjectDocument::lastModified() const{
    return m_lastModified;
}

inline void QProjectDocument::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

}// namespace

#endif // QPROJECTDOCUMENT_H
