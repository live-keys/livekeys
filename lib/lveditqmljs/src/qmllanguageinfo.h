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


// QmlTypeInfo
// ---------------------------------------------


class LV_EDITQMLJS_EXPORT QmlTypeInfo{

    friend class QmlTypeInfoPrivate;

public:
    QmlTypeInfo();
    QmlTypeInfo(const QmlTypeInfo& other);
    ~QmlTypeInfo();

    QmlTypeInfo& operator = (const QmlTypeInfo& other);

    void setExportType(const QmlTypeReference& exportType);

    const QmlTypeReference& exportType() const;
    const QmlTypeReference& classType() const;
    const QmlTypeReference& inherits() const;
    const QmlTypeReference& prefereredType() const;
    const QString& document() const;

    int totalProperties() const;
    QmlPropertyInfo propertyAt(int index) const;
    QmlPropertyInfo propertyAt(const QString& name) const;


    int totalFunctions() const;
    QmlFunctionInfo functionAt(int index) const;
    QmlFunctionInfo functionAt(const QString& name) const;

    bool isValid() const;
    QString toString() const;

    bool isDeclaredInQml() const;
    bool isDeclaredInCpp() const;

    static bool isObject(const QString& declarationType);
    static QString toQmlPrimitive(const QString& cppPrimitive);
    static QString typeDefaultValue(const QString& typeString);

private:
    QmlTypeReference m_exportType;
    QmlTypeReference m_classType;
    QmlTypeReference m_inherits;
    QString          m_document;
    QmlTypeInfoPrivate* m_d;
};

// QmlInheritanceInfo
// ---------------------------------------------

class QmlInheritanceInfo{
public:
    QString toString() const;
    void join(const QmlInheritanceInfo& path);
    void append(const QmlTypeInfo& tr);
    bool isEmpty() const;

    QmlTypeReference languageType() const;
public:
    QList<QmlTypeInfo> nodes;
};

}// namespace

#endif // LVQMLLANGUAGEINFO_H
