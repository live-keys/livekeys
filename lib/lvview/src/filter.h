/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef LVFILTER_H
#define LVFILTER_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include <functional>

namespace lv{

class FilterWorker;

/// @private
class LV_VIEW_EXPORT Filter{

private:
    FilterWorker* m_workerThread = 0;

public:
    Filter();
    virtual ~Filter();

    void setWorkerThread(FilterWorker* worker){
        m_workerThread = worker;
    }

    FilterWorker* workerThread(){
        return m_workerThread;
    }

    virtual void process(){}

    void use(
        Shared::ReadScope* locker,
        const std::function<void()>& cb,
        const std::function<void()>& rs);

};

}// namespace

#endif // LVFILTER_H
