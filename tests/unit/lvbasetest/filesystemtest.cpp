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

#include "filesystemtest.h"
#include "live/visuallog.h"
#include "live/applicationcontext.h"
#include "live/fileio.h"
#include "live/path.h"
#include "live/directory.h"

#include <map>

Q_TEST_RUNNER_REGISTER(FileSystemTest);

using namespace lv;

class Directory{

public:
    class IteratorPrivate;
    class Iterator{
    };

private:
    Directory();
};


FileSystemTest::FileSystemTest(QObject *parent)
    : QObject(parent)
{
}

void FileSystemTest::initTestCase(){
}

void FileSystemTest::testWorkPath(){
    QVERIFY(Path::exists(workPath()));
}

void FileSystemTest::testDirectoryCreation(){
    std::string newdir = Path::join(workPath(), "newdir");
    if ( Path::exists(newdir)){
        QVERIFY(Path::remove(newdir));
    }

    QVERIFY(Path::createDirectory(newdir));
    QVERIFY(Path::exists(newdir));
    QVERIFY(Path::isDir(newdir));
    QVERIFY(!Path::isFile(newdir));
    QVERIFY(!Path::isSymlink(newdir));
    QVERIFY(Path::name(newdir) == "newdir");
    QVERIFY(Path::absolutePath(newdir) == newdir);

    std::string newdir_a = Path::join(newdir, "a");
    QVERIFY(Path::createDirectory(newdir_a));
    QVERIFY(Path::exists(newdir_a));
    QVERIFY(Path::name(newdir_a) == "a");
    QVERIFY(Path::parent(newdir_a) == newdir);

    std::string newdir_b_c = Path::join(newdir, "b/c");
    QVERIFY(Path::createDirectories(newdir_b_c));
    QVERIFY(Path::exists(newdir_b_c));
    QVERIFY(Path::name(newdir_b_c) == "c");

    std::string newdir_b_d = Path::join(newdir, "b/d");
    Path::rename(newdir_b_c, newdir_b_d);
    QVERIFY(!Path::exists(newdir_b_c));
    QVERIFY(Path::exists(newdir_b_d));

    std::string newdir_b_d_unresolved = Path::join(newdir_b_d, "../../b/d");
    QVERIFY(Path::resolve(newdir_b_d_unresolved) == Path::resolve(newdir_b_d));

    auto parts = Path::split(newdir_b_d);
    QVERIFY(parts.size() >= 2);
    QVERIFY(parts[parts.size() - 2] == "b");
    QVERIFY(parts[parts.size() - 1] == "d");

    std::string newdir_es = Path::join(newdir, "es");
    Path::createSymlink(newdir_es, newdir_b_d);
    QVERIFY(Path::exists(newdir_es));
    QVERIFY(Path::isSymlink(newdir_es));
    QVERIFY(Path::followSymlink(newdir_es) == newdir_b_d);

    QVERIFY(Path::relativePath(newdir, newdir) == "");
    QVERIFY(Path::relativePath(newdir, newdir_b_d) == "b/d");
    QVERIFY(Path::relativePath(newdir_b_d, newdir) == "../..");
    QVERIFY(Path::relativePath(newdir_b_d, newdir_a) == "../../a");

    QVERIFY(Path::remove(newdir));
}

void FileSystemTest::testFileCreation(){
    std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

    std::string newdir = Path::join(workPath(), "newdir");
    if ( Path::exists(newdir)){
        QVERIFY(Path::remove(newdir));
    }
    QVERIFY(Path::createDirectory(newdir));

    std::string newfile = Path::join(newdir, "newfile.txt");

    QVERIFY(fio->writeToFile(newfile, "test"));
    QVERIFY(Path::exists(newfile));
    QVERIFY(!Path::isDir(newfile));
    QVERIFY(!Path::isSymlink(newfile));
    QVERIFY(Path::isFile(newfile));
    QVERIFY(Path::name(newfile) == "newfile.txt");
    QVERIFY(Path::baseName(newfile) == "newfile");
    QVERIFY(Path::extension(newfile) == ".txt");
    QVERIFY(Path::suffix(newfile) == "txt");
    QVERIFY(Path::completeSuffix(newfile) == "txt");
    QVERIFY(Path::hasExtensions(newfile, {"txt"}));

    std::string newfile_h = Path::join(newdir, "newfile.txt.h");
    QVERIFY(fio->writeToFile(newfile_h, "test"));
    QVERIFY(Path::exists(newfile_h));
    QVERIFY(Path::name(newfile_h) == "newfile.txt.h");
    QVERIFY(Path::baseName(newfile_h) == "newfile");
    QVERIFY(Path::extension(newfile_h) == ".h");
    QVERIFY(Path::suffix(newfile_h) == "h");
    QVERIFY(Path::completeSuffix(newfile_h) == "txt.h");
    QVERIFY(Path::hasExtensions(newfile_h, {"txt.h", "h"}));
    QVERIFY(Path::hasExtensions(newfile_h, {"txt", "h"}));
    QVERIFY(!Path::hasExtensions(newfile_h, {"txt"}));

    QVERIFY(Path::relativePath(newfile, newfile_h) == "newfile.txt.h");
    QVERIFY(Path::relativePath(newfile, newdir) == "");

    QVERIFY(Path::remove(newdir));
}

