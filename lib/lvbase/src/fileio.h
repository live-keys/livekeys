#ifndef LVFILEIOINTERFACE_H
#define LVFILEIOINTERFACE_H

#include "live/lvbaseglobal.h"
#include <string>

namespace lv{

class LV_BASE_EXPORT FileIOInterface{

public:
    FileIOInterface();
    virtual ~FileIOInterface();

    virtual std::string readFromFile(const std::string& path) = 0;
    virtual bool writeToFile(const std::string& path, const std::string& content) = 0;
    virtual bool writeToFile(const std::string& path, const char* content, size_t length) = 0;
};

class LV_BASE_EXPORT FileIO : public FileIOInterface{

public:
    FileIO();
    ~FileIO() override;

    std::string readFromFile(const std::string& path) override;
    bool writeToFile(const std::string& path, const std::string& content) override;
    bool writeToFile(const std::string& path, const char* content, size_t length) override;

};

}// namespace

#endif // FILEIOINTERFACE_H
