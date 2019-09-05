#include "parser.h"
#include "live/visuallog.h"

#include <fstream>

namespace lv{ namespace el{

Parser::Parser(){

}

std::list<std::string> Parser::parseExportNames(const std::string& moduleFile){
    if ( moduleFile.rfind(".lv.js") != std::string::npos ){
        return parseExportNamesJs(moduleFile);
    }

    std::list<std::string> exportNames;
    return exportNames;
}

std::list<std::string> Parser::parseExportNamesJs(const std::string &jsModuleFile){
    std::ifstream instream(jsModuleFile, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        THROW_EXCEPTION(lv::Exception, std::string("Cannot open file: ") + jsModuleFile, Exception::toCode("~Path"));
    }

    instream.seekg(0, std::ios::end);
    size_t size = instream.tellg();
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    std::list<std::string> exportNames;

    //TODO: Quick hack for testing purposes, this will need implementation
    size_t start = 0;
    while ((start = buffer.find("module.exports[\"", start)) != std::string::npos) {
        size_t end = buffer.find("\"]", start);
        if ( end != std::string::npos ){
            exportNames.push_back(buffer.substr(start + 16, end - start - 16));
        }
        start = end + 1;
    }

    return exportNames;
}

}} // namespace lv, el
