#ifndef QLIVECV_H
#define QLIVECV_H

#include <QString>

#define LIVECV_VERSION_MAJOR 1
#define LIVECV_VERSION_MINOR 2
#define LIVECV_VERSION_PATCH 0

class QGuiApplication;
class QQmlApplicationEngine;
class QCodeDocument;
class QUrl;

class QLiveCV{

public:
    QLiveCV(int argc, char *argv[]);
    ~QLiveCV();

    void loadLibrary(const QString& library);
    void loadQml(const QUrl& url);

    int versionMajor() const;
    int versionMinor() const;
    int versionPatch() const;
    QString versionString() const;

    int exec();

    const QString& dir() const;

private:
    QLiveCV(const QLiveCV&);
    QLiveCV& operator = (const QLiveCV&);

    void parseArguments();
    void solveImportPaths();

    QGuiApplication* m_app;
    QQmlApplicationEngine* m_engine;

    QCodeDocument* m_document;
    QString m_dir;

};

inline int QLiveCV::versionMajor() const{
    return LIVECV_VERSION_MAJOR;
}

inline int QLiveCV::versionMinor() const{
    return LIVECV_VERSION_MINOR;
}

inline int QLiveCV::versionPatch() const{
    return LIVECV_VERSION_PATCH;
}

inline QString QLiveCV::versionString() const{
    return
        QString::number(versionMajor()) + "." +
        QString::number(versionMinor()) + "." +
            QString::number(versionPatch());
}

inline const QString &QLiveCV::dir() const{
    return m_dir;
}

#endif // QLIVECV_H
