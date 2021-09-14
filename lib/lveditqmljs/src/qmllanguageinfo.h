/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVQMLLANGUAGEINFO_H
#define LVQMLLANGUAGEINFO_H

#include "live/lveditqmljsglobal.h"

#include <QString>
#include <QMap>
#include <QSharedPointer>

class QmlDirParser;

namespace lv{

// QmlTypeReference
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlTypeReference{

public:
    enum Language{
        Unknown = 0,
        Qml = 1,
        Cpp = 2
    };

public:
    QmlTypeReference(Language lang = QmlTypeReference::Unknown, const QString& name = "", const QString& path = "");

    static QmlTypeReference split(const QString& typeId);
    QString join() const;

    const QString& name() const;
    const QString& path() const;
    Language language() const;
    QString languageString() const;

    bool isEmpty() const;

private:
    Language m_language;
    QString  m_path;
    QString  m_name;
};

// QmlEnumInfo
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlEnumInfo{

public:
    QString        name;
    QList<QString> keys;
    QList<int>     values;
};

// QmlFunctionInfo
// ---------------------------------------------

class QmlTypeInfoPrivate;

class LV_EDITQMLJS_EXPORT QmlFunctionInfo{

public:
    enum{
        Private,
        Protected,
        Public
    };

    enum{
        Signal,
        Slot,
        SlotGenerated,
        Method
    };

public:
    friend class QmlTypeInfoPrivate;

public:
    QmlFunctionInfo();

    QString definition() const;
    QString toString() const;
    bool isValid() const;

    void addParameter(const QString& str, const QmlTypeReference& tr);

public:
    QString          name;
    int              accessType;
    int              functionType;
    QmlTypeReference objectType;
    QmlTypeReference returnType;
    QList<QPair<QString, QmlTypeReference>> parameters;
};

// QmlPropertyInfo
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlPropertyInfo{

public:
    QmlPropertyInfo();
    QmlPropertyInfo(
        const QString& name,
        const QmlTypeReference& typeName,
        const QmlTypeReference& objectType = QmlTypeReference());

    bool isValid() const;

public:
    bool             isWritable;
    bool             isList;
    bool             isPointer;
    QString          name;
    QmlTypeReference typeName;
    QmlTypeReference objectType;
};

// QmlDocumentReference
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlDocumentReference{

public:
    bool isValid() const{ return !path.isEmpty(); }

    QMap<QString, QString>          watchers;
    QString                         path;
    QList<QPair<QString, QString> > dependencies;
};

// QmlTypeInfo
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlTypeInfo{

    friend class QmlTypeInfoPrivate;

public:
    typedef QSharedPointer<QmlTypeInfo>       Ptr;
    typedef QSharedPointer<const QmlTypeInfo> ConstPtr;

public:
    static Ptr create();
    static Ptr clone(const QmlTypeInfo::ConstPtr& other);

    ~QmlTypeInfo();

    void setExportType(const QmlTypeReference& exportType);
    void setClassType(const QmlTypeReference& classType);
    void setInheritanceType(const QmlTypeReference& inheritsType);

    const QmlTypeReference& exportType() const;
    const QmlTypeReference& classType() const;
    const QmlTypeReference& inherits() const;
    const QmlTypeReference& prefereredType() const;
    const QmlDocumentReference& document() const;

    void setDocument(const QmlDocumentReference& doc);

    int totalProperties() const;
    QmlPropertyInfo propertyAt(int index) const;
    QmlPropertyInfo propertyAt(const QString& name) const;
    void appendProperty(const QmlPropertyInfo& prop);
    void updateProperty(int index, const QmlPropertyInfo& prop);


    int totalFunctions() const;
    QmlFunctionInfo functionAt(int index) const;
    QmlFunctionInfo functionAt(const QString& name) const;
    void appendFunction(const QmlFunctionInfo& function);

    int totalEnums() const;
    QmlEnumInfo enumAt(int index) const;
    QmlEnumInfo enumAt(const QString& name) const;

    bool isValid() const;
    QString toString() const;

    bool isDeclaredInQml() const;
    bool isDeclaredInCpp() const;

    const QString& defaultProperty() const;
    bool isSingleton() const;
    bool isComposite() const;
    bool isCreatable() const;

    static bool isObject(const QString& declarationType);
    static bool isQmlBasicType(const QString& declarationType);
    static QmlTypeReference toQmlPrimitive(const QmlTypeReference &cppPrimitive);
    static QString typeDefaultValueAsText(const QString& typeString);

private:
    QmlTypeInfo();
    QmlTypeInfo(const QmlTypeInfo&);
    QmlTypeInfo& operator=(const QmlTypeInfo&);

    QmlTypeReference       m_exportType;
    QmlTypeReference       m_classType;
    QmlTypeReference       m_inherits;
    QmlDocumentReference   m_document;
    QString                m_defaultProperty;
    bool                   m_isSingleton;
    bool                   m_isCreatable;
    bool                   m_isComposite;

    QList<QmlPropertyInfo> m_properties;
    QList<QmlFunctionInfo> m_methods;
    QList<QmlEnumInfo>     m_enums;
};

// QmlInheritanceInfo
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlInheritanceInfo{
public:
    QString toString() const;
    void join(const QmlInheritanceInfo& path);
    void append(const QmlTypeInfo::Ptr& tr);
    bool isEmpty() const;
    QmlPropertyInfo defaultProperty() const;

    QmlTypeReference languageType() const;
public:
    QList<QmlTypeInfo::Ptr> nodes;
};

// QmlLibraryInfo
// ---------------------------------------------

class LV_EDITQMLJS_EXPORT QmlLibraryInfo{

public:
    enum ScanStatus{
        NotScanned = 0,
        WaitingOnProcess,
        ScanError,
        NoPrototypeLink,
        RequiresDependency,
        Done
    };

public:
    typedef QSharedPointer<QmlLibraryInfo>       Ptr;
    typedef QSharedPointer<const QmlLibraryInfo> ConstPtr;

public:
    static Ptr create(const QString& uri);
    static Ptr create(const QmlDirParser& parser);
    static Ptr clone(const QmlLibraryInfo::ConstPtr& linfo);

