#ifndef QCODEDOCUMENT_HPP
#define QCODEDOCUMENT_HPP

#include <QQuickItem>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit QCodeDocument(QQuickItem *parent = 0);

public slots:
    QString openFile(const QUrl& file);
    void saveFile(const QUrl &file, const QString& content);
    
    const QString& path() const;
    void setPath(const QString& path);

    void setEngine(QQmlEngine* engine);

signals:
    void pathChanged();

private:
    QString     m_path;
    QString     m_openedFile;
    QQmlEngine* m_engine;
    QStringList m_importPaths;

};

inline const QString &QCodeDocument::path() const{
    return m_path;
}

inline void QCodeDocument::setPath(const QString &path){
    if ( m_path != path ){
        m_path = path;
        emit pathChanged();
    }
}

#endif // QCODEDOCUMENT_HPP
