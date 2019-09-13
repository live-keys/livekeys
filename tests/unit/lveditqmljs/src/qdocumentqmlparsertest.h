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

#ifndef QDOCUMENTQMLPARSERTEST_H
#define QDOCUMENTQMLPARSERTEST_H

#include <QObject>
#include "qtestrunner.h"

class QDocumentQmlParserTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    QDocumentQmlParserTest(QObject* parent = 0);
    ~QDocumentQmlParserTest();

private:
    QString filePath(const QString& relativePath);
    QString readFile(const QString& path);

    void initTestCase();
    void identifierTest();
    void identifierValueTest();
    void identifierFunctionMemberTest();
    void identifierMemberTest();
    void identifierScopeTest();
    void rangeScopeTest();



private slots:
};

#endif // QDOCUMENTQMLPARSERTEST_H
