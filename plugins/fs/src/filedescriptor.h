#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <QObject>

class QFile;

namespace lv {

class FileDescriptor : public QObject
{
    Q_OBJECT
public:
    FileDescriptor(QObject* parent=nullptr) : QObject(parent), file(nullptr) {}
    FileDescriptor(QFile* file, QObject *parent = nullptr);
    FileDescriptor(const FileDescriptor& other);

    void operator=(const FileDescriptor& other);
    ~FileDescriptor();
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

Q_DECLARE_METATYPE(lv::FileDescriptor)

#endif // FILEDESCRIPTOR_H
