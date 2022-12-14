#include "testmodule.h"
#include "testcase.h"
#include "tester.h"
#include "scenario.h"

#include "live/elements/modulelibrary.h"

void TestModuleLoader::load(lv::el::ModuleLibrary *library){
    library->addType<lv::el::Tester>();
    library->addType<lv::el::TestCase>();
    library->addType<lv::el::Scenario>();
}
