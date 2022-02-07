#ifndef LVMODULEFILEJSDATA_H
#define LVMODULEFILEJSDATA_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/compiler/modulefile.h"
#include "live/elements/jsmodule.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT ModuleFileJsData : public ModuleFile::CompilationData{

public:
    ModuleFileJsData(const JsModule::Ptr& jsModule);

private:
    JsModule::Ptr m_jsModule;
};

}}// namespace lv, el

#endif // LVMODULEFILEJSDATA_H
