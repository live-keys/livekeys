#include "modulefile.h"
#include "languagenodes_p.h"
#include "live/elements/compiler/languageparser.h"
#include "live/exception.h"
#include "live/module.h"
#include "live/modulecontext.h"
#include "live/packagegraph.h"

#include <sstream>

namespace lv{ namespace el{

class ModuleFilePrivate{
public:
    std::string name;
    std::string content;
    ElementsModule::Ptr elementsModule;
    ProgramNode* rootNode;
    ModuleFile::CompilationData* compilationData;

    ModuleFile::State state;

    std::list<ModuleFile::Export> exports;
    std::list<ModuleFile::Import> imports;
    std::list<ModuleFile*> dependencies;
    std::list<ModuleFile*> dependents;
};

ModuleFile::~ModuleFile(){
    delete m_d->compilationData;
    delete m_d;
}

void ModuleFile::resolveTypes(){
    auto impTypes = m_d->rootNode->importTypes();
    for ( auto nsit = impTypes.begin(); nsit != impTypes.end(); ++nsit ){
        for ( auto it = nsit->second.begin(); it != nsit->second.end(); ++it ){
            ProgramNode::ImportType& impType = it->second;
            bool foundLocalExport = false;
            if ( impType.importNamespace.empty() ){
                auto foundExp = m_d->elementsModule->findExport(impType.name);
                if ( foundExp.isValid() ){
                    addDependency(foundExp.file);

                    m_d->rootNode->resolveImport(impType.importNamespace, impType.name, "./" + foundExp.file->jsFileName());
                    foundLocalExport = true;
                }
            }

            if ( !foundLocalExport ){
                for( auto impIt = m_d->imports.begin(); impIt != m_d->imports.end(); ++impIt ){
                    if ( impIt->as == impType.importNamespace ){
                        auto foundExp = impIt->module->findExport(impType.name);
                        if ( foundExp.isValid() ){
                            if ( impIt->isRelative ){
                                // this plugin to package
                                std::vector<Utf8> parts = Utf8(m_d->elementsModule->module()->context()->importId).split(".");
                                if ( parts.size() > 0 ){
                                    parts.erase(parts.begin());
                                }

                                std::string pluginToPackage = "";
                                if ( parts.size() > 0 ){
                                    for ( size_t i = 0; i < parts.size(); ++i ){
                                        if ( !pluginToPackage.empty() )
                                            pluginToPackage += '/';
                                        pluginToPackage += "..";
                                    }
                                } else {
                                    pluginToPackage = ".";
                                }

                                // package to new plugin

                                std::vector<Utf8> packageToNewPluginParts = Utf8(impIt->module->module()->context()->importId).split(".");
                                if ( packageToNewPluginParts.size() > 0 ){
                                    packageToNewPluginParts.erase(packageToNewPluginParts.begin());
                                }
                                std::string packageToNewPlugin = Utf8::join(packageToNewPluginParts, "/").data();
                                if ( !packageToNewPlugin.empty() )
                                    packageToNewPlugin += "/";

                                m_d->rootNode->resolveImport(impType.importNamespace, impType.name, pluginToPackage + "/" + packageToNewPlugin + foundExp.file->jsFileName());

                            } else {
                                std::vector<Utf8> packageToPlugin = impIt->module->module()->context()->importId.split(".");
                                packageToPlugin.erase(packageToPlugin.begin());

                                std::string configPackageBuildPath = m_d->elementsModule->compiler()->packageBuildPath();
                                std::string packageBuildPath =
                                    impIt->module->module()->context()->package->name() +
                                    (configPackageBuildPath.empty() ? "" : "/" + configPackageBuildPath);
                                std::string packageToPluginStr = Utf8::join(packageToPlugin, "/").data();
                                std::string importPath = packageBuildPath + (packageToPluginStr.empty() ? "" : "/" + packageToPluginStr) + "/" + foundExp.file->jsFileName();

                                m_d->rootNode->resolveImport(impType.importNamespace, impType.name, importPath);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ModuleFile::compile(){
    m_d->elementsModule->compiler()->compileModuleFileToJs(m_d->elementsModule->module(), filePath(), m_d->content, m_d->rootNode);
}

ModuleFile::State ModuleFile::state() const{
    return m_d->state;
}

const std::string &ModuleFile::name() const{
    return m_d->name;
}

std::string ModuleFile::fileName() const{
    return m_d->name + ".lv";
}

std::string ModuleFile::jsFileName() const{
    return fileName() + m_d->elementsModule->compiler()->outputExtension();
}

std::string ModuleFile::jsFilePath() const{
    return m_d->elementsModule->compiler()->moduleBuildPath(m_d->elementsModule->module()) + "/" + jsFileName();
}

std::string ModuleFile::filePath() const{
    return m_d->elementsModule->module()->path() + "/" + fileName();
}

const ElementsModule::Ptr &ModuleFile::module() const{
    return m_d->elementsModule;
}

const std::list<ModuleFile::Export> &ModuleFile::exports() const{
    return m_d->exports;
}

const std::list<ModuleFile::Import> &ModuleFile::imports() const{
    return m_d->imports;
}

void ModuleFile::resolveImport(const std::string &uri, ElementsModule::Ptr epl){
    for ( auto it = m_d->imports.begin(); it != m_d->imports.end(); ++it ){
        if ( it->uri == uri )
            it->module = epl;
    }
}

void ModuleFile::addDependency(ModuleFile *dependency){
    if ( dependency == this )
        return;
    m_d->dependencies.push_back(dependency);
    dependency->m_d->dependents.push_back(this);

    PackageGraph::CyclesResult<ModuleFile*> cr = checkCycles(this);
    if ( cr.found() ){
        std::stringstream ss;

        for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
            ModuleFile* n = *it;
            if ( it != cr.path().begin() )
                ss << " -> ";
            ss << n->name();
        }

        for ( auto it = m_d->dependencies.begin(); it != m_d->dependencies.end(); ++it ){
            if ( *it == dependency ){
                m_d->dependencies.erase(it);
                break;
            }
        }
        for ( auto it = dependency->m_d->dependents.begin(); it != dependency->m_d->dependents.end(); ++it ){
            if ( *it == this ){
                dependency->m_d->dependents.erase(it);
                break;
            }
        }

        THROW_EXCEPTION(lv::Exception, "Module file dependency cycle found: "  + ss.str(), lv::Exception::toCode("Cycle"));
    }
}

void ModuleFile::setCompilationData(CompilationData *cd){
    if ( m_d->compilationData )
        delete m_d->compilationData;
    m_d->compilationData = cd;
}

bool ModuleFile::hasDependency(ModuleFile *module, ModuleFile *dependency){
    for ( auto it = module->m_d->dependencies.begin(); it != module->m_d->dependencies.end(); ++it ){
        if ( *it == dependency )
            return true;
    }
    return false;
}

PackageGraph::CyclesResult<ModuleFile *> ModuleFile::checkCycles(ModuleFile *mf){
    std::list<ModuleFile*> path;
    path.push_back(mf);

    for ( auto it = mf->m_d->dependencies.begin(); it != mf->m_d->dependencies.end(); ++it ){
        PackageGraph::CyclesResult<ModuleFile*> cr = checkCycles(mf, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<ModuleFile*>(PackageGraph::CyclesResult<ModuleFile*>::NotFound);;
}

PackageGraph::CyclesResult<ModuleFile*> ModuleFile::checkCycles(
        ModuleFile *mf, ModuleFile *current, std::list<ModuleFile *> path)
{
    path.push_back(current);
    if ( current == mf )
        return PackageGraph::CyclesResult<ModuleFile*>(PackageGraph::CyclesResult<ModuleFile*>::Found, path);

    for ( auto it = current->m_d->dependencies.begin(); it != current->m_d->dependencies.end(); ++it ){
        PackageGraph::CyclesResult<ModuleFile*> cr = checkCycles(mf, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<ModuleFile*>(PackageGraph::CyclesResult<ModuleFile*>::NotFound);
}

ModuleFile::ModuleFile(ElementsModule::Ptr plugin, const std::string &name, const std::string &content, ProgramNode *node)
    : m_d(new ModuleFilePrivate)
{
    std::string componentName = name;
    size_t extension = componentName.find(".lv");
    if ( extension != std::string::npos )
        componentName = componentName.substr(0, extension);

    m_d->elementsModule = plugin;
    m_d->name = componentName;
    m_d->state = ModuleFile::Initiaized;
    m_d->content = content;
    m_d->rootNode = node;
    m_d->compilationData = nullptr;

    std::vector<BaseNode*> exports = m_d->elementsModule->compiler()->collectProgramExports(content, node);

    for ( auto val : exports ){
        if ( val->typeString() == "ComponentInstanceStatement" ){
            auto expression = val->as<ComponentInstanceStatementNode>();
            ModuleFile::Export expt;
            expt.type = ModuleFile::Export::Element;
            expt.name = expression->name(content);
            m_d->exports.push_back(expt);

        } else if ( val->typeString() == "ComponentDeclaration"){
            auto expression = val->as<ComponentDeclarationNode>();
            ModuleFile::Export expt;
            expt.type = ModuleFile::Export::Component;
            expt.name = expression->name(content);
            m_d->exports.push_back(expt);
        }
    }

    std::vector<ImportNode*> imports = node->imports();
    for ( auto val : imports ){
        ModuleFile::Import imp;
        imp.uri = val->path(content);
        imp.as = val->as(content);
        imp.isRelative = val->isRelative();
        m_d->imports.push_back(imp);
    }
}

}} // namespace lv, el
