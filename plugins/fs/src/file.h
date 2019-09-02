#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFile>
#include <QJSValue>
#include "filedescriptor.h"
#include <QVariant>

namespace lv {

class File : public QObject
{
    Q_OBJECT
    Q_ENUMS(Flags)
public:
    enum Flags {
        NotOpen = QFile::NotOpen,
        ReadOnly = QFile::ReadOnly,
        WriteOnly = QFile::WriteOnly,
        ReadWrite = QFile::ReadWrite,
        Append = QFile::Append,
        Truncate = QFile::Truncate,
        Text = QFile::Text,
        Unbuffered = QFile::Unbuffered,
        NewOnly = QFile::NewOnly,
        ExistingOnly = QFile::ExistingOnly
    };

    explicit File(QObject *parent = nullptr);

public slots:
    bool remove(QJSValue path);
    bool rename(QJSValue old, QJSValue nu);
    qint64 size(QJSValue path);
    lv::FileDescriptor open(QJSValue path, Flags flags);

};

}

#endif // FILE_H
