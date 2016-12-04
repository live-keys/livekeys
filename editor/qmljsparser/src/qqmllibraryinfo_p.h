#ifndef QQMLLIBRARYINFO_H
#define QQMLLIBRARYINFO_H

#include "languageutils/componentversion.h"
#include "qmljs/qmljsdocument.h"

#include <QMap>

namespace lcv{

class QQmlLibraryInfo{

public:
    class ExportVersion{
    public:
        ExportVersion()
            : versionMajor(-1)
            , versionMinor(-1)
        {}
        ExportVersion(int vma, int vmi, LanguageUtils::FakeMetaObject::ConstPtr o)
            : versionMajor(vma)
            , versionMinor(vmi)
            , object(o)
        {}
        bool isValid() const{ return versionMinor != -1 && versionMajor != -1; }

        int versionMajor;
        int versionMinor;
        LanguageUtils::FakeMetaObject::ConstPtr object;
    };

    class Export{
    public:
        QList<ExportVersion> versions;
    };


public:
    typedef QSharedPointer<QQmlLibraryInfo>       Ptr;
    typedef QSharedPointer<const QQmlLibraryInfo> ConstPtr;

public:
    static Ptr create();
    static Ptr create(const QmlDirParser& parser);

    ~QQmlLibraryInfo();

    QmlJS::LibraryInfo& data(){ return m_data; }
    const QmlJS::LibraryInfo& data() const{ return m_data; }
    QMap<QString, Export>& exports(){ return m_exports; }

    void updateExports();

    void listExports(QStringList* exports);
    ExportVersion findExport(const QString& name);
    ExportVersion findExportByClassName(const QString& name);
    LanguageUtils::FakeMetaObject::ConstPtr findObjectByClassName(const QString& name);

    void setDependencies(const QStringList &paths);
    const QList<QString>& dependencyPaths() const;

private:
    QQmlLibraryInfo();
    QQmlLibraryInfo(const QmlDirParser& parser);

private:
    void addExport();

    QMap<QString, Export> m_exports;
    QmlJS::LibraryInfo    m_data;
    QList<QString>        m_dependencyPaths;
};

inline QQmlLibraryInfo::Ptr QQmlLibraryInfo::create(){
    return QQmlLibraryInfo::Ptr(new QQmlLibraryInfo);
}

inline QQmlLibraryInfo::Ptr QQmlLibraryInfo::create(const QmlDirParser &parser){
    return QQmlLibraryInfo::Ptr(new QQmlLibraryInfo(parser));
}

inline const QList<QString> &QQmlLibraryInfo::dependencyPaths() const{
    return m_dependencyPaths;
}

}// namespace

#endif // QQMLLIBRARYINFO_H
