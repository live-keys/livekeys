#ifndef LVPARSER_H
#define LVPARSER_H

#include <list>
#include <string>

namespace lv{ namespace el{

class Parser{

public:
    Parser();

    static std::list<std::string> parseExportNames(const std::string &moduleFile);

private:
    static std::list<std::string> parseExportNamesJs(const std::string& jsModuleFile);
};

}} // namespace lv, el

#endif // LVPARSER_H
