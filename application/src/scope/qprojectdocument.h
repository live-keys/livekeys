#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>

namespace lcv{

class QProjectFile;
class QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(QProjectFile* file    READ file    NOTIFY fileChanged)
    Q_PROPERTY(QString       content READ content NOTIFY contentChanged)

public:
    explicit QProjectDocument(QProjectFile* file, QObject *parent = 0);
    ~QProjectDocument();

    QProjectFile* file() const;

    const QString& content() const;

    void readContent();

public slots:
    void dumpContent(const QString& content);

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
