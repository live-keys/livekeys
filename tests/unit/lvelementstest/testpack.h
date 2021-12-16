#ifndef TESTPACK_H
#define TESTPACK_H

#include <string>
#include "live/utf8.h"

class TestPack{

public:
    TestPack(std::string path, bool clearOnDestruct = true);
    ~TestPack();

    void unpack(std::string filePath);
    void clear();

    std::string listLocation();
    const std::string& path() const;

private:
    DISABLE_COPY(TestPack);

    bool        m_clearOnDestruction;
    std::string m_path;
};

inline const std::string &TestPack::path() const{
    return m_path;
}

#endif // TESTPACK_H
