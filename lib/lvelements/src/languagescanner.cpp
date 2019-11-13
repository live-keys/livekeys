#include "languagescanner.h"
#include "live/plugincontext.h"
#include "live/elements/parseddocument.h"

namespace lv{ namespace el{

LanguageScanner::LanguageScanner(LanguageParser::Ptr parser, LockedFileIOSession::Ptr io)
    : m_packageGraph(new PackageGraph)
    , m_parser(parser)
    , m_io(io)
{
}

void LanguageScanner::resolveModule(ModuleInfo::Ptr module){
    size_t totalDocs = module->totalUnresolvedDocuments();
    for ( size_t i = 0; i < totalDocs; ++i ){
        DocumentInfo::Ptr doc = module->unresolvedDocumentAt(i);
        size_t totalTypes = doc->totalTypes();
        for ( size_t t = 0; t < totalTypes; ++t ){
            TypeInfo::Ptr type = doc->typeAt(t);
            //TODO: For name, instance, and each property in this type, resolve
            // it's type from the loaded modules
        }
    }
}

LanguageScanner::~LanguageScanner(){
}

LanguageScanner::Ptr LanguageScanner::create(LanguageParser::Ptr parser, LockedFileIOSession::Ptr io){
    return LanguageScanner::Ptr(new LanguageScanner(parser, io));
}

/**
 * \brief Parses this module, acquiring all dependencies and parsing all of its documents
 *
 * The documents are parsed in isolation, meaning the types won't be resolved as this module
 * is parsed.
 */
ModuleInfo::Ptr LanguageScanner::parseModule(const std::string &importUri){
    Plugin::Ptr plugin = m_packageGraph->loadPlugin(importUri);

    if ( !plugin || !plugin->context() )
        return ModuleInfo::Ptr(nullptr);

    //TODO: Check live.types file first.

    ModuleInfo::Ptr module = ModuleInfo::create(plugin->context()->importId, plugin->path());

    const std::list<Plugin::Ptr>& localdeps = plugin->context()->localDependencies;
    for ( auto it = localdeps.begin(); it != localdeps.end(); ++it ){
        Plugin::Ptr dep = *it;
        module->addDependency(dep->context()->importId);
    }

    const std::list<std::string>& deps = plugin->dependencies();
    for ( auto it = deps.begin(); it != deps.end(); ++it ){
        std::string dep = *it;
        module->addDependency(dep);
    }

    const std::list<std::string>& files = plugin->modules();
    for ( auto it = files.begin(); it != files.end(); ++it ){
        std::string filePath = plugin->filePath() + "/" + *it + ".lv";
        std::string content = m_io->readFromFile(filePath);
        LanguageParser::AST* ast = m_parser->parse(content);
        DocumentInfo::Ptr info = ParsedDocument::extractInfo(content, ast);

        for ( size_t i = 0; i < info->totalImports(); ++i ){
            Plugin::Ptr depPlugin = m_packageGraph->loadPlugin(info->importAt(i).segments(), plugin);
            if ( depPlugin && depPlugin->context() )
                module->addDependency(depPlugin->context()->importId);
        }

        module->addUnresolvedDocument(info);
        m_parser->destroy(ast);
    }

    module->updateScanStatus(ModuleInfo::Parsed);
    return module;
}

void LanguageScanner::queueModule(const ModuleInfo::Ptr &module){
    auto it = locateModuleInQueue(module->importUri().data());
    if ( it != m_modulesToLoad.end() )
        return;

    size_t totalDependencies = module->totalDependencies();
    for ( size_t i = 0; i < totalDependencies; ++i ){
        Utf8 dep = module->dependencyAt(i);
        if ( !hasModule(dep.data()) ){
            auto it = locateModuleInQueue(dep.data());
            if ( it == m_modulesToLoad.end() ){
                ModuleInfo::Ptr depModule = parseModule(dep.data());
                queueModule(depModule);
            }
        }
    }

    m_modulesToLoad.push_back(module);
}

void LanguageScanner::consumeQueue(size_t max){
    while ( m_modulesToLoad.size() > 0 && max > 0 ){
        ModuleInfo::Ptr module = m_modulesToLoad.front();
        m_modulesToLoad.pop_front();

        bool resolvedAllDependencies = true;
        size_t totalDependencies = module->totalDependencies();
        for ( size_t i = 0; i < totalDependencies; ++i ){
            Utf8 dep = module->dependencyAt(i);
            if ( !hasModule(dep.data()) ){
                resolvedAllDependencies = false;
            }
        }

        if ( !resolvedAllDependencies ){
            queueModule(module);
        } else {
            resolveModule(module);
            m_loadedModules[module->importUri().data()] = module;
            m_onModuleReady(module);
        }

        --max;
    }

}

bool LanguageScanner::hasModule(const std::string& importUri){
    return (m_loadedModules.find(importUri) == m_loadedModules.end());
}

std::list<ModuleInfo::Ptr>::iterator LanguageScanner::locateModuleInQueue(const std::string& importUri){
    for ( auto it = m_modulesToLoad.begin(); it != m_modulesToLoad.end(); ++it ){
        if ( (*it)->importUri().data() == importUri ){
            return it;
        }
    }
    return m_modulesToLoad.end();
}

}} // namespace lv, el
