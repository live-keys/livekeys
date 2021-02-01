#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class FileIO : public QObject{

    Q_OBJECT

public:
    explicit FileIO(QObject *parent = nullptr);
    void setEngine(QQmlEngine* engine);

    static QObject* provider(QQmlEngine *engine, QJSEngine*);

public slots:
    QByteArray readUrl(const QUrl& url);
    QByteArray read(const QString& path);
    void write(const QString& path, const QString &data);
    void writeUrl(const QUrl& path, const QString& data);


private:
    QQmlEngine* m_engine;
};

#endif // FILEIO_H
