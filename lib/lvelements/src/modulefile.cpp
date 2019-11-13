#include "modulefile.h"
#include "imports_p.h"
#include "live/elements/languageparser.h"
#include "live/exception.h"
#include "live/plugin.h"
#include "live/plugincontext.h"
#include "live/packagegraph.h"
#include "live/elements/engine.h"
#include "live/elements/script.h"

#include <sstream>

namespace lv{ namespace el{

class ModuleFilePrivate{
public:
    std::string name;
    ElementsPlugin::Ptr plugin;

    ModuleFile::State state;

    std::list<std::string> exportNames;

    std::list<ModuleFile*> dependencies;
    std::list<ModuleFile*> dependents;

    Script::Ptr script;

    Imports* imports;
    Object* exports;
};

ModuleFile::~ModuleFile(){
    delete m_d->imports;
    delete m_d->exports;
    delete m_d;
}

LocalValue ModuleFile::get(Engine* engine, ModuleFile *from, const std::string &name){
    if ( m_d->state == ModuleFile::Ready )
        return LocalObject(*m_d->exports).get(engine, name);

    if ( from == this )
        THROW_EXCEPTION(
            lv::Exception,
            "Requiring the same module \'" + m_d->name + "\' in " + m_d->plugin->plugin()->context()->importId,
            1);

    if ( from->plugin().get() == plugin().get() ){ // within the same plugin

        if ( !hasDependency(from, this) ){
            from->m_d->dependencies.push_back(this);
            m_d->dependents.push_back(from);

            PackageGraph::CyclesResult<ModuleFile*> cr = checkCycles(from);
            if ( cr.found() ){
                std::stringstream ss;

                for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
                    ModuleFile* n = *it;
                    if ( it != cr.path().begin() )
                        ss << " -> ";
                    ss << n->name();
                }

                for ( auto it = from->m_d->dependencies.begin(); it != from->m_d->dependencies.end(); ++it ){
                    if ( *it == this ){
                        from->m_d->dependencies.erase(it);
                        break;
                    }
                }
                for ( auto it = m_d->dependents.begin(); it != m_d->dependents.end(); ++it ){
                    if ( *it == from ){
                        m_d->dependents.erase(it);
                        break;
                    }
                }

                THROW_EXCEPTION(lv::Exception, "Module file dependency cycle found: "  + ss.str(), 4);
            }
        }

    } else { // add a plugin dependency instead
        PackageGraph* pg = plugin()->plugin()->context()->packageGraph;
        pg->addDependency(from->plugin()->plugin(), plugin()->plugin());
    }


    m_d->state = ModuleFile::Loading;

    m_d->script  = engine->compileElement(plugin()->plugin()->path() + "/" + m_d->name + ".lv");
    initializeImportsExports(engine);
    m_d->script->loadAsModule(this);

    m_d->state = ModuleFile::Ready;

    return LocalObject(*m_d->exports).get(engine, name);
}

void ModuleFile::parse(Engine* engine){
    std::string fp = filePath();
    if ( engine && engine->moduleFileType() == Engine::JsOnly ){
        fp += ".js";
    }
    m_d->exportNames = LanguageParser::parseExportNames(fp);
    m_d->state = ModuleFile::Parsed;
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

std::string ModuleFile::filePath() const{
    return m_d->plugin->plugin()->path() + "/" + fileName();
}

const ElementsPlugin::Ptr &ModuleFile::plugin() const{
    return m_d->plugin;
}

const std::list<std::string> &ModuleFile::exports() const{
    return m_d->exportNames;
}

Imports *ModuleFile::imports(){
    return m_d->imports;
}

LocalValue ModuleFile::createObject(Engine* engine) const{
    v8::Local<v8::Object> obj = v8::Object::New(engine->isolate());
    v8::Local<v8::Value> exports = LocalValue(engine, *m_d->exports).data();

    v8::Local<v8::String> exportsKey = v8::String::NewFromUtf8(
        engine->isolate(), "exports", v8::String::kInternalizedString
    );
    v8::Local<v8::String> pathKey = v8::String::NewFromUtf8(
        engine->isolate(), "path", v8::String::kInternalizedString
    );
    v8::Local<v8::String> pathStr = v8::String::NewFromUtf8(
        engine->isolate(), filePath().c_str(), v8::String::kInternalizedString
    );

    obj->Set(exportsKey, exports);
    obj->Set(pathKey, pathStr);

    return LocalValue(obj);
}

void ModuleFile::initializeImportsExports(Engine *engine){
    m_d->imports = new Imports(engine, this);
    m_d->exports = new Object(engine);
    *m_d->exports = Object::create(engine);
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

ModuleFile::ModuleFile(const ElementsPlugin::Ptr &plugin, const std::string& name)
    : m_d(new ModuleFilePrivate)
{
    m_d->plugin = plugin;
    m_d->name = name;
    m_d->state = ModuleFile::Initiaized;
    m_d->imports = nullptr;
    m_d->exports = nullptr;
}

}} // namespace lv, el
