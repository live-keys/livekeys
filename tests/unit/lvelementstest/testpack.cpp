#include "testpack.h"

#include <fstream>
#include <cctype>
#include "qdebug.h"
#include <direct.h>
#include "qdir.h"
#include "live/exception.h"

#define fs std::experimental::filesystem::v1

TestPack::TestPack(std::string path)
{
    m_path = path;
}

void TestPack::unpack(std::string filePath)
{
    std::ifstream inputStream(filePath);
    if (!inputStream.is_open())
        THROW_EXCEPTION(lv::Exception, "Can't find the file " + filePath, 1);

    std::string line;
    std::ofstream outputStream;
    while (getline(inputStream, line))
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

            std::replace(outputFileName.begin(), outputFileName.end(), '/', '\\');

            std::string temp = "";
            std::vector<std::string> pathParts;
            for (unsigned i = 0; i < outputFileName.length(); ++i)
            {
                if (outputFileName[i] != '\\')
                    temp += outputFileName[i];
                else {
                    if (temp != "") pathParts.push_back(temp);
                    temp = "";
                }
            }
            if (temp != "") pathParts.push_back(temp);

            temp = m_path;
            if (outputFileName.length() > 0)
            {
                for (unsigned i = 0; i<pathParts.size()-1; ++i)
                {
                    temp += "\\" + pathParts[i];
                    QDir tempdir(temp.c_str());
                    if (!tempdir.exists())
                        tempdir.mkpath(".");
                }
                outputStream.close();
                outputStream.open((m_path + "\\" + outputFileName).c_str());
            }
            continue;
        }

        if (outputStream.is_open())
            outputStream << line << std::endl;

    }
    outputStream.close();
}

void TestPack::clear()
{
    QString path = m_path.c_str();
    std::string folderName = m_path.substr(m_path.find_last_of('\\') + 1);
    QDir dir(path);
    dir.removeRecursively();
    path += "\\..\\";
    QDir create(path);
    create.mkpath(folderName.c_str());
}


