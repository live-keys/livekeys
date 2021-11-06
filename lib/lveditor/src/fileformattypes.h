#ifndef LVFILEFORMATTYPES_H
#define LVFILEFORMATTYPES_H

#include <QObject>
#include <QJSValue>

namespace lv{

class FileFormatTypes : public QObject{

    Q_OBJECT

private:
    class Entry{
    public:
        QString matchName;
        QString matchExtension;
        QString matchWildCard;
        QString type;

        bool isMatch(const QString& fileName);
    };

public:
    explicit FileFormatTypes(QObject *parent = nullptr);
    ~FileFormatTypes();

    void add(const QJSValue& entries);
    void addEntry(const QString& type, const QString& name = "", const QString& extension = "", const QString& match = "");

public slots:
    QString find(const QString& path);

private:
    QList<Entry> m_entries;
};

}// namespace

#endif // FILEFORMATTYPES_H
