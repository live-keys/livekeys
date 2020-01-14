/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef FILTERTESTSTUB_H
#define FILTERTESTSTUB_H

#include <QObject>
#include "live/qmlact.h"
#include "shareddatateststub.h"

class FilterTestStub : public lv::QmlAct{

    Q_OBJECT
    Q_PROPERTY(SharedDataTestStub* input1 READ input1 WRITE setInput1 NOTIFY input1Changed)
    Q_PROPERTY(SharedDataTestStub* input2 READ input2 WRITE setInput2 NOTIFY input2Changed)
    Q_PROPERTY(SharedDataTestStub* output READ output NOTIFY outputChanged)

public:
    explicit FilterTestStub(QObject *parent = 0);
    ~FilterTestStub();

    void setInput1(SharedDataTestStub* input1);
    void setInput2(SharedDataTestStub* input2);

    void process();

    SharedDataTestStub* input1() const;
    SharedDataTestStub* input2() const;
    SharedDataTestStub* output() const;

signals:
    void input1Changed(SharedDataTestStub*);
    void input2Changed(SharedDataTestStub*);
    void outputChanged(SharedDataTestStub*);

private:
    SharedDataTestStub* m_input1;
    SharedDataTestStub* m_input2;
    SharedDataTestStub* m_output;
    int                 m_sleepTime;
};

inline void FilterTestStub::setInput1(SharedDataTestStub *input1){
    m_input1 = input1;
    emit input1Changed(input1);

    process();
}

inline void FilterTestStub::setInput2(SharedDataTestStub *input2){
    m_input2 = input2;
    emit input2Changed(input2);

    process();
}

inline SharedDataTestStub *FilterTestStub::input2() const{
    return m_input2;
}

inline SharedDataTestStub *FilterTestStub::output() const{
    return m_output;
}

inline SharedDataTestStub *FilterTestStub::input1() const{
    return m_input1;
}

#endif // FILTERTESTSTUB_H
