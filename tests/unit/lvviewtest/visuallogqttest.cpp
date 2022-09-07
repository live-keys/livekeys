/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#include "visuallogqttest.h"
#include "visuallogteststub.h"
#include "live/visuallog.h"
#include "live/exception.h"
#include "live/visuallogmodel.h"

#include <QQmlEngine>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTemporaryFile>

Q_TEST_RUNNER_REGISTER(VisualLogQtTest);

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

VisualLogQtTest::VisualLogQtTest(QObject *parent)
    : QObject(parent)
    , m_engine(0)
    , m_vlogModel(0)
{
}

VisualLogQtTest::~VisualLogQtTest(){
}

void VisualLogQtTest::initTestCase(){
    m_engine = new QQmlEngine;
    m_vlogModel = new VisualLogModel(m_engine);

    vlog().configure("global", {
        {"defaultLevel", "Info"},
        {"toConsole",    false}
    });
}

void VisualLogQtTest::cleanupTestCase(){
    delete m_vlogModel;
    delete m_engine;
}

void VisualLogQtTest::levelObjectTest(){
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

    QCOMPARE(ts->objects[0].second["type"].asString(), MLNode::StringType("VisualLogQtTestStub"));
    QCOMPARE(ts->objects[0].second["field1"].asInt(),  100);

    QCOMPARE(ts->objects[1].second["type"].asString(), MLNode::StringType("VisualLogQtTestStub"));
    QCOMPARE(ts->objects[1].second["field1"].asInt(),  100);

    ts->messages.clear();
    vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Info}
    });

    vlog("test").e(teststub);
    vlog("test").i(teststub);
    vlog("test").d(teststub);

    QCOMPARE(ts->messages.size(), 2);

    QCOMPARE(ts->objects[0].second["type"].asString(), MLNode::StringType("VisualLogQtTestStub"));
    QCOMPARE(ts->objects[0].second["field1"].asInt(),  100);

    QCOMPARE(ts->objects[1].second["type"].asString(), MLNode::StringType("VisualLogQtTestStub"));
    QCOMPARE(ts->objects[1].second["field1"].asInt(),  100);

    vlog().removeTransports("test");
}

void VisualLogQtTest::viewOutputTest(){
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
