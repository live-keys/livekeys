#ifndef QLIVECV_H
#define QLIVECV_H

#include <QString>
#include <QLibrary>

#define LIVECV_VERSION_MAJOR 1
#define LIVECV_VERSION_MINOR 2
#define LIVECV_VERSION_PATCH 0

class QGuiApplication;
class QQmlApplicationEngine;
class QCodeDocument;

class QUrl;

class QLiveCV{

public:
    QLiveCV(const QStringList &arguments);
    ~QLiveCV();

    void loadLibrary(const QString& library);
    void loadQml(const QUrl& url);

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();

    const QString& dir() const;

private:
    QLiveCV(const QLiveCV&);
    QLiveCV& operator = (const QLiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    QQmlApplicationEngine* m_engine;

    QCodeDocument* m_document;
    QString m_dir;
    QLibrary m_lcvlib;

};

inline int QLiveCV::versionMajor(){
    return LIVECV_VERSION_MAJOR;
}

inline int QLiveCV::versionMinor(){
    return LIVECV_VERSION_MINOR;
}

inline int QLiveCV::versionPatch(){
    return LIVECV_VERSION_PATCH;
}

inline QString QLiveCV::versionString(){
    return
        QString::number(versionMajor()) + "." +
        QString::number(versionMinor()) + "." +
            QString::number(versionPatch());
}

inline const QString &QLiveCV::dir() const{
    return m_dir;
}

#endif // QLIVECV_H
