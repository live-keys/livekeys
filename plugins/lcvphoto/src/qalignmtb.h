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

#ifndef QALIGNMTB_H
#define QALIGNMTB_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"

class QAlignMTB : public QObject{

    Q_OBJECT
    Q_PROPERTY(int                maxBits       READ maxBits        WRITE setMaxBits        NOTIFY maxBitsChanged)
    Q_PROPERTY(int                excludeRange  READ excludeRange   WRITE setExcludeRange   NOTIFY excludeRangeChanged)
    Q_PROPERTY(bool               cut           READ cut            WRITE setCut            NOTIFY cutChanged)

public:
    explicit QAlignMTB(QObject *parent = nullptr);
    ~QAlignMTB();

    int maxBits() const;
    void setMaxBits(int maxBits);

    int excludeRange() const;
    void setExcludeRange(int excludeRange);

    bool cut() const;
    void setCut(bool cut);

public slots:
    lv::QmlObjectList* process(lv::QmlObjectList* input);
signals:
    void maxBitsChanged();
    void excludeRangeChanged();
    void cutChanged();

private:
    void createAlignMTB();

    lv::QmlObjectList*    m_input;
    cv::Ptr<cv::AlignMTB> m_alignMTB;
    int                   m_maxBits;
    int                   m_excludeRange;
    bool                  m_cut;
};

#endif // QALIGNMTB_H
