#include "testpack.h"

#include <fstream>
#include <cctype>
#include <QDir>
#include <QDirIterator>

#include "live/exception.h"
#include "live/visuallog.h"

TestPack::TestPack(std::string path)
{
    m_path = path;
}

void TestPack::unpack(std::string filePath)
{
    std::ifstream inputStream(filePath);
    if (!inputStream.is_open())
        THROW_EXCEPTION(lv::Exception, "Can't find the file " + filePath, 1);

    // create test path first

    QDir tempdir(m_path.c_str());
    if (!tempdir.exists())
        tempdir.mkpath(".");

    std::string line;
    std::ofstream outputStream;

    while (std::getline(inputStream, line))
    {
        if (line.length() > 6 && line.substr(0, 6) == "//////")
        {
            std::string outputFileName = line.substr(6);

            outputFileName.erase(outputFileName.begin(), std::find_if(outputFileName.begin(), outputFileName.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            outputFileName.erase(std::find_if(outputFileName.rbegin(), outputFileName.rend(), [](int ch) {
                    return !std::isspace(ch);
            }).base(), outputFileName.end());

            if (outputFileName.length() > 0){

                size_t fileNameIndex = outputFileName.rfind('/');
                if ( fileNameIndex != std::string::npos ){
                    QDir tempdir(QString::fromStdString(m_path + "/" + outputFileName.substr(0, fileNameIndex)));
                    if (!tempdir.exists())
                        tempdir.mkpath(".");
                }

                if (outputStream.is_open() )
                    outputStream.close();
                outputStream.open((m_path + "/" + outputFileName).c_str());
            }
            continue;
        }

        if (outputStream.is_open())
            outputStream << line << std::endl;

    }
    outputStream.close();
}

void TestPack::clear(){
    QDir dir(m_path.c_str());
    dir.removeRecursively();
}

std::string TestPack::listLocation(){
    QDirIterator dit(QString::fromStdString(m_path), QDirIterator::Subdirectories);
    std::string result;
    while ( dit.hasNext() ){
        dit.next();
        QFileInfo next = dit.fileInfo();
        if ( next.fileName() != "." && next.fileName() != ".."){
            if ( next.isFile() ){
                std::string sizeStr = QString::number(next.size()).toStdString();
                result += next.filePath().mid((int)m_path.size()).toStdString() + " [Size:" + sizeStr + "]\n";
            } else {
                result += next.filePath().mid((int)m_path.size()).toStdString() + "\n";
            }
        }

    }

    return result;
}


