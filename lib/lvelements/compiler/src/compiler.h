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

#ifndef LVCOMPILER
#define LVCOMPILER

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/elements/compiler/languageparser.h"
#include "live/utf8.h"
#include "live/fileio.h"
#include "live/package.h"
#include "live/module.h"

namespace lv{

class MLNode;

namespace el{

class BaseNode;
class ProgramNode;
class ElementsModule;
class CompilerPrivate;
class LV_ELEMENTS_COMPILER_EXPORT Compiler{

public:
    typedef std::shared_ptr<Compiler>       Ptr;
    typedef std::shared_ptr<const Compiler> ConstPtr;

    class LV_ELEMENTS_COMPILER_EXPORT Config{

        friend class Compiler;
        friend class CompilerPrivate;

    public:
        Config(
            bool fileOutput = true,
            const std::string& outputExtension = ".js",
            FileIOInterface* ioInterface = new FileIO()
        );

        bool hasCustomBaseComponent();
        void addImplicitType(const std::string& typeName);
        void addImportPath(const std::string& path);
        void setBaseComponent(const std::string& name, const std::string& importUri);
        void initialize(const MLNode& config);
        void allowUnresolvedTypes(bool allow){ m_allowUnresolved = allow; }

    private:
        bool                   m_fileOutput;
        bool                   m_fileOutputOnlyOnModified;
        FileIOInterface*       m_fileIO;
        std::string            m_outputExtension;
        std::string            m_elementPath;
        std::string            m_baseComponent;
        std::string            m_baseComponentUri;
        std::list<std::string> m_implicitTypes;
        std::list<std::string> m_importPaths;
        std::string            m_importLocalPath;
        std::string            m_packageBuildPath;
        bool                   m_enableJsImports;
        bool                   m_allowUnresolved;
    };

public:
    ~Compiler();

    static Ptr create(const Config& config = Config(), PackageGraph* pg = nullptr);

    FileIOInterface* fileIO() const;

    const std::list<std::string>& importPaths() const;

    std::string compileToJs(const std::string& path, const std::string& contents);
    std::string compileToJs(const std::string& path, const std::string& contents, LanguageParser::AST* ast);
    std::string compileToJs(const std::string& path, const std::string& content, BaseNode* node);
    std::string compileModuleFileToJs(const Module::Ptr& plugin, const std::string& path, const std::string& content, BaseNode* node);

    const std::string& packageBuildPath() const;
    std::string moduleFileBuildPath(const Module::Ptr& plugin, const std::string& path);
    std::string moduleBuildPath(const Module::Ptr& module);

    std::vector<BaseNode*> collectProgramExports(const std::string& contents, ProgramNode* node);
    ProgramNode* parseProgramNodes(const std::string& filePath, const std::string &fileName, LanguageParser::AST* ast);

    const std::string& outputExtension() const;
    const std::string& importLocalPath() const;
    const LanguageParser::Ptr& parser() const;

    void configureImplicitType(const std::string& type);

    static std::shared_ptr<ElementsModule> compile(Compiler::Ptr compiler, const std::string& path, Engine* engine = nullptr);
    static std::shared_ptr<ElementsModule> compileModule(Compiler::Ptr compiler, const std::string& path, Engine* engine = nullptr);
    static std::vector<std::shared_ptr<ElementsModule> > compilePackage(Compiler::Ptr compiler, const std::string& path, Engine* engine = nullptr);
    static std::shared_ptr<ElementsModule> compileImportedModule(Compiler::Ptr compiler, const std::string& path, const Module::Ptr& requstingModule, Engine* engine = nullptr);

    const std::vector<std::string> &packageImportPaths() const;
    void setPackageImportPaths(const std::vector<std::string>& paths);

    std::shared_ptr<ElementsModule> findLoadedModuleByPath(const std::string& path) const;

private:
    std::string createModuleBuildPath(const Module::Ptr& plugin);

    Compiler(const Config& config, PackageGraph *pg);

    DISABLE_COPY(Compiler);

    CompilerPrivate* m_d;
};

}}// namespace lv,  el

#endif // LVCOMPILER
