#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>
#include "qlcveditorglobal.h"

namespace lcv{

class QProjectFile;
class Q_LCVEDITOR_EXPORT QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectFile* file    READ file    NOTIFY fileChanged)
    Q_PROPERTY(QString            content READ content NOTIFY contentChanged)

public:
    explicit QProjectDocument(QProjectFile* file, QObject *parent = 0);
    ~QProjectDocument();

    lcv::QProjectFile* file() const;

    const QString& content() const;

    void readContent();

public slots:
    void dumpContent(const QString& content);
    void save();

signals:
    void fileChanged();
    void contentChanged();

private:
    QProjectFile* m_file;
    QString       m_content;
};

inline QProjectFile *QProjectDocument::file() const{
    return m_file;
}

inline const QString &QProjectDocument::content() const{
    return m_content;
}

}// namespace

#endif // QPROJECTDOCUMENT_H
