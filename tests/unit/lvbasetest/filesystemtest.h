
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

#ifndef FILESYSTEMTEST_H
#define FILESYSTEMTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/mlnode.h"

class FileSystemTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    FileSystemTest(QObject* parent = 0);
    ~FileSystemTest(){}

private slots:
    void initTestCase();
    void testWorkPath();
    void testDirectoryCreation();
    void testFileCreation();
    void testCopy();
    void testDirectoryIterator();

private:
    std::string workPath();

};

#endif // FILESYSTEMTEST_H
