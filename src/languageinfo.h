/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVLANGUAGEINFO_H
#define LVLANGUAGEINFO_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/utf8.h"
#include "live/mlnode.h"

#include <vector>

namespace lv{ namespace el{

class MetaObject;

class LV_ELEMENTS_COMPILER_EXPORT EnumInfo{

public:
    EnumInfo(Utf8 name);
    ~EnumInfo();

    void addKey(Utf8 key, int value);
    size_t keyCount() const;
    Utf8 key(size_t index) const;

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);

    Utf8 name() { return m_name; }
private:
    Utf8 m_name;
    std::vector<Utf8> m_keys;
    std::vector<int>  m_values;
};

class LV_ELEMENTS_COMPILER_EXPORT TypeReference{

public:
    TypeReference(uint32_t language = 'u', const Utf8& name = "", const Utf8& path = "");

    static TypeReference split(const Utf8& typeId);
    Utf8 join() const;
    bool isEmpty() const;

    const Utf8& name() const;
    const Utf8& path() const;
    uint32_t language() const;
    Utf8 languageString() const;

    static Utf8 languageString(uint32_t languageId);
    static uint32_t languageId(const Utf8& languageString);
    static uint32_t languageId(char l0, char l1, char l2, char l3);

private:
    uint32_t m_language;
    Utf8     m_name;
    Utf8     m_path;
};

class LV_ELEMENTS_COMPILER_EXPORT FunctionInfo{

public:
    FunctionInfo(Utf8 name, Utf8 returnType = "");
    ~FunctionInfo();

    const Utf8& name() const;
    const Utf8& returnType() const;

    void addParameter(const Utf8& name, const Utf8& typeName);
    size_t parameterCount() const;
    const std::pair<Utf8, Utf8>& parameter(size_t index) const;

    static FunctionInfo extractFromDeclaration(const std::string& name, const std::string& declaration);

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    Utf8 m_name;
    Utf8 m_returnType;
    std::vector<std::pair<Utf8, Utf8>> m_parameters;
};

class LV_ELEMENTS_COMPILER_EXPORT PropertyInfo{

public:
    PropertyInfo(Utf8 name, Utf8 typeName = "");
    ~PropertyInfo();

    const Utf8& name() const;
    const Utf8& typeName() const;

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);

private:
    Utf8 m_name;
    Utf8 m_typeName;
};

class LV_ELEMENTS_COMPILER_EXPORT ImportInfo{

public:
    ImportInfo(const std::vector<Utf8>& segments, const Utf8& importAs = Utf8(), bool isRelative = false);

    bool isRelative() const;
    const Utf8& importAs() const;

    size_t totalSegments() const;
    const Utf8& segmentAt(size_t index) const;

    std::vector<std::string> segments() const;

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    bool              m_isRelative;
    std::vector<Utf8> m_segments;
    Utf8              m_importAs;
};

class LV_ELEMENTS_COMPILER_EXPORT TypeInfo{

public:
    typedef std::shared_ptr<TypeInfo>       Ptr;
    typedef std::shared_ptr<const TypeInfo> ConstPtr;

public:
    ~TypeInfo();

    static TypeInfo::Ptr create(Utf8 name, Utf8 inheritsName, bool isCreatable, bool isInstance);

    const Utf8& typeName() const;
    const Utf8& inheritsName() const;
    const Utf8& className() const;
    void setClassName(Utf8 className);
    bool isCreatable() const;
    bool isInstance() const;

    void setConstructor(const FunctionInfo& constructor);
    const FunctionInfo& getConstructor() const;

    size_t totalProperties() const;
    const PropertyInfo& propertyAt(size_t index) const;
    void addProperty(const PropertyInfo& propertyInfo);

    size_t totalFunctions() const;
    const FunctionInfo& functionAt(size_t index) const;
    void addFunction(const FunctionInfo& functionInfo);

    size_t totalMethods() const;
    const FunctionInfo& methodAt(size_t index) const;
    void addMethod(const FunctionInfo& functionInfo);

