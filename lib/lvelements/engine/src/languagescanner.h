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

#ifndef LVLANGUAGESCANNER_H
#define LVLANGUAGESCANNER_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/compiler/languageinfo.h"
#include "live/elements/compiler/languageparser.h"
#include "live/packagegraph.h"
#include "live/fileio.h"

#include <memory>
#include <functional>

namespace lv{ namespace el{

class LV_ELEMENTS_ENGINE_EXPORT LanguageScanner{

public:
    typedef std::shared_ptr<LanguageScanner>       Ptr;
    typedef std::shared_ptr<const LanguageScanner> ConstPtr;

public:
    ~LanguageScanner();

    static LanguageScanner::Ptr create(LanguageParser::Ptr parser, FileIOInterface* io);

    ModuleInfo::Ptr parseModule(const std::string& importUri);
    void onModuleReady(const std::function<void(ModuleInfo::ConstPtr)> callback);

    void queueModule(const std::string& importUri);
    void queueModule(const ModuleInfo::Ptr& module);

    void consumeQueue(size_t max);

    bool hasModule(const std::string &importUri);
    std::list<ModuleInfo::Ptr>::iterator locateModuleInQueue(const std::string& importUri);

    void setPackageImportPaths(const std::vector<std::string>& paths);
    const std::vector<std::string>& packageImportPaths() const;

private:
    LanguageScanner(LanguageParser::Ptr parser, FileIOInterface *io);
    DISABLE_COPY(LanguageScanner);

    void resolveModule(ModuleInfo::Ptr module);

    PackageGraph* m_packageGraph;

    std::list<ModuleInfo::Ptr>                  m_modulesToLoad;
    std::map<std::string, ModuleInfo::ConstPtr> m_loadedModules;
    std::function<void(ModuleInfo::ConstPtr)>   m_onModuleReady;

    LanguageParser::Ptr                         m_parser;
    FileIOInterface*                            m_io;
};

inline void LanguageScanner::onModuleReady(const std::function<void(ModuleInfo::ConstPtr)> callback){
    m_onModuleReady = callback;
}

}} // namespace lv, el

#endif // LVLANGUAGESCANNER_H
