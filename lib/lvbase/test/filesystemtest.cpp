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

#include "catch_library.h"
#include "live/visuallog.h"
#include "live/applicationcontext.h"
#include "live/fileio.h"
#include "live/path.h"
#include "live/directory.h"

#include <map>

using namespace lv;

namespace{

std::string workPath(){
    return Path::temporaryDirectory();
}

}

TEST_CASE( "FileSystem Test", "[FileSystem]" ) {
    SECTION("Test WorkPath"){
        REQUIRE(Path::exists(workPath()));
    }
    SECTION("Test Directory Creation"){
        std::string newdir = Path::join(workPath(), "newdir");
        if ( Path::exists(newdir)){
            REQUIRE(Path::remove(newdir));
        }

        REQUIRE(Path::createDirectory(newdir));
        REQUIRE(Path::exists(newdir));
        REQUIRE(Path::isDir(newdir));
        REQUIRE(!Path::isFile(newdir));
        REQUIRE(!Path::isSymlink(newdir));
        REQUIRE(Path::name(newdir) == "newdir");
        REQUIRE(Path::absolutePath(newdir) == newdir);

        std::string newdir_a = Path::join(newdir, "a");
        REQUIRE(Path::createDirectory(newdir_a));
        REQUIRE(Path::exists(newdir_a));
        REQUIRE(Path::name(newdir_a) == "a");
        REQUIRE(Path::parent(newdir_a) == newdir);

        std::string newdir_b_c = Path::join(newdir, "b/c");
        REQUIRE(Path::createDirectories(newdir_b_c));
        REQUIRE(Path::exists(newdir_b_c));
        REQUIRE(Path::name(newdir_b_c) == "c");

        std::string newdir_b_d = Path::join(newdir, "b/d");
        Path::rename(newdir_b_c, newdir_b_d);
        REQUIRE(!Path::exists(newdir_b_c));
        REQUIRE(Path::exists(newdir_b_d));

        std::string newdir_b_d_unresolved = Path::join(newdir_b_d, "../../b/d");
        REQUIRE(Path::resolve(newdir_b_d_unresolved) == Path::resolve(newdir_b_d));

        auto parts = Path::split(newdir_b_d);
        REQUIRE(parts.size() >= 2);
        REQUIRE(parts[parts.size() - 2] == "b");
        REQUIRE(parts[parts.size() - 1] == "d");

        std::string newdir_es = Path::join(newdir, "es");

#ifdef PLATFORM_OS_UNIX
        Path::createSymlink(newdir_es, newdir_b_d);
        REQUIRE(Path::exists(newdir_es));
        REQUIRE(Path::isSymlink(newdir_es));
        REQUIRE(Path::followSymlink(newdir_es) == newdir_b_d);
#endif

        REQUIRE(Path::relativePath(newdir, newdir) == "");
        REQUIRE(Path::relativePath(newdir, newdir_b_d) == "b/d");
        REQUIRE(Path::relativePath(newdir_b_d, newdir) == "../..");
        REQUIRE(Path::relativePath(newdir_b_d, newdir_a) == "../../a");
        REQUIRE(Path::remove(newdir));
    }

    SECTION("Test File Creation"){
        std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

        std::string newdir = Path::join(workPath(), "newdir");
        if ( Path::exists(newdir)){
            REQUIRE(Path::remove(newdir));
        }
        REQUIRE(Path::createDirectory(newdir));

        std::string newfile = Path::join(newdir, "newfile.txt");

        REQUIRE(fio->writeToFile(newfile, "test"));
        REQUIRE(Path::exists(newfile));
        REQUIRE(!Path::isDir(newfile));
        REQUIRE(!Path::isSymlink(newfile));
        REQUIRE(Path::isFile(newfile));
        REQUIRE(Path::name(newfile) == "newfile.txt");
        REQUIRE(Path::baseName(newfile) == "newfile");
        REQUIRE(Path::extension(newfile) == ".txt");
        REQUIRE(Path::suffix(newfile) == "txt");
        REQUIRE(Path::completeSuffix(newfile) == "txt");
        REQUIRE(Path::hasExtensions(newfile, {"txt"}));

        std::string newfile_h = Path::join(newdir, "newfile.txt.h");
        REQUIRE(fio->writeToFile(newfile_h, "test"));
        REQUIRE(Path::exists(newfile_h));
        REQUIRE(Path::name(newfile_h) == "newfile.txt.h");
        REQUIRE(Path::baseName(newfile_h) == "newfile");
        REQUIRE(Path::extension(newfile_h) == ".h");
        REQUIRE(Path::suffix(newfile_h) == "h");
        REQUIRE(Path::completeSuffix(newfile_h) == "txt.h");
        REQUIRE(Path::hasExtensions(newfile_h, {"txt.h", "h"}));
        REQUIRE(Path::hasExtensions(newfile_h, {"txt", "h"}));
        REQUIRE(!Path::hasExtensions(newfile_h, {"txt"}));

        REQUIRE(Path::relativePath(newfile, newfile_h) == "newfile.txt.h");
        REQUIRE(Path::relativePath(newfile, newdir) == "");

        REQUIRE(Path::remove(newdir));
    }
    SECTION("Test Copy"){
        std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

        std::string newdir = Path::join(workPath(), "newdir");
        if ( Path::exists(newdir)){
            REQUIRE(Path::remove(newdir));
        }
        REQUIRE(Path::createDirectory(newdir));
        REQUIRE(Path::exists(newdir));
        REQUIRE(Path::isDir(newdir));

        std::string newdir_a = Path::join(newdir, "a");
        std::string newdir_a_b = Path::join(newdir_a, "b");
        std::string newdir_a_c = Path::join(newdir_a, "c");
        REQUIRE(Path::createDirectory(newdir_a));
        REQUIRE(Path::createDirectory(newdir_a_b));
        REQUIRE(Path::createDirectory(newdir_a_c));
        std::string newdir_a_c_file = Path::join(newdir_a_c, "file.txt");
        REQUIRE(fio->writeToFile(newdir_a_c_file, "file1"));

        std::string newdir_acopy = Path::join(newdir, "acopy");

        bool caughtException = false;
        try{
            Path::copyFile(newdir_a, newdir_acopy);
        } catch ( lv::Exception& e ){
            caughtException = true;
            REQUIRE(e.code() == lv::Exception::toCode("~File"));
        }
        REQUIRE(caughtException);

        std::string newdir_file = Path::join(newdir, "file.txt");
        Path::copyFile(newdir_a_c_file, newdir_file);
        REQUIRE(Path::exists(newdir_file));
        REQUIRE(fio->readFromFile(newdir_file) == "file1");


        caughtException = false;
        try{
            std::string newdir_file_fail = Path::join(newdir, "file_fail.txt");
            Path::copyRecursive(newdir_a_c_file, newdir_file_fail);
        } catch ( lv::Exception& e ){
            caughtException = true;
            REQUIRE(e.code() == lv::Exception::toCode("File"));
        }
        REQUIRE(caughtException);

        REQUIRE(!Path::exists(newdir_acopy));
        Path::copyRecursive(newdir_a, newdir_acopy);
        REQUIRE(Path::exists(Path::join(newdir_acopy, "b")));
        REQUIRE(Path::exists(Path::join(newdir_acopy, "c")));
        REQUIRE(Path::exists(Path::join(newdir_acopy, "c/file.txt")));
        REQUIRE(fio->readFromFile(Path::join(newdir_acopy, "c/file.txt")) == "file1");

        REQUIRE(Path::remove(newdir));
    }
    SECTION("Test Directory Iterator"){
        std::unique_ptr<FileIO> fio = std::make_unique<FileIO>();

        std::string newdir = Path::join(workPath(), "newdir");
        if ( Path::exists(newdir)){
            REQUIRE(Path::remove(newdir));
        }
        REQUIRE(Path::createDirectory(newdir));

        REQUIRE(fio->writeToFile(Path::join(newdir, "file1.txt"), "test1"));
        REQUIRE(fio->writeToFile(Path::join(newdir, "file2.txt"), "test2"));
        REQUIRE(fio->writeToFile(Path::join(newdir, "file3.txt"), "test3"));

        std::map<std::string, bool> paths;
        lv::Directory::Iterator dit = lv::Directory::iterate(newdir);
        while ( !dit.isEnd() ){
            paths[Path::name(dit.path())] = true;
            dit.next();
        }

        REQUIRE(paths.size() == 3);
        REQUIRE(paths["file1.txt"] == true);
        REQUIRE(paths["file2.txt"] == true);
        REQUIRE(paths["file3.txt"] == true);

        REQUIRE(Path::remove(newdir));
    }
}

