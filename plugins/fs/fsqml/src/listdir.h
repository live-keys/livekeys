#ifndef LVLISTDIR_H
#define LVLISTDIR_H

#include <QObject>

namespace lv{
/// \private
class ListDir : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString path       READ path   WRITE setPath  NOTIFY pathChanged)
    Q_PROPERTY(int flags          READ flags  WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(QStringList output READ output NOTIFY outputChanged)
    Q_ENUMS(Flags)

public:
    enum Flags{
        NoFlags = 0,
        Subdirectories = 0x2,
        FollowSymlinks = 0x1
    };

public:
    ListDir(QObject* parent = nullptr);
    ~ListDir();

    const QString& path() const;
    void setPath(const QString& path);

    const QStringList& output() const;

    void setFlags(int flags);
    int flags() const;

    void process();

signals:
    void pathChanged();
    void outputChanged();

    void flagsChanged();

private:
    QString     m_path;
    QStringList m_output;
    int         m_flags;
};

inline const QString &ListDir::path() const{
    return m_path;
}

inline const QStringList &ListDir::output() const{
    return m_output;
}

inline int ListDir::flags() const{
    return m_flags;
}

inline void ListDir::setFlags(int flags){
    if (m_flags == flags)
        return;

    m_flags = flags;
    emit flagsChanged();

    process();
}

inline void ListDir::setPath(const QString &path){
    if (m_path == path)
        return;

    m_path = path;
    emit pathChanged();

    process();
}

}// namespace

#endif // LVLISTDIR_H
