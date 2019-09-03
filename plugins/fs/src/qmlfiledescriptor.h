#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <QObject>

class QFile;

namespace lv {

class QmlFileDescriptor : public QObject
{
    Q_OBJECT
public:
    QmlFileDescriptor(QObject* parent=nullptr) : QObject(parent), file(nullptr) {}
    QmlFileDescriptor(QFile* file, QObject *parent = nullptr);
    QmlFileDescriptor(const QmlFileDescriptor& other);

    void operator=(const QmlFileDescriptor& other);
    ~QmlFileDescriptor();
public slots:
    bool isValid();
    void setFile(QFile* f) { file = f; }
    void close();
    bool seek(int pos);
    qint64 write(QByteArray content);
    qint64 writeString(QString text);
    QByteArray read(qint64 numOfBytes);
private:
    QFile* file;
};

}

#endif // FILEDESCRIPTOR_H
