#include "testpack.h"

//#include <experimental/filesystem>
//#include <fstream>
//#include <cctype>
//#include "qdebug.h"
//#include <direct.h>

#define fs std::experimental::filesystem::v1

TestPack::TestPack(std::string path)
{
    m_path = path;
}

void TestPack::unpack(std::string filePath)
{
//    std::ifstream inputStream(filePath);
//    if (!inputStream.is_open())
//    {
//        qDebug() << "Can't find the file " << filePath.c_str();
//        return;
//    }

//    std::string line;
//    std::ofstream outputStream;
//    while (getline(inputStream, line))
//    {
//        if (line.length() > 6 && line.substr(0, 6) == "//////")
//        {
//            std::string outputFileName = line.substr(6);

//            outputFileName.erase(outputFileName.begin(), std::find_if(outputFileName.begin(), outputFileName.end(), [](int ch) {
//                return !std::isspace(ch);
//            }));
//            outputFileName.erase(std::find_if(outputFileName.rbegin(), outputFileName.rend(), [](int ch) {
//                    return !std::isspace(ch);
//            }).base(), outputFileName.end());

//            std::replace(outputFileName.begin(), outputFileName.end(), '/', '\\');

//            std::string temp = "";
//            std::vector<std::string> pathParts;
//            for (unsigned i = 0; i < outputFileName.length(); ++i)
//            {
//                if (outputFileName[i] != '\\')
//                    temp += outputFileName[i];
//                else {
//                    if (temp != "") pathParts.push_back(temp);
//                    temp = "";
//                }
//            }
//            if (temp != "") pathParts.push_back(temp);

//            temp = m_path;
//            if (outputFileName.length() > 0)
//            {
//                for (unsigned i = 0; i<pathParts.size()-1; ++i)
//                {
//                    temp += "\\" + pathParts[i];
//                    if (!fs::exists(temp))
//                        _mkdir(temp.c_str());
//                }
//                outputStream.close();
//                outputStream.open((m_path + "\\" + outputFileName).c_str());
//            }
//            continue;
//        }

//        if (outputStream.is_open())
//            outputStream << line << std::endl;

//    }
//    outputStream.close();
}

void TestPack::clear()
{
//    fs::remove_all(fs::path(m_path));
//    _mkdir(m_path.c_str());
}


