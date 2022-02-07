/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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
