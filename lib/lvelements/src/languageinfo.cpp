#include "languageinfo.h"
#include "live/elements/metaobject.h"

namespace lv{ namespace el{


// EnumInfo
// ------------------------------------------------------------

EnumInfo::EnumInfo(Utf8 name)
    : m_name(name)
{
}

EnumInfo::~EnumInfo(){
}

void EnumInfo::addKey(Utf8 key, int value){
    m_keys.push_back(key);
    m_values.push_back(value);
}

size_t EnumInfo::keyCount() const{
    return m_keys.size();
}

Utf8 EnumInfo::key(size_t index) const{
    return m_keys.at(index);
}

MLNode EnumInfo::toMLNode() const
{
    MLNode result(MLNode::Object);
    MLNode rhs(MLNode::Object);

    int count = m_keys.size();
    for (int i=0; i<count; ++i)
    {
        rhs[m_keys.at(i).data()] = m_values.at(i);
    }

    result[m_name.data()] = rhs;

    return result;
}

void EnumInfo::fromMLNode(const MLNode &node)
{
    auto nit = node.begin();
    m_name = nit.key();
    MLNode values = nit.value();

    for (auto it = values.begin(); it != values.end(); ++it)
    {
        m_keys.push_back(it.key());
        m_values.push_back(it.value().asInt());
    }

}

// Typename
// ------------------------------------------------------------

TypeReference::TypeReference(uint32_t language, const Utf8 &name, const Utf8 &path)
    : m_language(language)
    , m_name(name)
    , m_path(path)
{
}

TypeReference TypeReference::split(const Utf8 &typeId){
    size_t languageSplit = typeId.find('/');
    size_t nameSplit     = typeId.findLast('#');

    if ( languageSplit == std::string::npos ){
        if ( nameSplit == std::string::npos ){
            return TypeReference('u', typeId);
        } else {
            return TypeReference('u', typeId.substr(0, nameSplit), typeId.data().substr(nameSplit + 1));
        }
    } else {
        uint32_t language = TypeReference::languageId(typeId.substr(0, languageSplit));
        if ( nameSplit == std::string::npos ){
            return TypeReference(language, typeId.data().substr(languageSplit + 1));
        } else {
            return TypeReference(language, typeId.data().substr(nameSplit + 1), typeId.substr(languageSplit + 1, nameSplit - languageSplit - 1));
        }
    }
}

Utf8 TypeReference::join() const{
    return languageString() + "/" + m_path + "#" + m_name;
}

bool TypeReference::isEmpty() const{
    return m_name.length() == 0;
}

const Utf8 &TypeReference::name() const{
    return m_name;
}

const Utf8 &TypeReference::path() const{
    return m_path;
}

uint32_t TypeReference::language() const{
    return m_language;
}

Utf8 TypeReference::languageString() const{
    return languageString(m_language);
}

Utf8 TypeReference::languageString(uint32_t languageId){
    std::string s;

    char c;
    c = static_cast<char>((languageId >> 24) & 0xFF);
    if ( c )
        s.push_back(c);
    c = static_cast<char>((languageId >> 16) & 0xFF);
    if ( c )
        s.push_back(c);
    c = static_cast<char>((languageId >> 8) & 0xFF);
    if ( c )
        s.push_back(c);
    c = static_cast<char>(languageId & 0xFF);
    if ( c )
        s.push_back(c);
    return s;
}

uint32_t TypeReference::languageId(const Utf8 &languageString){
    if ( languageString.length() == 4 ){
        return languageId(languageString[0], languageString[1], languageString[2], languageString[3]);
    } else if ( languageString.length() == 3 ){
        return languageId(languageString[0], languageString[1], languageString[2], 0);
    } else if ( languageString.length() == 2 ){
        return languageId(languageString[0], languageString[1], 0, 0);
    } else if ( languageString.length() == 1 ){
        return languageId(languageString[0], 0, 0, 0);
    }
    return 0;
}

uint32_t TypeReference::languageId(char l0, char l1, char l2, char l3){
    return
        static_cast<uint32_t>(l0 << 24) | static_cast<uint32_t>(l1 << 16) |
        static_cast<uint32_t>(l2 << 8)  | static_cast<uint32_t>(l3);

}

// FunctionInfo
// ------------------------------------------------------------

FunctionInfo::FunctionInfo(Utf8 name, Utf8 returnType)
    : m_name(name)
    , m_returnType(returnType)
{
}

FunctionInfo::~FunctionInfo(){
}

const Utf8 &FunctionInfo::name() const{
    return m_name;
}

const Utf8 &FunctionInfo::returnType() const{
    return m_returnType;
}

void FunctionInfo::addParameter(const Utf8 &name, const Utf8 &typeName){
    m_parameters.push_back(std::make_pair(name, typeName));
}

size_t FunctionInfo::parameterCount() const{
    return m_parameters.size();
}

const std::pair<Utf8, Utf8> &FunctionInfo::parameter(size_t index) const{
    return m_parameters.at(index);
}

FunctionInfo FunctionInfo::extractFromDeclaration(const std::string& name, const std::string &declaration){
    size_t argStart = declaration.find('(');
    size_t argEnd   = declaration.find(')');
    if ( argStart == std::string::npos || argEnd == std::string::npos )
        return FunctionInfo(name);

    std::string returnType = declaration.substr(0, argStart);

    FunctionInfo fi(name, returnType.empty() ? "" : "cpp/" + returnType);

    const char* str = declaration.c_str() + argStart + 1;

    do{
        const char *begin = str;

        while(*str != ',' && *str != ')' && *str)
            str++;

        if ( begin != str ){
            fi.addParameter("", "cpp/" + std::string(begin, str));
        }

        if ( *str == ')' )
            break;
    } while (0 != *str++);

    return fi;
}

MLNode FunctionInfo::toMLNode() const
{
    MLNode result(MLNode::Object);
    MLNode rhs(MLNode::Object);

    rhs["return_type"] = m_returnType.data();
    MLNode params(MLNode::Array);
    for (int i=0; i<m_parameters.size(); ++i)
    {
        params.append(MLNode({{m_parameters[i].first.data(), m_parameters[i].second.data()}}));
    }

    rhs["parameters"] = params;
    result[m_name.data()] = rhs;

    return result;
}

void FunctionInfo::fromMLNode(const MLNode &node)
{
    auto nit = node.begin();
    m_name = nit.key();
    MLNode rest = nit.value();
    m_returnType = rest["return_type"].asString();
    MLNode arr = rest["parameters"];
    MLNode::ArrayType arrnode = arr.asArray();
    for (auto it=arrnode.begin(); it != arrnode.end(); ++it)
    {
        Utf8 name = it->begin().key();
        Utf8 type = it->begin().value().asString();
        m_parameters.push_back(std::make_pair(name, type));
    }
}

// PropertyInfo
// ------------------------------------------------------------

PropertyInfo::PropertyInfo(Utf8 name, Utf8 typeName)
    : m_name(name)
    , m_typeName(typeName)
{
}

PropertyInfo::~PropertyInfo(){
}

const Utf8 &PropertyInfo::name() const{
    return m_name;
}

const Utf8 &PropertyInfo::typeName() const{
    return m_typeName;
}

MLNode PropertyInfo::toMLNode() const
{
    return {{m_name.data(), m_typeName.data()}};
}

void PropertyInfo::fromMLNode(const MLNode &node)
{
    auto nit = node.begin();

    m_name = nit.key();
    m_typeName = nit.value().asString();
}

// TypeInfo
// ------------------------------------------------------------

TypeInfo::TypeInfo(Utf8 name, Utf8 inheritsName, bool isCreatable, bool isInstance)
    : m_typeName(name)
    , m_inherits(inheritsName)
    , m_constructor("")
    , m_isCreatable(isCreatable)
    , m_isInstance(isInstance)
{
}

TypeInfo::~TypeInfo(){
}

TypeInfo::Ptr TypeInfo::create(Utf8 name, Utf8 inheritsName, bool isCreatable, bool isInstance){
    return TypeInfo::Ptr(new TypeInfo(name, inheritsName, isCreatable, isInstance));
}

const Utf8 &TypeInfo::typeName() const{
    return m_typeName;
}

const Utf8 &TypeInfo::inheritsName() const{
    return m_inherits;
}

const Utf8 &TypeInfo::className() const{
    return m_className;
}

void TypeInfo::setClassName(Utf8 className){
    m_className = className;
}

bool TypeInfo::isCreatable() const{
    return m_isCreatable;
}

bool TypeInfo::isInstance() const{
    return m_isInstance;
}

void TypeInfo::setConstructor(const FunctionInfo &constructor){
    m_constructor = constructor;
}

const FunctionInfo &TypeInfo::getConstructor() const{
    return m_constructor;
}

size_t TypeInfo::totalProperties() const{
    return m_properties.size();
}

const PropertyInfo &TypeInfo::propertyAt(size_t index) const{
    return m_properties.at(index);
}

void TypeInfo::addProperty(const PropertyInfo &propertyInfo){
    m_properties.push_back(propertyInfo);
}

size_t TypeInfo::totalFunctions() const{
    return m_functions.size();
}

const FunctionInfo &TypeInfo::functionAt(size_t index) const{
    return m_functions.at(index);
}

void TypeInfo::addFunction(const FunctionInfo &fnInfo){
    m_functions.push_back(fnInfo);
}

size_t TypeInfo::totalMethods() const{
    return m_methods.size();
}

const FunctionInfo &TypeInfo::methodAt(size_t index) const{
    return m_methods.at(index);
}

void TypeInfo::addMethod(const FunctionInfo &functionInfo){
    m_methods.push_back(functionInfo);
}

size_t TypeInfo::totalEvents() const{
    return m_events.size();
}

const FunctionInfo &TypeInfo::eventAt(size_t index) const{
    return m_events.at(index);
}

void TypeInfo::addEvent(const FunctionInfo &eventInfo){
    m_events.push_back(eventInfo);
}

TypeInfo::Ptr TypeInfo::extract(const MetaObject &mo, const Utf8 &uri, bool isInstance, bool isCreatable){
    std::string name = uri.length() == 0 ? "u/" + mo.name() : "lv/" + uri.data() + "#" + mo.name();

    TypeInfo::Ptr ti = TypeInfo::create(name, mo.base() ? "cpp/" + mo.base()->fullName() : "", isInstance, isCreatable);
    ti->setClassName("cpp/" + mo.fullName());

    // Constructor
    if ( mo.constructor() ){
        Constructor* c = mo.constructor();
        ti->setConstructor(FunctionInfo::extractFromDeclaration("constructor", c->getDeclaration()));
    }

    // Properties
    for ( auto propIt = mo.ownPropertiesBegin(); propIt != mo.ownPropertiesEnd(); ++propIt ){
        Property* p = propIt->second;
        ti->addProperty(PropertyInfo(p->name(), "cpp/" + p->type()));
    }

    // Methods
    for ( auto funcIt = mo.ownMethodsBegin(); funcIt != mo.ownMethodsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        ti->addMethod(FunctionInfo::extractFromDeclaration(funcIt->first, f->getDeclaration()));
    }

    // Functions
    for ( auto funcIt = mo.functionsBegin(); funcIt != mo.functionsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        ti->addFunction(FunctionInfo::extractFromDeclaration(funcIt->first, f->getDeclaration()));
    }

    // Events
    for ( auto eventIt = mo.ownEventsBegin(); eventIt != mo.ownEventsEnd(); ++eventIt ){
        Function* f = eventIt->second;
        ti->addEvent(FunctionInfo::extractFromDeclaration(eventIt->first, f->getDeclaration()));
    }

    return ti;
}

MLNode TypeInfo::toMLNode() const
{
    MLNode result(MLNode::Object);
    result["is_creatable"] = m_isCreatable;
    result["is_instance"] = m_isInstance;
    result["type_name"] = m_typeName.data();
    result["class_name"] = m_className.data();
    if (m_inherits != "")
        result["inherits"] = m_inherits.data();

    result["constructor"] = m_constructor.toMLNode();

    MLNode properties(MLNode::Array);
    for (int i = 0; i<m_properties.size();++i)
        properties.append(m_properties[i].toMLNode());
    result["properties"] = properties;

    MLNode functions(MLNode::Array);
    for (int i = 0; i<m_functions.size();++i)
        functions.append(m_functions[i].toMLNode());
    result["functions"] = functions;

    MLNode methods(MLNode::Array);
    for (int i = 0; i<m_methods.size();++i)
        methods.append(m_methods[i].toMLNode());
    result["methods"] = methods;

    MLNode events(MLNode::Array);
    for (int i = 0; i<m_events.size();++i)
        events.append(m_events[i].toMLNode());
    result["events"] = events;

    return result;
}

void TypeInfo::fromMLNode(const MLNode &node)
{
    MLNode::ObjectType object = node.asObject();

    m_typeName = object.at("type_name").asString();
    m_className = object.at("class_name").asString();
    if (object.find("inherits") != object.end())
        m_inherits = object.at("inherits").asString();
    m_isInstance = object.at("is_instance").asBool();
    m_isCreatable = object.at("is_creatable").asBool();

    MLNode prop = object.at("properties");
    MLNode::ArrayType prop_array = prop.asArray();
    for (int i = 0; i < prop_array.size(); ++i)
    {
        MLNode property = prop_array[i];
        PropertyInfo pi("");
        pi.fromMLNode(property);
        m_properties.push_back(pi);
    }

    MLNode func = object.at("functions");
    MLNode::ArrayType func_array = func.asArray();
    for (int i = 0; i < func_array.size(); ++i)
    {
        MLNode function = func_array[i];
        FunctionInfo fi({});
        fi.fromMLNode(function);
        m_functions.push_back(fi);
    }

    MLNode meth = object.at("methods");
    MLNode::ArrayType meth_array = meth.asArray();
    for (int i = 0; i < meth_array.size(); ++i)
    {
        MLNode method = meth_array[i];
        FunctionInfo fi({});
        fi.fromMLNode(method);
        m_methods.push_back(fi);
    }

    MLNode ev = object.at("events");
    MLNode::ArrayType ev_array = ev.asArray();
    for (int i = 0; i < ev_array.size(); ++i)
    {
        MLNode event = ev_array[i];
        FunctionInfo fi({});
        fi.fromMLNode(event);
        m_events.push_back(fi);
    }
}


// InheritanceInfo
// ------------------------------------------------------------

InheritanceInfo::InheritanceInfo(const std::vector<TypeInfo::Ptr> &types)
    : m_types(types)
{
}

InheritanceInfo::~InheritanceInfo(){
}

size_t InheritanceInfo::totalTypes() const{
    return  m_types.size();
}

const TypeInfo::Ptr &InheritanceInfo::typeAt(size_t index) const{
    return m_types.at(index);
}

void InheritanceInfo::addType(const TypeInfo::Ptr &type){
    m_types.push_back(type);
}

MLNode InheritanceInfo::toMLNode() const
{
    MLNode result(MLNode::Array);

    for (int i = 0; i < m_types.size();++i)
    {
        result.append(m_types[i]->toMLNode());
    }
    return result;
}

void InheritanceInfo::fromMLNode(const MLNode &node)
{
    MLNode::ArrayType arr = node.asArray();
    for (int i = 0; i<arr.size();++i)
    {
        TypeInfo::Ptr ti = TypeInfo::create("","", false, false);
        ti->fromMLNode(arr.at(i));
        m_types.push_back(ti);
    }
}


// DocumentInfo
// ------------------------------------------------------------

DocumentInfo::DocumentInfo(const std::vector<TypeInfo::Ptr> &types)
    : m_status(NotParsed)
    , m_types(types)
{
}

DocumentInfo::~DocumentInfo(){
}

DocumentInfo::Ptr DocumentInfo::create(const std::vector<TypeInfo::Ptr> &types){
    return DocumentInfo::Ptr(new DocumentInfo(types));
}

size_t DocumentInfo::totalTypes() const{
    return m_types.size();
}

const TypeInfo::Ptr &DocumentInfo::typeAt(size_t index) const{
    return m_types.at(index);
}

void DocumentInfo::addType(const TypeInfo::Ptr &type){
    m_types.push_back(type);
}

size_t DocumentInfo::totalImports() const{
    return m_imports.size();
}

const ImportInfo &DocumentInfo::importAt(size_t index) const{
    return m_imports.at(index);
}

void DocumentInfo::addImport(const ImportInfo &import){
    m_imports.push_back(import);
}

void DocumentInfo::updateScanStatus(DocumentInfo::ScanStatus status){
    m_status = status;
}

DocumentInfo::ScanStatus DocumentInfo::scanStatus() const{
    return m_status;
}

MLNode DocumentInfo::toMLNode() const
{
    MLNode result(MLNode::Object);
    result["status"] = m_status;

    MLNode imports(MLNode::Array);
    for (int i = 0; i < m_imports.size(); ++i)
    {
        imports.append(m_imports[i].toMLNode());
    }
    result["imports"] = imports;

    MLNode types(MLNode::Array);
    for (int i =0; i<m_types.size();++i)
    {
        types.append(m_types[i]->toMLNode());
    }
    result["types"] = types;

    return result;
}

void DocumentInfo::fromMLNode(const MLNode &node)
{
    MLNode::ObjectType object = node.asObject();
    m_status = static_cast<DocumentInfo::ScanStatus>(object.at("status").asInt());

    auto imports = object.at("imports").asArray();
    for (int i = 0; i < imports.size(); ++i)
    {
        ImportInfo ii({});
        ii.fromMLNode(imports.at(i));
        m_imports.push_back(ii);
    }

    auto types = object.at("types").asArray();
    for (int i =0; i<types.size(); ++i)
    {
        TypeInfo::Ptr ti = TypeInfo::create("", "", false, false);
        ti->fromMLNode(types.at(i));
        m_types.push_back(ti);
    }
}


// ModuleInfo
// ------------------------------------------------------------

ModuleInfo::ModuleInfo(Utf8 importUri, Utf8 path, const std::vector<TypeInfo::Ptr> &types)
    : m_importUri(importUri)
    , m_path(path)
    , m_scanStatus(NotParsed)
    , m_types(types)
{
}

ModuleInfo::Ptr ModuleInfo::create(Utf8 importUri, Utf8 path, const std::vector<TypeInfo::Ptr> &types){
    return ModuleInfo::Ptr(new ModuleInfo(importUri, path, types));
}

ModuleInfo::~ModuleInfo(){
}

const Utf8 &ModuleInfo::importUri() const{
    return m_importUri;
}

const Utf8 &ModuleInfo::path() const{
    return m_path;
}

size_t ModuleInfo::totalTypes() const{
    return m_types.size();
}

TypeInfo::ConstPtr ModuleInfo::typeAt(size_t index) const{
    return m_types.at(index);
}

void ModuleInfo::addType(const TypeInfo::Ptr &type){
    m_types.push_back(type);
}

void ModuleInfo::updateScanStatus(ModuleInfo::ScanStatus status){
    m_scanStatus = status;
}

size_t ModuleInfo::totalUnresolvedDocuments() const{
    return m_unresolvedDocuments.size();
}

const DocumentInfo::Ptr &ModuleInfo::unresolvedDocumentAt(size_t index) const{
    return m_unresolvedDocuments.at(index);
}

void ModuleInfo::addUnresolvedDocument(const DocumentInfo::Ptr &document){
    m_unresolvedDocuments.push_back(document);
}

size_t ModuleInfo::totalDependencies() const{
    return m_dependencies.size();
}

const Utf8 &ModuleInfo::dependencyAt(size_t index) const{
    return m_dependencies.at(index);
}

void ModuleInfo::addDependency(const Utf8 &dep){
    for ( auto it = m_dependencies.begin(); it != m_dependencies.end(); ++it )
        if ( *it == dep )
            return;
    m_dependencies.push_back(dep);
}

MLNode ModuleInfo::toMLNode() const
{
    MLNode result(MLNode::Object);

    result["import_uri"] = m_importUri.data();
    result["path"] = m_path.data();
    result["status"] = m_scanStatus;

    MLNode deps(MLNode::Array);
    for (int i = 0; i<m_dependencies.size();++i)
        deps.append(m_dependencies[i].data());
    result["dependencies"] = deps;

    MLNode dox(MLNode::Array);
    for (int i=0;i<m_unresolvedDocuments.size();++i)
        dox.append(m_unresolvedDocuments[i]->toMLNode());
    result["unresolved"] = dox;

    MLNode types(MLNode::Array);
    for (int i =0; i<m_types.size();++i)
        types.append(m_types[i]->toMLNode());
    result["types"] = types;

    return result;
}

void ModuleInfo::fromMLNode(const MLNode &node)
{
    auto object = node.asObject();
    m_importUri = object.at("import_uri").asString();
    m_path = object.at("path").asString();
    m_scanStatus = static_cast<ModuleInfo::ScanStatus>(object.at("status").asInt());

    auto deps = object.at("dependencies").asArray();
    for (int i =0; i<deps.size();++i)
    {
        m_dependencies.push_back(deps.at(i).asString());
    }

    auto unr = object.at("unresolved").asArray();
    for (int i =0; i < unr.size();++i)
    {
        DocumentInfo::Ptr di = DocumentInfo::create();
        di->fromMLNode(unr.at(i));
        m_unresolvedDocuments.push_back(di);
    }

    auto types = object.at("types").asArray();
    for (int i =0; i<types.size();++i)
    {
        TypeInfo::Ptr ti = TypeInfo::create("","", false, false);
        ti->fromMLNode(types.at(i));
        m_types.push_back(ti);
    }
}

// ImportInfo
// ------------------------------------------------------------

ImportInfo::ImportInfo(const std::vector<Utf8> &segments, const Utf8 &importAs, bool isRelative)
    : m_isRelative(isRelative)
    , m_segments(segments)
    , m_importAs(importAs)
{
}

bool ImportInfo::isRelative() const{
    return m_isRelative;
}

const Utf8 &ImportInfo::importAs() const{
    return m_importAs;
}

size_t ImportInfo::totalSegments() const{
    return m_segments.size();
}

const Utf8 &ImportInfo::segmentAt(size_t index) const{
    return m_segments.at(index);
}

std::vector<std::string> ImportInfo::segments() const{
    std::vector<std::string> result;
    for ( auto it = m_segments.begin(); it != m_segments.end(); ++it )
        result.push_back((*it).data());
    return result;
}

MLNode ImportInfo::toMLNode() const
{
    MLNode result(MLNode::Object);

    result["is_relative"] = m_isRelative;
    MLNode segs(MLNode::Array);
    for (int i=0; i<m_segments.size();++i)
    {
        segs.append(m_segments[i].data());
    }
    result["segments"] = segs;

    if (m_importAs != "")
        result["import_as"] = m_importAs.data();

    return result;
}

void ImportInfo::fromMLNode(const MLNode &node)
{
    MLNode::ObjectType obj = node.asObject();

    m_isRelative = obj.at("is_relative").asBool();
    MLNode::ArrayType arr = obj.at("segments").asArray();

    for (int i=0;i<arr.size();++i)
    {
        m_segments.push_back(Utf8(arr.at(i).asString()));
    }

    if (obj.find("import_as") != obj.end())
        m_importAs = obj.at("import_as").asString();
}

}}// namespace lv, el