    size_t totalEvents() const;
    const FunctionInfo& eventAt(size_t index) const;
    void addEvent(const FunctionInfo& eventInfo);

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    TypeInfo(Utf8 name, Utf8 inheritsName, bool isCreatable, bool isInstance);

    Utf8                      m_typeName;
    Utf8                      m_className;
    Utf8                      m_inherits;
    std::vector<PropertyInfo> m_properties;
    std::vector<FunctionInfo> m_functions;
    std::vector<FunctionInfo> m_methods;
    std::vector<FunctionInfo> m_events;
    FunctionInfo              m_constructor;
    bool                      m_isCreatable;
    bool                      m_isInstance;
};

class LV_ELEMENTS_COMPILER_EXPORT InheritanceInfo{

public:
    InheritanceInfo(const std::vector<TypeInfo::Ptr>& types = std::vector<TypeInfo::Ptr>());
    ~InheritanceInfo();

    size_t totalTypes() const;
    const TypeInfo::Ptr& typeAt(size_t index) const;
    void addType(const TypeInfo::Ptr& type);

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    std::vector<TypeInfo::Ptr> m_types;
};

class LV_ELEMENTS_COMPILER_EXPORT DocumentInfo{

public:
    enum ScanStatus{
        NotParsed, Ready, Parsed
    };

    typedef std::shared_ptr<DocumentInfo>       Ptr;
    typedef std::shared_ptr<const DocumentInfo> ConstPtr;

public:
    ~DocumentInfo();

    static DocumentInfo::Ptr create(const std::vector<TypeInfo::Ptr>& types = std::vector<TypeInfo::Ptr>());

    size_t totalTypes() const;
    const TypeInfo::Ptr& typeAt(size_t index) const;
    void addType(const TypeInfo::Ptr& type);

    size_t totalImports() const;
    const ImportInfo& importAt(size_t index) const;
    void addImport(const ImportInfo& import);

    void updateScanStatus(ScanStatus status);
    ScanStatus scanStatus() const;

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    DocumentInfo(const std::vector<TypeInfo::Ptr>& types = std::vector<TypeInfo::Ptr>());

    ScanStatus                 m_status;
    std::vector<ImportInfo>    m_imports;
    std::vector<TypeInfo::Ptr> m_types;

};

class LV_ELEMENTS_COMPILER_EXPORT ModuleInfo{

public:
    enum ScanStatus{
        NotParsed, Ready, Parsed
    };

    typedef std::shared_ptr<ModuleInfo>       Ptr;
    typedef std::shared_ptr<const ModuleInfo> ConstPtr;

public:
    static ModuleInfo::Ptr create(
        Utf8 importUri, Utf8 path, const std::vector<TypeInfo::Ptr>& types = std::vector<TypeInfo::Ptr>());

    ~ModuleInfo();

    const Utf8& importUri() const;
    const Utf8& path() const;

    size_t totalTypes() const;
    TypeInfo::ConstPtr typeAt(size_t index) const;
    void addType(const TypeInfo::Ptr& type);

    void updateScanStatus(ScanStatus status);
    ScanStatus scanStatus() const { return m_scanStatus; }

    size_t totalUnresolvedDocuments() const;
    const DocumentInfo::Ptr& unresolvedDocumentAt(size_t index) const;
    void addUnresolvedDocument(const DocumentInfo::Ptr& document);

    size_t totalDependencies() const;
    const Utf8& dependencyAt(size_t index) const;
    void addDependency(const Utf8& dep);

    MLNode toMLNode() const;
    void fromMLNode(const MLNode& node);
private:
    ModuleInfo(Utf8 importUri, Utf8 path, const std::vector<TypeInfo::Ptr>& types = std::vector<TypeInfo::Ptr>());

    Utf8       m_importUri;
    Utf8       m_path;
    ScanStatus m_scanStatus;
    std::vector<Utf8>              m_dependencies;
    std::vector<DocumentInfo::Ptr> m_unresolvedDocuments;
    std::vector<TypeInfo::Ptr>     m_types;
};

} // namespace el

namespace ml{

//TODO: Add serialization, and deserialization

} // namespace ml

}// namespace lv, el

#endif // LVLANGUAGEINFO_H
