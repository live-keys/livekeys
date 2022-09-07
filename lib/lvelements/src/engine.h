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

#ifndef LVENGINE_H
#define LVENGINE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/value.h"
#include "live/elements/script.h"
#include "live/elements/jsmodule.h"

#include "live/elements/compiler/elementsmodule.h"
#include "live/elements/compiler/languageparser.h"
#include "live/elements/compiler/compiler.h"

#include "live/packagegraph.h"

namespace lv{

class Exception;

namespace el{

class Context;
class ComponentTemplate;
class EnginePrivate;
class LV_ELEMENTS_EXPORT Engine{

    friend class EnginePrivate;
    friend class Script;
    friend class Element;
    friend class ElementPrivate;

    // Initialization Functionality
    // ----------------------------

public:
    class InitializeData;
    class LV_ELEMENTS_EXPORT InitializeScope{
    public:
        InitializeScope(const std::string& defaultLocation);
        ~InitializeScope();
    };

    static void initialize(const std::string& defaultLocation);
    static void dispose();
    static bool isInitialized();
    static void disposeAtExit();

    // Exception CatchData
    // -------------------

    class LV_ELEMENTS_EXPORT CatchData{

        friend class Engine;
        friend class Script;

    public:
        bool canContinue() const;
        bool hasTerminated() const;
        void rethrow();
        std::string message() const;
        std::string stack() const;
        std::string fileName() const;
        int lineNumber() const;
        Element* object() const;

    private:
        CatchData(Engine* engine, v8::TryCatch* tc);

        Engine*       m_engine;
        v8::TryCatch* m_tryCatch;
    };

private:
    static InitializeData* m_initializeData;

    // Engine
    // ------

public:
    enum ModuleFileType{
        Lv = 0,
        LvOrJs,
        JsOnly
    };

public:
    Engine(const Compiler::Ptr& compiler = Compiler::create());
    ~Engine();

    Object require(ModuleLibrary* module, const Object& o);

    void scope(const std::function<void()> &f);

    v8::Isolate* isolate();

    Context* currentContext() const;

    Script::Ptr compileJs(const std::string& str);
    Script::Ptr compileJsEnclosed(const std::string& str);
    Script::Ptr compileJsModuleFile(const std::string& path);

    JsModule::Ptr loadJsModule(const std::string& path);
    Element* runFile(const std::string& path);

    ComponentTemplate* registerTemplate(const MetaObject* mo);
    v8::Local<v8::FunctionTemplate> pointTemplate();
    v8::Local<v8::FunctionTemplate> sizeTemplate();
    v8::Local<v8::FunctionTemplate> rectangleTemplate();
    v8::Local<v8::FunctionTemplate> importsTemplate();

    bool isElementConstructor(const Callable& c);

    void throwError(const lv::Exception* exception, Element* object);
    void tryCatch(const std::function<void()>& f, const std::function<void(const CatchData&)>& c);

    bool hasPendingException();
    void incrementTryCatchNesting();
    void decrementTryCatchNesting();
    void clearPendingException();

    void handleError(const std::string& message, const std::string& stack, const std::string& file, int line);

    ModuleFileType moduleFileType() const;
    void setModuleFileType(ModuleFileType type);

    const LanguageParser::Ptr& parser() const;
    const Compiler::Ptr& compiler() const;

private:
    void importInternals();
    void wrapScriptObject(Element* element);

    int tryCatchNesting();
    void setGlobalErrorHandler(bool value);


    EnginePrivate* m_d;
};

}} // namespace lv, el

#endif // LVENGINE_H