    ~QmlLibraryInfo();

    const QMap<QString, QmlTypeInfo::Ptr>& exports() const;

    QStringList listExports() const;
    QmlTypeInfo::Ptr typeInfoByName(const QString& exportName);
    QmlTypeInfo::Ptr typeInfoByClassName(const QString& className);
    QmlTypeInfo::Ptr typeInfo(const QString& name, QmlTypeReference::Language language);

    void updateImportInfo(int versionMajor, int versionMinor);

    ScanStatus status() const;
    void setStatus(ScanStatus status);

    QString statusString() const;

    void setPath(const QString& path);

    const QString& path() const;
    const QString& uri() const;
    QStringList uriSegments() const;

    QString importStatement() const;

    int importVersionMinor() const;
    int importVersionMajor() const;

    void addType(const QmlTypeInfo::Ptr& typeExport);
    void addDependency(const QString& dependency);
    void addDependencies(const QStringList& dependencies);
    const QList<QString>& dependencies() const;

    void updateUri(const QString& uri);

    QString toString() const;

private:
    QmlLibraryInfo();
    QmlLibraryInfo(const QmlDirParser& parser);

private:
    QMap<QString, QmlTypeInfo::Ptr> m_exports;
    QList<QmlTypeInfo::Ptr>         m_internals;
    QList<QString>                  m_dependencies;

    QString               m_path;
    QString               m_uri;
    int                   m_importVersionMajor;
    int                   m_importVersionMinor;
    ScanStatus            m_status;
};

}// namespace

#endif // LVQMLLANGUAGEINFO_H
