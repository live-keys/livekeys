#include <QCoreApplication>
#include <QTest>
#include <qqml.h>

#include "testrunner.h"
#include "lvlinecontroltest.h"
#include "live/visuallog.h"
#include <iostream>

int main(int argc, char *argv[]){

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    int result = -1;
    try {
        result = lv::TestRunner::runTests(argc, argv);
    } catch (std::exception e)
    {
        vlog("test-main") << e.what();
    }
    return result;
}
