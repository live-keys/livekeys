#ifndef TESTPACK_H
#define TESTPACK_H

#include <string>

class TestPack{

public:
    TestPack(std::string path);
    ~TestPack() { clear(); }

    void unpack(std::string filePath);
    void clear();

    std::string listLocation();
    const std::string& path() const;

private:
    std::string m_path;
};

inline const std::string &TestPack::path() const{
    return m_path;
}

#endif // TESTPACK_H
