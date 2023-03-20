/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "catch_library.h"
#include "live/visuallog.h"
#include "live/exception.h"
#include "live/fileio.h"

#include <vector>
#include <utility>

using namespace lv;

namespace{

class VisualLogTransportStub : public VisualLog::Transport{

    // Transport interface
public:
    void onMessage(const VisualLog::Configuration *configuration,
                   const VisualLog::MessageInfo &messageInfo,
                   const std::string&message) override
    {
        messages.push_back(std::make_pair(messageInfo.prefix(configuration), message));
    }

    void onObject(const VisualLog::Configuration *configuration,
                  const VisualLog::MessageInfo &messageInfo,
                  const std::string& type,
                  const MLNode &node) override
    {
        MLNode storedNode = node;
        storedNode["type"] = type;
        objects.push_back(std::make_pair(messageInfo.prefix(configuration), storedNode));
    }

public:
    std::vector<std::pair<std::string, std::string> > messages;
    std::vector<std::pair<std::string, MLNode> >  objects;
};

class RunOnce{
public:
    static void setup(){
        static bool hasSetup = false;
        if ( !hasSetup ){
            hasSetup =  true;
            vlog().configure("global", {
                {"defaultLevel", "Info"},
                {"toConsole",    false}
            });
        }
    }

};

}// namespace


TEST_CASE( "VisualLog Test", "[VisualLog]" ) {

    RunOnce::setup();

    SECTION("Test Levels"){
        VisualLogTransportStub* ts = new VisualLogTransportStub;
        vlog().addTransport("test", ts);
        vlog().configure("test", {
            {"level", VisualLog::MessageInfo::Error},
            {"defaultLevel",     VisualLog::MessageInfo::Info}
        });

        vlog("test").f() << "test" << " " << "fatal";
        vlog("test").e() << "test" << " " << "error";
        vlog("test")     << "test" << " " << "info";

        REQUIRE(ts->messages.size() == 2);
        REQUIRE(ts->messages[0].second == "test fatal");
        REQUIRE(ts->messages[1].second == "test error");


        ts->messages.clear();
        vlog().configure("test", {
            {"defaultLevel", VisualLog::MessageInfo::Error}
        });

        vlog("test") << "test" << " " << "error";
        REQUIRE(ts->messages.size() == 1);
        REQUIRE(ts->messages[0].second == "test error");

        ts->messages.clear();
        vlog().configure("test", {
            {"level", VisualLog::MessageInfo::Info}
        });

        vlog("test").e() << "test" << " " << "error";
        vlog("test").i() << "test" << " " << "info";
        vlog("test").d() << "test" << " " << "debug";

        REQUIRE(ts->messages.size() == 2);
        REQUIRE(ts->messages[0].second == "test error");
        REQUIRE(ts->messages[1].second == "test info");
        REQUIRE(ts->objects.size() == 0);

        vlog().removeTransports("test");
    }
    SECTION("Test Prefix"){
        VisualLogTransportStub* ts = new VisualLogTransportStub;

        vlog().addTransport("test", ts);
        vlog().configure("test", {
            {"level", VisualLog::MessageInfo::Info},
            {"defaultLevel",     VisualLog::MessageInfo::Info},
            {"prefix",           "%F|%N|%U|%L|%V|%v"}
        });

        vlog("test").at("test/visuallogtest.cpp", 100, "prefixTest") << "test" << " " << "info";

        REQUIRE(ts->messages.size() == 1);

        auto prefixSegments = Utf8(ts->messages[0].first).split("|");
        REQUIRE(prefixSegments.size() == 6);
        REQUIRE(prefixSegments[0] == "test/visuallogtest.cpp");
        REQUIRE(prefixSegments[1] == "visuallogtest.cpp");
        REQUIRE(prefixSegments[2] == "prefixTest");
        REQUIRE(prefixSegments[3] == "100");
        REQUIRE(prefixSegments[4] == "Info");
        REQUIRE(prefixSegments[5] == "info");

        vlog().configure("test", {{"prefix", ""}});
        vlog().removeTransports("test");
    }
    SECTION("Test File Output"){
        std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

        std::string workPath = Path::temporaryDirectory();
        REQUIRE(Path::exists(workPath));


        std::string tempFilePath = Path::join(workPath, "newfile.txt");

        REQUIRE(fio->writeToFile(tempFilePath, ""));
        REQUIRE(Path::exists(tempFilePath));


        vlog().configure("test", {
            {"level",         VisualLog::MessageInfo::Info},
            {"defaultLevel",  VisualLog::MessageInfo::Info},
            {"file",          tempFilePath},
        });
        vlog("test")     << "test" << " " << "info";
        vlog("test").d() << "test" << " " << "debug";

        std::string contents = fio->readFromFile(tempFilePath);
        REQUIRE(contents == "test info\n");
    }
    SECTION("Test Daily File Output"){
        std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

        std::string workPath = Path::temporaryDirectory();

        vlog().configure("test", {
            {"level",        VisualLog::MessageInfo::Info},
            {"defaultLevel", VisualLog::MessageInfo::Info},
            {"file",         MLNode::StringType(workPath + "/_temp_.txt")},
            {"logDaily",     true}
        });

        vlog("test")     << "test" << " " << "info";
        vlog("test").d() << "test" << " " << "debug";


        std::string contents = fio->readFromFile(workPath + "/_temp_.txt");
        REQUIRE(contents == "test info\n");
    }
}

