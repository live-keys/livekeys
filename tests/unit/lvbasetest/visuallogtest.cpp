/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
#include "visuallogteststub.h"
#include "live/visuallog.h"
#include "live/exception.h"
#include "live/visuallogmodel.h"

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
                   const QString &message) Q_DECL_OVERRIDE
    {
        messages.append(QPair<QString, QString>(messageInfo.prefix(configuration), message));
    }

    void onObject(const VisualLog::Configuration *configuration,
                  const VisualLog::MessageInfo &messageInfo,
                  const QString& type,
                  const MLNode &node) Q_DECL_OVERRIDE
    {
        MLNode storedNode = node;
        storedNode["type"] = type.toStdString();
        objects.append(QPair<QString, MLNode>(messageInfo.prefix(configuration), storedNode));
    }

public:
    QList<QPair<QString, QString> > messages;
    QList<QPair<QString, MLNode> >  objects;
};

VisualLogTest::VisualLogTest(QObject *parent)
    : QObject(parent)
    , m_engine(0)
    , m_vlogModel(0)
{
}

VisualLogTest::~VisualLogTest(){
}

void VisualLogTest::initTestCase(){
    m_engine = new QQmlEngine;
    m_vlogModel = new VisualLogModel(m_engine);

    vlog().configure("global", {
        {"defaultLevel", "Info"},
        {"toConsole",    false}
    });
}

void VisualLogTest::cleanupTestCase(){
    delete m_vlogModel;
    delete m_engine;
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

void VisualLogTest::levelObjectTest(){
    VisualLogTransportStub* ts = new VisualLogTransportStub;
    VisualLogTestStub teststub(100, "test");

    vlog().addTransport("test", ts);
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Error},
        {"defaultLevel",     VisualLog::MessageInfo::Info}
    });

    vlog("test").f(teststub);
    vlog("test").e(teststub);

    QCOMPARE(ts->objects.size(), 2);

    QCOMPARE(ts->objects[0].second["type"].asString(), MLNode::StringType("VisualLogTestStub"));
    QCOMPARE(ts->objects[0].second["field1"].asInt(),  100);

    QCOMPARE(ts->objects[1].second["type"].asString(), MLNode::StringType("VisualLogTestStub"));
    QCOMPARE(ts->objects[1].second["field1"].asInt(),  100);

    ts->messages.clear();
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info}
    });

    vlog("test").e(teststub);
    vlog("test").i(teststub);
    vlog("test").d(teststub);

    QCOMPARE(ts->messages.size(), 2);

    QCOMPARE(ts->objects[0].second["type"].asString(), MLNode::StringType("VisualLogTestStub"));
    QCOMPARE(ts->objects[0].second["field1"].asInt(),  100);

    QCOMPARE(ts->objects[1].second["type"].asString(), MLNode::StringType("VisualLogTestStub"));
    QCOMPARE(ts->objects[1].second["field1"].asInt(),  100);

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

void VisualLogTest::viewOutputTest(){
    QQmlEngine engine;
    VisualLogModel vlm(&engine);

    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info},
        {"defaultLevel",     VisualLog::MessageInfo::Info}
    });

    VisualLog::setViewTransport(&vlm);

    vlog("test")     << "test" << " " << "info";
    vlog("test").d() << "test" << " " << "debug";

    QCOMPARE(vlm.rowCount(QModelIndex()), 1);
    QCOMPARE(vlm.entryAt(0).data, QString("test info"));

    VisualLog::setViewTransport(0);
}
