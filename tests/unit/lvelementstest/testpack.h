#ifndef TESTPACK_H
#define TESTPACK_H
#include <string>

class TestPack
{
public:
    TestPack(std::string path);
    ~TestPack() { clear(); }
    void unpack(std::string filePath);
    void clear();
private:
    std::string m_path;
};

#endif // TESTPACK_H
