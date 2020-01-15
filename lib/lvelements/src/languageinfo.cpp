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

}}// namespace lv, el
