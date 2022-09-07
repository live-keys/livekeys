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

#include "visuallogtest.h"
#include "live/visuallog.h"
#include "live/exception.h"

#include <QQmlEngine>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTemporaryFile>

Q_TEST_RUNNER_REGISTER(VisualLogTest);

using namespace lv;

class VisualLogTransportStub : public VisualLog::Transport{

    // Transport interface
public:
    void onMessage(const VisualLog::Configuration *configuration,
                   const VisualLog::MessageInfo &messageInfo,
                   const std::string&message) Q_DECL_OVERRIDE
    {
        messages.append(QPair<QString, QString>(QString::fromStdString(messageInfo.prefix(configuration)), QString::fromStdString(message)));
    }

    void onObject(const VisualLog::Configuration *configuration,
                  const VisualLog::MessageInfo &messageInfo,
                  const std::string& type,
                  const MLNode &node) Q_DECL_OVERRIDE
    {
        MLNode storedNode = node;
        storedNode["type"] = type;
        objects.append(QPair<QString, MLNode>(QString::fromStdString(messageInfo.prefix(configuration)), storedNode));
    }

public:
    QList<QPair<QString, QString> > messages;
    QList<QPair<QString, MLNode> >  objects;
};

VisualLogTest::VisualLogTest(QObject *parent)
    : QObject(parent)
{
}

VisualLogTest::~VisualLogTest(){
}

void VisualLogTest::initTestCase(){
    vlog().configure("global", {
        {"defaultLevel", "Info"},
        {"toConsole",    false}
    });
}

void VisualLogTest::cleanupTestCase(){
}

void VisualLogTest::levelTest(){
    VisualLogTransportStub* ts = new VisualLogTransportStub;
    vlog().addTransport("test", ts);
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Error},
        {"defaultLevel",     VisualLog::MessageInfo::Info}
    });

    vlog("test").f() << "test" << " " << "fatal";
    vlog("test").e() << "test" << " " << "error";
    vlog("test")     << "test" << " " << "info";

    QCOMPARE(ts->messages.size(), 2);
    QCOMPARE(ts->messages[0].second, QString("test fatal"));
    QCOMPARE(ts->messages[1].second, QString("test error"));


    ts->messages.clear();
    vlog().configure("test", {
        {"defaultLevel", VisualLog::MessageInfo::Error}
    });

    vlog("test") << "test" << " " << "error";
    QCOMPARE(ts->messages.size(), 1);
    QCOMPARE(ts->messages[0].second, QString("test error"));

    ts->messages.clear();
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info}
    });

    vlog("test").e() << "test" << " " << "error";
    vlog("test").i() << "test" << " " << "info";
    vlog("test").d() << "test" << " " << "debug";

    QCOMPARE(ts->messages.size(), 2);
    QCOMPARE(ts->messages[0].second, QString("test error"));
    QCOMPARE(ts->messages[1].second, QString("test info"));
    QCOMPARE(ts->objects.size(), 0);

    vlog().removeTransports("test");
}

void VisualLogTest::prefixTest(){
    VisualLogTransportStub* ts = new VisualLogTransportStub;

    vlog().addTransport("test", ts);
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info},
        {"defaultLevel",     VisualLog::MessageInfo::Info},
        {"prefix",           "%F|%N|%U|%L|%V|%v"}
    });

    vlog("test").at("test/visuallogtest.cpp", 100, "prefixTest") << "test" << " " << "info";

    QCOMPARE(ts->messages.size(), 1);

    QStringList prefixSegments = ts->messages[0].first.split('|');

    QCOMPARE(prefixSegments.size(), 6);
    QCOMPARE(prefixSegments[0], QString("test/visuallogtest.cpp"));
    QCOMPARE(prefixSegments[1], QString("visuallogtest.cpp"));
    QCOMPARE(prefixSegments[2], QString("prefixTest"));
    QCOMPARE(prefixSegments[3], QString("100"));
    QCOMPARE(prefixSegments[4], QString("Info"));
    QCOMPARE(prefixSegments[5], QString("info"));

    vlog().configure("test", {{"prefix", ""}});
    vlog().removeTransports("test");
}

void VisualLogTest::fileOutputTest(){
    QTemporaryFile tf;
    if ( !tf.open() )
        return;

    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info},
        {"defaultLevel",     VisualLog::MessageInfo::Info},
        {"file",          tf.fileName().toStdString()},
    });
    tf.close();

    vlog("test")     << "test" << " " << "info";
    vlog("test").d() << "test" << " " << "debug";

    tf.open();
    QCOMPARE(tf.size(), 10);
    QCOMPARE(tf.readAll(), QByteArray("test info\n"));
    tf.close();

    vlog().configure("test", {{"file", tf.fileName().toStdString()}});
}

void VisualLogTest::dailyFileOutputTest(){
    QTemporaryDir dr;
    if ( !dr.isValid() )
        return;

    vlog().configure("test", {
        {"level",        VisualLog::MessageInfo::Info},
        {"defaultLevel", VisualLog::MessageInfo::Info},
        {"file",         MLNode::StringType((dr.path() + "/_temp_.txt").toStdString())},
        {"logDaily",     true}
    });

    vlog("test")     << "test" << " " << "info";
    vlog("test").d() << "test" << " " << "debug";

    QFile f(dr.path() + "/_temp_.txt");
    if ( f.open(QIODevice::ReadOnly) ){
        QCOMPARE(f.readAll(), QByteArray("test info\n"));
        f.close();
    }
    vlog("test").closeFile();
    f.remove();

    vlog().configure("test", {{"file", ""}, {"logDaily", false}});
}