void FileSystemTest::testCopy(){
    std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

    std::string newdir = Path::join(workPath(), "newdir");
    if ( Path::exists(newdir)){
        QVERIFY(Path::remove(newdir));
    }
    QVERIFY(Path::createDirectory(newdir));
    QVERIFY(Path::exists(newdir));
    QVERIFY(Path::isDir(newdir));

    std::string newdir_a = Path::join(newdir, "a");
    std::string newdir_a_b = Path::join(newdir_a, "b");
    std::string newdir_a_c = Path::join(newdir_a, "c");
    QVERIFY(Path::createDirectory(newdir_a));
    QVERIFY(Path::createDirectory(newdir_a_b));
    QVERIFY(Path::createDirectory(newdir_a_c));
    std::string newdir_a_c_file = Path::join(newdir_a_c, "file.txt");
    QVERIFY(fio->writeToFile(newdir_a_c_file, "file1"));

    std::string newdir_acopy = Path::join(newdir, "acopy");

    bool caughtException = false;
    try{
        Path::copyFile(newdir_a, newdir_acopy);
    } catch ( lv::Exception& e ){
        caughtException = true;
        QVERIFY(e.code() == lv::Exception::toCode("~File"));
    }
    QVERIFY(caughtException);

    std::string newdir_file = Path::join(newdir, "file.txt");
    Path::copyFile(newdir_a_c_file, newdir_file);
    QVERIFY(Path::exists(newdir_file));
    QVERIFY(fio->readFromFile(newdir_file) == "file1");


    caughtException = false;
    try{
        std::string newdir_file_fail = Path::join(newdir, "file_fail.txt");
        Path::copyRecursive(newdir_a_c_file, newdir_file_fail);
    } catch ( lv::Exception& e ){
        caughtException = true;
        QVERIFY(e.code() == lv::Exception::toCode("File"));
    }
    QVERIFY(caughtException);

    QVERIFY(!Path::exists(newdir_acopy));
    Path::copyRecursive(newdir_a, newdir_acopy);
    QVERIFY(Path::exists(Path::join(newdir_acopy, "b")));
    QVERIFY(Path::exists(Path::join(newdir_acopy, "c")));
    QVERIFY(Path::exists(Path::join(newdir_acopy, "c/file.txt")));
    QVERIFY(fio->readFromFile(Path::join(newdir_acopy, "c/file.txt")) == "file1");

    QVERIFY(Path::remove(newdir));
}

void FileSystemTest::testDirectoryIterator(){
    std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

    std::string newdir = Path::join(workPath(), "newdir");
    if ( Path::exists(newdir)){
        QVERIFY(Path::remove(newdir));
    }
    QVERIFY(Path::createDirectory(newdir));

    QVERIFY(fio->writeToFile(Path::join(newdir, "file1.txt"), "test1"));
    QVERIFY(fio->writeToFile(Path::join(newdir, "file2.txt"), "test2"));
    QVERIFY(fio->writeToFile(Path::join(newdir, "file3.txt"), "test3"));

    std::map<std::string, bool> paths;
    lv::Directory::Iterator dit = lv::Directory::iterate(newdir);
    while ( !dit.isEnd() ){
        paths[Path::name(dit.path())] = true;
        dit.next();
    }

    QVERIFY(paths.size() == 3);
    QVERIFY(paths["file1.txt"] == true);
    QVERIFY(paths["file2.txt"] == true);
    QVERIFY(paths["file3.txt"] == true);

    QVERIFY(Path::remove(newdir));
}

std::string FileSystemTest::workPath(){
    return Path::temporaryDirectory();
}

